/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

/** iqplayer_lib.cpp
 *
 * \author:FrancoisTaburet: francois.taburet@nokia-bell-labs.com
 */
#define _LARGEFILE_SOURCE
#define _FILE_OFFSET_BITS 64
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/resource.h>
#include <errno.h>
#include <math.h>
#include <sys/time.h>
#include "common_lib.h"
#include "assertions.h"
#include "common/utils/LOG/log.h"

static void busy_wait(uint32_t usecs) {
  uint32_t elapsed_usecs;
  struct timespec requestStart, requestEnd;
  
  // Active wait
  clock_gettime(CLOCK_REALTIME, &requestStart);
  while (1) {
    clock_gettime(CLOCK_REALTIME, &requestEnd);
    elapsed_usecs = ( requestEnd.tv_sec - requestStart.tv_sec ) / 1e-6
      + ( requestEnd.tv_nsec - requestStart.tv_nsec ) / 1e3;
    if (elapsed_usecs >= usecs)
      break;
  }
  //LOG_I(HW, "Expected: %d Elapsed: %d us\n", usecs, elapsed_usecs);
}

static void parse_iqfile_header(openair0_device *device, iqfile_header_t *iq_fh) {
  char tmp[4]=OAIIQFILE_ID;
  AssertFatal((memcmp(iq_fh->oaiid,tmp,sizeof(iq_fh->oaiid)) == 0),
              "iqfile doesn't seem to be compatible with oai (invalid id %.4s in header)\n",
              iq_fh->oaiid);
  device->type = iq_fh->devtype;
  device->openair0_cfg[0].tx_sample_advance=iq_fh->tx_sample_advance;
  device->openair0_cfg[0].tx_bw =  device->openair0_cfg[0].rx_bw = iq_fh->bw;
  device->recplay_state->nbSamplesBlocks=iq_fh->nbSamplesBlocks;
  LOG_UI(HW,"Replay iqs from %s device, bandwidth %e\n",get_devname(iq_fh->devtype),iq_fh->bw);
}


/*! \brief Called to start the iqplayer device. Return 0 if OK, < 0 if error
    @param device pointer to the device structure specific to the RF hardware target
*/
static int iqplayer_loadfile(openair0_device *device, openair0_config_t *openair0_cfg) {
  recplay_state_t *s = device->recplay_state;
  recplay_conf_t  *c = openair0_cfg->recplay_conf;

  struct stat sb;
  s->fd = open(c->u_sf_filename, O_RDONLY );
  if (s->fd >= 0 ) {
    fstat(s->fd, &sb);
  } else {
    LOG_E( HW,"Cannot open %s exiting.\n", c->u_sf_filename );
    exit(-1);
  }
  
  if (c->use_mmap) {
    // use mmap
      s->mapsize=sb.st_size;
      LOG_I(HW,"Loading subframes using mmap() from %s size=%lu bytes ...\n",c->u_sf_filename, (uint64_t)sb.st_size );
      void *mptr = mmap(NULL, sb.st_size, PROT_WRITE, MAP_PRIVATE, s->fd, 0) ;
      if (mptr != MAP_FAILED) {
        s->ms_sample = (iqrec_t *) ( mptr + sizeof(iqfile_header_t));      
        parse_iqfile_header(device, (iqfile_header_t *)mptr);
        s->ms_sample = (iqrec_t *)((char *)mptr + sizeof(iqfile_header_t));
        LOG_I(HW,"Loaded %u subframes.\n",s->nbSamplesBlocks );
      } else {
        LOG_E(HW,"Cannot mmap file, exiting, errnor %s\n",strerror(errno));
        close(s->fd);
        exit(-1);
      }
  } else {
    iqfile_header_t fh;
    size_t hs = read(s->fd,&fh,sizeof(fh));

    if (hs == sizeof(fh)) {
        parse_iqfile_header(device, &fh);
        fstat(s->fd, &sb);
        s->mapsize=sb.st_size;
        LOG_I(HW, "Loading %u subframes from %s,size=%lu bytes ...\n",s->nbSamplesBlocks, c->u_sf_filename,(uint64_t)sb.st_size);
        // allocate buffer for 1 subframe at a time
        s->ms_sample = (iqrec_t *) malloc(sizeof(iqrec_t)+BELL_LABS_IQ_BYTES_PER_SF);

        if (s->ms_sample == NULL) {
          LOG_E(HW,"Memory allocation failed for individual subframe replay mode.\n" );
          close(s->fd);
          exit(-1);
        }

        memset(s->ms_sample, 0, sizeof(iqrec_t));

        // point at beginning of iqs in file
        if (lseek(s->fd,sizeof(iqfile_header_t), SEEK_SET) == 0) {
          LOG_I(HW,"Initial seek at beginning of the file\n" );
        } else {
          LOG_I(HW,"Problem initial seek at beginning of the file, %s\n",strerror(errno));
        }
      } else {
        LOG_E(HW,"Cannot read header in %s exiting.\n",c->u_sf_filename );
        close(s->fd);
        exit(-1);
      }
  }

  s->currentPtr=(uint8_t *)s->ms_sample;
  return 0;
}

/*! \brief print the device statistics
 * \param device the hardware to use
 * \returns  0 on success
 */
static int trx_iqplayer_get_stats(openair0_device *device)
{
  LOG_I(HW, "trx_iqplayer_get_stats() called, not implemented\n");
  return 0;
}

/*! \brief Reset device statistics
 * \param device the hardware to use
 * \returns 0 in success
 */
static int trx_iqplayer_reset_stats(openair0_device *device)
{
  LOG_I(HW, "trx_iqplayer_reset_stats() called, not implemented\n");
  return 0;
}

/*! \brief Stop operation of the transceiver
 */
static int trx_iqplayer_stop(openair0_device *device)
{
  LOG_I(HW, "trx_iqplayer_stop() called, not implemented\n");
  return 0;
}

/*! \brief Set RX frequencies
 * \param device the hardware to use
 * \param openair0_cfg RF frontend parameters set by application
 * \returns 0 in success
 */
static int trx_iqplayer_set_freq(openair0_device *device, openair0_config_t *openair0_cfg)
{
  LOG_I(HW, "trx_iqplayer_set_freq() called, not implemented\n");
  return 0;
}

/*! \brief Set gains
 * \param device the hardware to use
 * \param openair0_cfg RF frontend parameters set by application
 * \returns 0 in success
 */
static int trx_iqplayer_set_gains(openair0_device *device, openair0_config_t *openair0_cfg)
{
  LOG_I(HW, "trx_iqplayer_set_gains() called, not implemented\n");
  return 0;
}

/*! \brief start the oai iq player
 * \param device, the hardware used
 */
static int trx_iqplayer_start(openair0_device *device) {
  return 0;
}

/*! \brief Terminate operation of the oai iq player
 * \param device, the hardware used
 */
static void trx_iqplayer_end(openair0_device *device) {
  if (device == NULL)
    return;
  if (device->recplay_state == NULL)
    return;
  
  if (device->openair0_cfg->recplay_conf->use_mmap) {
    if (device->recplay_state->ms_sample != MAP_FAILED) {
      munmap(device->recplay_state->ms_sample, device->recplay_state->mapsize);
      }
  } else {
    if (device->recplay_state->ms_sample != NULL) {
      free(device->recplay_state->ms_sample);
    } 
  }
  device->recplay_state->ms_sample = NULL;
  if (device->recplay_state->fd >= 0) {
    close(device->recplay_state->fd);
    device->recplay_state->fd = -1;
  }

}
/*! \brief Write iqs function when in replay mode, just introduce a delay, as configured at init time,
      @param device pointer to the device structure specific to the RF hardware target
      @param timestamp The timestamp at which the first sample MUST be sent
      @param buff Buffer which holds the samples
      @param nsamps number of samples to be sent
      @param antenna_id index of the antenna if the device has multiple antennas
      @param flags flags must be set to true if timestamp parameter needs to be applied
*/
static int trx_iqplayer_write(openair0_device *device, openair0_timestamp timestamp, void **buff, int nsamps, int cc, int flags) {
  struct timespec req;
  req.tv_sec = 0;
  req.tv_nsec = device->openair0_cfg->recplay_conf->u_sf_write_delay * 1000;
  nanosleep(&req, NULL);
  return nsamps;
}

/*! \brief Receive samples from iq file.
 * Read \ref nsamps samples from each channel to buffers. buff[0] is the array for
 * the first channel. *ptimestamp is the time at which the first sample
 * was received.
 * \param device the hardware to use
 * \param[out] ptimestamp the time at which the first sample was received.
 * \param[out] buff An array of pointers to buffers for received samples. The buffers must be large enough to hold the number of samples \ref nsamps.
 * \param nsamps Number of samples. One sample is 2 byte I + 2 byte Q => 4 byte.
 * \param antenna_id Index of antenna for which to receive samples
 * \returns the number of sample read
*/
static int trx_iqplayer_read(openair0_device *device, openair0_timestamp *ptimestamp, void **buff, int nsamps, int cc) {
  recplay_state_t *s = device->recplay_state;
  static struct timeval tprev;
  static int read_count = 0;
  
  if (s->curSamplesBlock==0 && s->wrap_count==0) { 
    s->currentTs=s->ms_sample->ts;
    tprev.tv_sec = s->ms_sample->tv_sec;
    tprev.tv_usec = s->ms_sample->tv_usec;
    LOG_I(HW, "First timestamp=%lu s->nbSamplesBlocks=%u\n", s->currentTs, s->nbSamplesBlocks);
  }

  if (s->curSamplesBlock == s->nbSamplesBlocks) {
    if (s->wrap_count == device->openair0_cfg->recplay_conf->u_sf_loops) {
      LOG_W(HW, "iqplayer device terminating subframes replay  after %u iteration\n",
            device->openair0_cfg->recplay_conf->u_sf_loops);
      exit_function(__FILE__, __FUNCTION__, __LINE__,"replay ended, triggering process termination\n", OAI_EXIT_NORMAL);
    }

    if (s->wrap_count > 0) {
        LOG_I(HW, "wrapping on iq file (%ld)\n", s->wrap_count);
    }
    s->curSamplesBlock = 0;
    s->wrap_count++;
    device->recplay_state->currentPtr=(uint8_t *)device->recplay_state->ms_sample;

    if (!(device->openair0_cfg->recplay_conf->use_mmap) ) {
      close(device->recplay_state->fd);
      iqplayer_loadfile(device, device->openair0_cfg);
    }
  }

  if (!(device->openair0_cfg->recplay_conf->use_mmap)) {
    // read sample from file
    if (read(s->fd, s->ms_sample, sizeof(iqrec_t)) != sizeof(iqrec_t)) {
      LOG_E(HW,"pb reading iq record header at index %lu %s\n",sizeof(iqrec_t)*s->curSamplesBlock, strerror(errno) );
      exit(-1);
    } else {
      if (read(s->fd, s->ms_sample+1, BELL_LABS_IQ_BYTES_PER_SF) !=  BELL_LABS_IQ_BYTES_PER_SF) {
        LOG_E(HW,"pb reading iq record data at index %lu %s\n",sizeof(iqrec_t)*s->curSamplesBlock, strerror(errno) );
        exit(-1);
      }
    }
  }

  iqrec_t *curHeader=(iqrec_t *)s->currentPtr;
  if (curHeader->header != BELL_LABS_IQ_HEADER) {
    LOG_I(HW,"Problem iq header nsamps %d\n", nsamps);
    AssertFatal(curHeader->header==BELL_LABS_IQ_HEADER,"" );
  }
  // the current timestamp is the stored timestamp until we wrap on input
  AssertFatal(nsamps*4==curHeader->nbBytes,"nsamps=%d curHeader->nbBytes=%d", (int)nsamps, (int)curHeader->nbBytes);
  *ptimestamp = s->currentTs;
  memcpy(buff[0], curHeader+1, nsamps*4);
  s->curSamplesBlock++;
  // Prepare for next read
  s->currentTs+=nsamps;

  if (device->openair0_cfg->recplay_conf->use_mmap)
    s->currentPtr+=sizeof(iqrec_t)+BELL_LABS_IQ_BYTES_PER_SF;

  // subframe read delay is not taken into account
  // Real time interval is handle to mimic RF board latcency
  struct timeval tcur, tdiff;
  tcur.tv_sec = curHeader->tv_sec;
  tcur.tv_usec = curHeader->tv_usec;
  timersub(&tcur, &tprev, &tdiff);
  if (tdiff.tv_usec > 0) {
    busy_wait(tdiff.tv_usec);
  }
  tprev = tcur;

  read_count++;
  LOG_D(HW, "returning %d samples at ts %lu read_count %d\n", nsamps, *ptimestamp, read_count);
  return nsamps;
}

int device_init(openair0_device *device, openair0_config_t *openair0_cfg) {
  device->openair0_cfg = openair0_cfg;
  device->trx_start_func = trx_iqplayer_start;
  device->trx_get_stats_func = trx_iqplayer_get_stats;
  device->trx_reset_stats_func = trx_iqplayer_reset_stats;
  device->trx_end_func = trx_iqplayer_end;
  device->trx_stop_func = trx_iqplayer_stop;
  device->trx_set_freq_func = trx_iqplayer_set_freq;
  device->trx_set_gains_func = trx_iqplayer_set_gains;
  // Replay subframes from from file
  //  openair0_cfg[0].rx_gain_calib_table = calib_table_b210_38;
  //  bw_gain_adjust=1;
  device->trx_write_func = trx_iqplayer_write;
  device->trx_read_func  = trx_iqplayer_read;
  iqplayer_loadfile(device, openair0_cfg);
  LOG_UI(HW,"iqplayer device initialized, replay %s for %i iteration(s)\n",openair0_cfg->recplay_conf->u_sf_filename,openair0_cfg->recplay_conf->u_sf_loops);
  return 0;
}
/*@}*/
