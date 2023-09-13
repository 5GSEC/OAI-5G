#include <math.h>               /* fmin, fmax */
#include <stdlib.h>             /* calloc */
#include <string.h>             /* strstr */
#include <time.h>               /* timespec */

#if 01
#include "attack_extern.h"      /* bts_attack */
#include "log.h"                /* LOG_T */
#else
#define LOG_T(c, x...)
static int bts_attack = 0;
enum {
  PHY,
  MAC,
  RRC,
  RLC,
  UTIL
};
#endif


/*
2023.07.05 15:12:58.395176 [PHY] I {lte-uesoftmodem} [UE  0][RAPROC] Frame 846, subframe 1: Generating PRACH (eNB 0) preamble index 15 for UL, TX power -52 dBm (PL 56 dB), l3msg 
2023.07.05 15:12:58.403380 [MAC] A {lte-uesoftmodem} [UE 0][RAPROC] Frame 846 Received RAR (4f|00.40.03.4c.7c.95) for preamble 15/15
2023.07.05 15:12:58.405555 [PHY] I {lte-uesoftmodem} [UE  0][RAPROC] Frame 847, Subframe 2 Generating (RRCConnectionRequest) Msg3 (nb_rb 1, first_rb 1, round 0, rvidx 0) Msg3: 20.6.1f|5a.50.45.38.23.76
2023.07.05 15:12:58.405613 [PHY] I {lte-uesoftmodem} [UE  0][RAPROC] frame 847, subframe 2: Msg3 Po_PUSCH -50 dBm (-10800,0,100*PL=5600,0,200)
2023.07.05 15:12:58.413538 [MAC] I {lte-uesoftmodem} [UE 0][RAPROC] Frame 847 : received contention resolution msg: 5a.50.45.38.23.76, Terminating RA procedure
2023.07.05 15:12:58.413751 [RRC] I {TASK_RRC_UE} [UE 0][RAPROC] Frame 847 : Logical Channel UL-DCCH (SRB1), Generating RRCConnectionSetupComplete (bytes53, eNB 0)
2023.07.05 15:17:21.147966 [RLC] W {lte-uesoftmodem} rlc_tick: discontinuity (expected 497.0, got 497.1)
2023.07.05 15:17:21.148422 [PHY] E {lte-uesoftmodem} Missed real time
2023.07.05 15:13:01.215522 [PHY] E {lte-uesoftmodem} [UE 0] Frame 1127, subframe 2 RRC Connection lost, returning to PRACH
2023.07.05 15:13:11.108715 [PHY] I {lte-uesoftmodem} generate_drs_pusch: index for Msc_RS=168 not found
*/

enum {
  _PRACH,
  _RAR,
  _CON_REQ,
  _PUSCH,
  _CON_SET,
  _CON_SETC,
  _DISCONT,
  _MISSED,
  _CON_LOST,
  _BAD_PUSCH
};

#define _PRACH_COMP     PHY
#define _RAR_COMP       MAC
#define _CON_REQ_COMP   PHY
#define _PUSCH_COMP     PHY
#define _CON_SET_COMP   MAC
#define _CON_SETC_COMP  RRC
#define _DISCONT_COMP   RLC
#define _MISSED_COMP    PHY
#define _CON_LOST_COMP  PHY
#define _BAD_PUSCH_COMP PHY

#define _PRACH_STR      "Generating PRACH ("
#define _RAR_STR        "Received RAR ("
#define _CON_REQ_STR    "Generating (RRCConnectionRequest) Msg3"
#define _PUSCH_STR      "Msg3 Po_PUSCH"
#define _CON_SET_STR    "received contention resolution msg:"
#define _CON_SETC_STR   "Generating RRCConnectionSetupComplete ("
#define _DISCONT_STR    "rlc_tick: discontinuity"
#define _MISSED_STR     "Missed real time"
#define _CON_LOST_STR   "RRC Connection lost, returning to PRACH"
#define _BAD_PUSCH_STR  "generate_drs_pusch: index for Msc_RS="

#define _PRACH_VAL      .10
#define _RAR_VAL        .15
#define _CON_REQ_VAL    .27
#define _PUSCH_VAL      .20
#define _CON_SET_VAL    .25
#define _CON_SETC_VAL   .30
#define _DISCONT_VAL    .35
#define _MISSED_VAL     .40
#define _CON_LOST_VAL   .45
#define _BAD_PUSCH_VAL  .50


#define IF_FIND_LINE(_sym)                                      \
  if (comp == _sym ## _COMP &&                                  \
      strstr (msg, _sym ## _STR) /* != (char *) NULL */) {      \
    _type = _sym;                                               \
    _val  = _sym ## _VAL;                                       \
  }

typedef int BOOL;
#define TRUE    1
#define FALSE   0


  static inline double
_nonZeroMin (double f1, double f2)
{
  if      (f1 == 0.0)
    return f2;
  else if (f2 == 0.0)
    return f1;
  else
    return fmin (f1, f2);
}


// BTS Resource Depletion

#define T_SIGMA_RANGE       2.0 /* 2 sigma around average */
#define T_DELTA_RANGE       0.1 /* 10 percent around single sample value */

typedef struct ConnectRequests {

  double average;
  double stdDev;
  double sum;
  double sumSq;
  double numRequests;

} ConnectRequests, *ConnectRequestsPtr;

#define MAX_NUM_CONN_REQUESTS   2

static ConnectRequestsPtr _ConnectRequests = (ConnectRequestsPtr) NULL;
static int                _numConnRequests = 0;
static BOOL               _doInitBTSAttack = TRUE;

static BOOL   _firstConnectReq = TRUE;
static double _lastConnectReq  = 0.0;
static double _missedStart     = 0.0;
static double _missedLast      = 0.0;
static double _discntStart     = 0.0;
static double _discntLast      = 0.0;

static int    _maxNumUEs       = 0;


  static inline void
_initBTSAttack ()
{
  if (_doInitBTSAttack) {
    _doInitBTSAttack = FALSE;
    _ConnectRequests = (ConnectRequestsPtr) calloc ((size_t) MAX_NUM_CONN_REQUESTS, (size_t) sizeof (ConnectRequests));
  }
}


  static int
_trackConnectionRequest (double connReq)
{
  ConnectRequestsPtr pConnReq = (ConnectRequestsPtr) NULL;
  int                idx;

  for (idx = 0; idx < _numConnRequests; ++idx) {
    ConnectRequestsPtr connReqPtr = &_ConnectRequests[idx];
    double             deltaRange = connReqPtr->numRequests > 1 ? connReqPtr->stdDev * T_SIGMA_RANGE : connReqPtr->sum * T_DELTA_RANGE;
    double             nominalVal = connReqPtr->average;

    if ((nominalVal - deltaRange) >= connReq && connReq <= (nominalVal + deltaRange)) {
      pConnReq = connReqPtr;
      break;
    }
  }

  // Note: UE "replacement" requests with arbitrary delays will be "rejected" due to
  // a limited number of _ConnectRequests (viz., 2).

  if (pConnReq == (ConnectRequestsPtr) NULL && _numConnRequests < MAX_NUM_CONN_REQUESTS)
    pConnReq = &_ConnectRequests[++_numConnRequests];

  // Update statistics

  if (pConnReq /* != (ConnectRequestsPtr) NULL */) {
    pConnReq->sum         += connReq;
    pConnReq->sumSq       += connReq * connReq;
    pConnReq->numRequests += 1;

    double fNumReqs = (double) pConnReq->numRequests;

    pConnReq->average = pConnReq->sum / fNumReqs;

    if (pConnReq->numRequests > 1)
      pConnReq->stdDev = sqrt ((pConnReq->sumSq - pConnReq->sum * pConnReq->sum / fNumReqs) / (fNumReqs - 1.0));
  }

  return pConnReq /* != (ConnectRequestsPtr) NULL */ ? idx : -1;
}


  static inline int
_trackMaxUEs ()
{
  if (_numConnRequests < 2) {
    ++_maxNumUEs;
    return 0;
  }
  else
    return _maxNumUEs;
}


  void
log_analyzer (const char *compName, const char *file, const char *func, int line, int comp, int level, const struct timespec t, const char *msg)
{
  if (bts_attack /* > 0 */) {
    _initBTSAttack ();

    int    _type;
    double _val;

         IF_FIND_LINE (_PRACH)
    else IF_FIND_LINE (_RAR)
    else IF_FIND_LINE (_CON_REQ)
    else IF_FIND_LINE (_PUSCH)
    else IF_FIND_LINE (_CON_SET)
    else IF_FIND_LINE (_CON_SETC)
    else IF_FIND_LINE (_DISCONT)
    else IF_FIND_LINE (_MISSED)
    else IF_FIND_LINE (_CON_LOST)
    else IF_FIND_LINE (_BAD_PUSCH)
    else return;

    double _time = (double) t.tv_sec + (double) t.tv_nsec / 1000000000.0;

    // Track connection requests

    if (_type == _CON_REQ) {
      if (_lastConnectReq /* != 0.0 */) {
        _val = _time - _lastConnectReq;

        if (!_firstConnectReq && (_discntStart /* != 0.0 */ || _missedStart /* != 0.0 */)) {
          double _tAdjust;
          
          _tAdjust      = fmax (_discntLast, _missedLast) - _nonZeroMin (_discntStart, _missedStart);
          _val         -= _tAdjust;
          _missedStart  = 0.0;
          _missedLast   = 0.0;
        }

        if (_firstConnectReq)
          _firstConnectReq = FALSE;
      }

      _discntStart    = 0.0;
      _discntLast     = 0.0;

      _lastConnectReq = _time;

      if (_firstConnectReq)
        return;

      (void) _trackConnectionRequest (_val);
    }

    // Track "consecutive" connection setup completions to determine maximum UE slots

    else if (_type == _CON_SETC) {
      int maxUEs = _trackMaxUEs ();

      if (maxUEs /* > 0 */)
        LOG_T (UTIL, "BTS log_analyzer: maximum number of UEs is %d", maxUEs);
    }

    else if (_type == _DISCONT) {
      if (_discntStart == 0.0)
        _discntStart = _time;
      _discntLast = _time;
    }

    else if (_type == _MISSED) {
      if (_missedStart == 0.0)
        _missedStart = _time;
      _missedLast = _time;
    }
  }
}
