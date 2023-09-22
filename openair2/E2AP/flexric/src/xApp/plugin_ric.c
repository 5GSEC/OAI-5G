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


#include "plugin_ric.h"

#include "util/alg_ds/alg/alg.h"
#include "util/compare.h"
#include "util/conf_file.h"
#include <assert.h>

#include <arpa/inet.h>
#include <dlfcn.h>
#include <dirent.h>

#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>

static inline
void free_sm_ric(void* key, void* value)
{
  assert(key != NULL);
  assert(value != NULL);

  sm_ric_t* sm = (sm_ric_t*)value;

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
void load_all_pugin_ric(plugin_ric_t* p, const char* dir_path)
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
    strncat(file_path, dir_path, 1024 - 1 ); 

    size_t const sz_file_name = strlen(in_file->d_name);
    assert( sz_dir_path + sz_file_name < 1024 - 1 );
    strncat(file_path + sz_dir_path , in_file->d_name, 1024 - 1 - sz_dir_path); 

    const char* needle = ".conf"; 
    const char* ans = strstr(file_path, needle);
    if(ans == NULL && is_regular_file(file_path)) // Not a Configuration file
      load_plugin_ric(p, file_path);

    in_file = readdir(fd);
  }
  closedir(fd);
}

void unload_all_pugin_ric(plugin_ric_t* p, const char* path)
{
  assert(p != NULL);
  assert(path != NULL);
  assert(0!=0 && "Not implemented!");
}

void init_plugin_ric(plugin_ric_t* p, const char* dir_path) 
{
  assert(p != NULL);
  assert(dir_path != NULL);

  p->dir_path = (char*)dir_path;

  const size_t ran_func_size = sizeof(uint16_t);
  assoc_init(&p->sm_ds, ran_func_size, cmp_ran_func_id, free_sm_ric );

  load_all_pugin_ric(p, dir_path);
}

void free_plugin_ric(plugin_ric_t* p)
{
  assert(p != NULL);
  assoc_free(&p->sm_ds); 
}

void load_plugin_ric(plugin_ric_t* p, const char* path)
{
  //ToDo: Looks like code from a sophomore. DO IT PROPERLY
  assert(p != NULL);
  assert(path != NULL);
  void* handle = dlopen(path, RTLD_NOW);
  if(handle == NULL){
    printf("Not valid path = %s \n", path);
  }
  assert(handle != NULL && "Could not open the file path");
  dlerror();    

  char* so_name = strrchr(path, '/');
  char* ptr_so_name = so_name + strlen("/lib");
  assert(so_name != NULL);

  char symbol_so[256] = {'m','a','k','e','_'};
  char* needle = "_sm.so";


  char* match = strstr(so_name, needle); // search_naive(strlen(needle), needle, strlen(so_name ), so_name);
  assert(match != NULL && "Could not find the string _sm.so in the so\n");

  char* ptr = &symbol_so[5];
  assert(match > ptr_so_name);
  strncat(ptr, ptr_so_name , match - ptr_so_name);
  ptr += match - ptr_so_name;
  const char* suffix = "_sm_ric";
  strncat(ptr,suffix, strlen(suffix));

  sm_ric_t* (*fp)(void);
  fp = dlsym(handle, symbol_so);
  check_dl_error();
  sm_ric_t* sm = fp();

  sm->handle = handle; 
  assert(sm != NULL);
  const uint16_t ran_func_id = sm->ran_func_id;
  assert(ran_func_id > 0 && "Reserved RAN function ID");
  assoc_insert(&p->sm_ds, &ran_func_id, sizeof(ran_func_id), sm);
  
  printf("[NEAR-RIC]: Loading SM ID = %d with def = %s \n", sm->ran_func_id, sm->ran_func_name);
}

void unload_plugin_ric(plugin_ric_t* p, uint16_t key)
{
  assert(p != NULL);
  assert(key != 0);
  assert(0!=0 && "Not implemented!");
}

sm_ric_t* sm_plugin_ric(plugin_ric_t* p, uint16_t  ran_func_id)
{
  assert(p != NULL);
  assert(ran_func_id > 0 && "Reserved value");

  void* start_it = assoc_front(&p->sm_ds);
  void* end_it = assoc_end(&p->sm_ds);
  void* it = find_if(&p->sm_ds, start_it, end_it, &ran_func_id, eq_ran_func_id); 
  assert(it != end_it && "RAN function ID not found in the RAN"); 

  sm_ric_t* sm = assoc_value(&p->sm_ds, it);
  assert(sm->ran_func_id == ran_func_id);
  return sm;
}

size_t size_plugin_ric(plugin_ric_t* p)
{
  assert(p != NULL);
  return assoc_size(&p->sm_ds); 
}


typedef struct
{
  int sockfd;
  struct sockaddr_in cli_addr; 
} fd_addr_t;


static inline
fd_addr_t init_udp_socket(const char* addr, int port)
{
  assert(addr != NULL);
  assert(port > 0);

  fd_addr_t ret = {0};
  ret.sockfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
  assert(ret.sockfd > -1 && "Error creating the socket");

  ret.cli_addr.sin_family = AF_INET;
  ret.cli_addr.sin_port = htons(port);

  int rc = inet_aton(addr, &ret.cli_addr.sin_addr);
  assert(rc == 1 && "Invalid string for the address IPv4");

  return ret;
}

static inline
bool file_exists(char const *filename) 
{
  struct stat buffer;
  return (stat(filename, &buffer) == 0);
}

static inline
size_t file_size(char const* filename)
{
  struct stat st;
  const int rc = stat(filename, &st);
  assert(rc != -1);
  return st.st_size;
}


static
void send_udp_socket(fd_addr_t* fd, size_t len, char const data[len])
{
  assert(fd != NULL);
  assert(len > 0);
 
  int rc = sendto(fd->sockfd, data, len , 0, (struct sockaddr *)&fd->cli_addr, sizeof(fd->cli_addr));
  if(rc != (int)len ){
    printf("Error rc = %d and len = %lu \n", rc, len);
    printf("strerror = %s \n", strerror(errno));

  }
  assert(rc == (int)len);
}

void tx_plugin_ric(plugin_ric_t* p, size_t len, char const file_path[len])
{
  assert(p != NULL);
  assert(len > 0);
  assert(file_exists(file_path) == true);

  char const * server_addr = "127.0.0.1";
  int const port = 8080;

  fd_addr_t fd = init_udp_socket(server_addr, port);

  // Send file name
  
  char* ptr = strrchr(file_path, '/'); 
  assert(ptr != NULL && "No absolute path provided");
  send_udp_socket(&fd, strlen(ptr +1), ptr + 1);

  int const size = file_size(file_path);

  // Send file size
  send_udp_socket(&fd, sizeof(int), (const char*)&size);

  char* data = calloc(1, size+1);
  assert(data != NULL && "Memory exhausted");

  FILE* fptr=fopen(file_path,"r");
  assert(fptr != NULL && "Unable to open file");

  int rc = fread(data,size,1,fptr);
  assert(rc == 1);

  // Send the file itself 
  send_udp_socket(&fd, size, data);


  free(data);

  fclose(fptr);

  close(fd.sockfd);
}

