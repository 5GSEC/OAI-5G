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



#include "plugin_agent.h"

#include "util/alg_ds/alg/alg.h"
#include "util/alg_ds/ds/lock_guard/lock_guard.h"
#include "util/compare.h"
#include "util/conf_file.h"
#include <assert.h>

#include <arpa/inet.h>
#include <dlfcn.h>
#include <dirent.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

/*
static
void* rx_plugin_agent(void* p_v)
{
  plugin_ag_t* p = (plugin_ag_t*)p_v;
  const int port = 8080;
  char buf[128] = {0};

  p->sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  assert(p->sockfd != -1 && "Error creating socket");

  struct sockaddr_in serv_addr = {.sin_family = AF_INET,
    .sin_port = htons(port),
    .sin_addr.s_addr = INADDR_ANY};

  int rc = bind(p->sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
  assert(rc != -1 && "Error while binding. Address already in use?");



  while(true){
    struct sockaddr_in cli_addr;
    socklen_t len = sizeof(cli_addr); 

    // Receive file name
    rc = recvfrom(p->sockfd, buf, 128, 0, (struct sockaddr *)&cli_addr,&len);
    if(p->flag_shutdown) 
      break;
    assert(rc > -1 && rc < 128 && "Buffer overflow");

    printf("Name of the file = %s\n",buf);

    // Receive file size
    int size = 0;
    rc = recvfrom(p->sockfd, &size, sizeof(int), 0, (struct sockaddr *)&cli_addr, &len);
    if(p->flag_shutdown) 
      break;
    assert(rc == sizeof(int));

    printf("Size of the file = %d\n",size);

    char* data = calloc(1, size);
    assert(data != NULL && "Memory exhausted!");
    // Receive file itself 
    rc = recvfrom(p->sockfd,data,size,0,(struct sockaddr *)&cli_addr, &len);
    if(p->flag_shutdown) 
      break;
    assert(rc == size);

    // Save file
    FILE* fptr = fopen(buf, "wb");
    rc = fwrite(data,size,1,fptr);
    assert(rc == 1);
    free(data);
    fclose(fptr);

    // Change the file permissions as it is a shared object
    int const mode = strtol("0755", 0, 8);
    rc = chmod(buf, mode);
    assert(rc > -1);
    if(p->flag_shutdown) 
      break;

    char full_path[PATH_MAX] = {0};
    char* ptr = getcwd(full_path, PATH_MAX);
    ptr[strlen(ptr)] = '/';
    memcpy(full_path + strlen(full_path), buf, strlen(buf));

     

    // Load the plugin in the agent
    load_plugin_ag(p, full_path);
    printf("File received and loaded\n");

  }
  printf("Closing the socket\n");
  close(p->sockfd);
  return NULL;
}
*/

static inline
void free_sm_agent(void* key, void* value)
{
  assert(key != NULL);
  assert(value != NULL);

  sm_agent_t* sm = (sm_agent_t*)value;

  void* handle = sm->handle;
  sm->free_sm(sm);

  if(handle != NULL)
      dlclose(handle);
}

static inline
void check_dl_error(void)
{
  const char* error = dlerror();
  if (error != NULL) {
    printf("Error from dlerror = %s \n", error);
    fflush(stdout);
    assert(0 != 0 && "error loading the init of the shared object");
  }
}

static
int is_regular_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

static
void load_all_pugin_ag(plugin_ag_t* p, const char* dir_path)
{
  /* Scanning the in directory */
  DIR* fd = opendir(dir_path);
  assert(fd != NULL && "Error opening the input directory");

  struct dirent* in_file = readdir(fd);
  while (in_file != NULL) {
    // We don't want current and parent directories
    if (!strcmp (in_file->d_name, ".")){
      in_file = readdir(fd);
      continue;
    }
    if (!strcmp (in_file->d_name, "..")){
      in_file = readdir(fd);
      continue;
    }

    char file_path[1024] = {0};
    size_t const sz_dir_path = strlen(dir_path);
    assert(sz_dir_path < 1024);
    strncat(file_path, dir_path, sz_dir_path < 1024 ? sz_dir_path : 1024 ); 

    size_t const rem = 1024 - sz_dir_path; 
    size_t const sz_in = strlen(in_file->d_name);
    assert(sz_in  < rem && "Lacking space...");
    strncat(file_path + sz_dir_path, in_file->d_name, sz_in < rem ? sz_in : rem );

    const char* needle = ".conf";
    const char* ans = strstr(file_path, needle);
    if(ans == NULL && is_regular_file(file_path)) // Not a Configuration file
      load_plugin_ag(p, file_path);

    in_file = readdir(fd);
  }

  closedir(fd);
}

void unload_all_pugin_ag(plugin_ag_t* p, const char* path)
{
  assert(p != NULL);
  assert(path != NULL);
  assert(0!=0 && "Not implemented!");
}

static
sm_io_ag_ran_t cp_io_ran(sm_io_ag_ran_t const* src)
{
  assert(src != NULL);
  sm_io_ag_ran_t dst = {0}; 

  // Read
  memcpy(dst.read_ind_tbl, src->read_ind_tbl, sizeof(read_ind_fp)*SM_AGENT_IF_READ_V0_END);
  memcpy(dst.read_setup_tbl, src->read_setup_tbl, sizeof(read_e2_setup_fp)*SM_AGENT_IF_E2_SETUP_ANS_V0_END);
  //read_rsu_fp read_rsu_tbl[0];
  //read_rsu_fp read_rsu_tbl[0];

  // Write
  memcpy(dst.write_ctrl_tbl, src->write_ctrl_tbl, sizeof(write_ctrl_fp)*SM_AGENT_IF_WRITE_CTRL_V0_END);
  memcpy(dst.write_subs_tbl, src->write_subs_tbl, sizeof(write_subs_fp)*SM_AGENT_IF_WRITE_SUBS_V0_END);

  return dst;
}

void init_plugin_ag(plugin_ag_t* p, const char* dir_path, sm_io_ag_ran_t io)
{
  assert(p != NULL);
  assert(dir_path != NULL);

  p->dir_path = (char*)dir_path;
 
  p->io = cp_io_ran(&io);

  p->flag_shutdown = false;

  pthread_mutexattr_t attr= {0};
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);

  int rc = pthread_mutex_init(&p->sm_ds_mtx, &attr);
  assert(rc == 0);

  const size_t ran_func_size = sizeof(uint16_t);
  assoc_init(&p->sm_ds, ran_func_size, cmp_ran_func_id, free_sm_agent );

  load_all_pugin_ag(p, dir_path);

//  pthread_create(&p->thread_rx, NULL, rx_plugin_agent, p);
}

void free_plugin_ag(plugin_ag_t* p)
{
  assert(p != NULL);

  assoc_free(&p->sm_ds); 

  pthread_mutex_destroy(&p->sm_ds_mtx);

  // Thread management
  p->flag_shutdown = true;

  int rc = shutdown(p->sockfd, SHUT_RDWR);
  if(rc != 0){
    printf("Closing the agent socket: %s \n", strerror(errno));
  }
  //assert(rc == 0);
//  rc = pthread_join(p->thread_rx, NULL);
//  assert(rc == 0);
}

static inline
bool absolute_path(const char* path)
{
  return path != NULL ? true : false;
}

void load_plugin_ag(plugin_ag_t* p, const char* path)
{
  //ToDo: Looks code from a sophomore. DO IT PROPERLY
  assert(p != NULL);
  assert(path != NULL);

  printf("[E2 AGENT]: Opening plugin from path = %s \n", path );

  void* handle = dlopen(path, RTLD_NOW);
  if(handle == NULL){
    printf("Error while opening the shared object = %s \n", dlerror());
  }
  assert(handle != NULL && "Could not open the file path");
  dlerror();    

  char* so_name = strrchr(path, '/');
  char* ptr_so_name = NULL; 
  if(absolute_path(so_name) == true){
    ptr_so_name = so_name + strlen("/lib");
    assert(so_name != NULL);
  } else {
     ptr_so_name = (char*)path + strlen("lib");
  }

  char symbol_so[256] = {'m','a','k','e','_'};
  char* needle = "_sm.so";

  char* match = strstr( ptr_so_name, needle); // search_naive(strlen(needle), needle, strlen(ptr_so_name ), ptr_so_name);
  assert(match != NULL && "Could not find the string _sm.so in the so\n");

  char* ptr = &symbol_so[5];
  assert(match > ptr_so_name);
  strncat(ptr, ptr_so_name , match - ptr_so_name);
  ptr += match - ptr_so_name;
  const char* suffix = "_sm_agent";
  strncat(ptr,suffix, strlen(suffix));

  sm_agent_t* (*fp)(sm_io_ag_ran_t);
  fp = dlsym(handle, symbol_so);
  check_dl_error();
  sm_agent_t* sm = fp(p->io);

  sm->handle = handle; 
  assert(sm != NULL);
  const uint16_t ran_func_id = sm->ran_func_id;

  {
    lock_guard(&p->sm_ds_mtx);
    assoc_insert(&p->sm_ds, &ran_func_id, sizeof(ran_func_id), sm);
  }

//  printf("AGENT: Accepting SM ID = %d with def = %s \n", sm->ran_func_id, sm->ran_func_name);
}

void unload_plugin_ag(plugin_ag_t* p, uint16_t key)
{
  assert(p != NULL);
  assert(key != 0);
  assert(0!=0 && "Not implemented!");
}

sm_agent_t* sm_plugin_ag(plugin_ag_t* p, uint16_t key)
{
  assert(p != NULL);
  assert(key > 0 && "Reserved value");

  lock_guard(&p->sm_ds_mtx);

  void* start_it = assoc_front(&p->sm_ds);
  void* end_it = assoc_end(&p->sm_ds);
  void* it = find_if(&p->sm_ds, start_it, end_it, &key, eq_ran_func_id); 
  assert(it != end_it && "RAN function ID not found in the RAN"); 

  sm_agent_t* sm = assoc_value(&p->sm_ds, it);

  assert(sm->ran_func_id == key);
  return sm;
}

size_t size_plugin_ag(plugin_ag_t* p)
{
  assert(p != NULL);

  lock_guard(&p->sm_ds_mtx);

  size_t s = assoc_size(&p->sm_ds); 

  return s;
}

