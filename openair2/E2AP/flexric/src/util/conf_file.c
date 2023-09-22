#include "conf_file.h"

#include <assert.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>

#include "alg_ds/alg/defer.h"

static
const char* default_conf_file = "/usr/local/etc/flexric/flexric.conf";

static
const char* default_libs_dir = "/usr/local/lib/flexric/";

/*
#include <assert.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "alg_ds/alg/defer.h"

const char* conf_file = "/usr/local/etc/flexric/flexric.conf";
const char* libs_dir = "/usr/local/lib/flexric/";

static
bool valid_ip(const char* ip)
{
  assert(ip != NULL);
  if (strlen(ip) < 7) // 8.8.8.8, at least we need 7 characters  
    return false;

  struct sockaddr_in sa;
  int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
  return result != 0;
}


static
char *ltrim(char *s)
{
    while(isspace(*s)) s++;
    return s;
}

static
char *rtrim(char *s)
{
    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back+1) = '\0';
    return s;
}

//static
//char *trim(char *s)
//{
//    return rtrim(ltrim(s)); 
//}

char* get_near_ric_ip(args_t args)
{
  char* line = NULL;
  defer({free(line);});
  size_t len = 0;
  ssize_t read;

  FILE * fp = fopen(args.conf_file, "r");
  
  if (fp == NULL){
    printf("%s not found. Did you forget to sudo make install?\n", args.conf_file);
    exit(EXIT_FAILURE);
  }
  
  defer({fclose(fp); } );
  
  char ip_addr[24] = {0};
  while ((read = getline(&line, &len, fp)) != -1) {
    const char* needle = "NEAR_RIC_IP =";
    char* ans = strstr(line, needle);
    if(ans != NULL){
      ans += strlen(needle); 
      ans = ltrim(ans);
      ans = rtrim(ans);
      memcpy(ip_addr, ans , strlen(ans)); // \n character
      break;
    }    
  }
    
  if(valid_ip(ip_addr) == false){
    printf("IP address string invalid = %s Check the config file\n",ip_addr);
    exit(EXIT_FAILURE);
  }
  
  return strdup(ip_addr);
}

int is_regular_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

void init_args(args_t* args)
{
  strcpy(args->conf_file,conf_file);
  strcpy(args->libs_dir, libs_dir);
   
}
void print_usage(const char* prog)
{
  printf("Usage: %s [options]\n", prog);
  printf("\n");
  printf("    General options:\n");
  printf("  -h         : print usage\n");
  printf("  -c         : path to the config file\n");
  printf("  -p         : path to the shared libs \n");
}  

int parse_args(int argc, char** argv, args_t* args)
{
  int opt;
  init_args(args);
  
  while((opt = getopt(argc, argv, "hc:p:")) != -1) {
    switch(opt) {
    case 'h':
      return 1;
      
    case 'c':
      strcpy(args->conf_file,optarg);
      break;

    case 'p':
      strcpy(args->libs_dir,optarg);
      break;
    default:
      return 1;
    }
  }
  printf("setting the config file to %s\n",args->conf_file);
  printf("setting path for the shared libraries to %s\n",args->libs_dir);
 return 0;
}

*/
































































static
bool valid_ip(const char* ip)
{
  assert(ip != NULL);
  if (strlen(ip) < 7) // 8.8.8.8, at least we need 7 characters  
    return false;

  struct sockaddr_in sa;
  int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
  return result != 0;
}

static
char *ltrim(char *s)
{
    while(isspace(*s)) s++;
    return s;
}

static
char *rtrim(char *s)
{
    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back+1) = '\0';
    return s;
}

static
bool is_directory(const char* path)
{
  assert(path != NULL);

  struct stat sb = {0};
  int const rc = stat(path, &sb); 

  if(rc != 0){
    printf("Error finding path %s \n%s \n ", path, strerror(errno) ); 
    exit(EXIT_FAILURE);
  }

  if(S_ISDIR(sb.st_mode) == true)
    return true;

  return false;
}

static
bool is_regular_file(const char* path)
{
  assert(path != NULL);

  struct stat sb = {0};
  int const rc = stat(path, &sb); 

  if(rc != 0){
    printf("Error finding path %s \n%s \n ", path, strerror(errno) ); 
    exit(EXIT_FAILURE);
  }

  if(S_ISREG(sb.st_mode) == true)
    return true;

  return false;
}

static
void print_usage(void)
{     
  printf("Usage: [options]\n"); 
  printf("\n");
  printf("    General options:\n");
  printf("  -h         : print usage\n");
  printf("  -c         : path to the config file\n");
  printf("  -p         : path to the shared libs \n");
  printf(
      "\n");
  printf("Ex. -p /usr/local/lib/flexric/ -c /usr/local/etc/flexric/flexric.conf \n");
} 

static
void parse_args(int argc, char* const* argv, fr_args_t* args)
{
  assert(argc > 1 && "No argument passed");
  assert(argv != NULL);
  assert(args != NULL);

  int opt = '?';
  const char *optstring = "hc:p:";
  while((opt = getopt(argc, argv, optstring)) != -1) {
    switch(opt) {
      case 'h':{
                 print_usage();
                 // printf("Usage  to be or not to be... \n" );
                 exit(EXIT_SUCCESS);
               }
      case 'c':  {
                   int const len = strlen(optarg);
                   assert(len < FR_CONF_FILE_LEN - 1);

                   if(!is_regular_file(optarg)){
                     printf("Error: %s is not a regular file \n", optarg);  
                     exit(EXIT_FAILURE);
                   }

                   memset(args->conf_file, '\0', FR_CONF_FILE_LEN);
                   strncpy(args->conf_file, optarg, len );

                   break;
                 }
      case 'p':{
                 int const len = strlen(optarg);
                 assert(len < FR_CONF_FILE_LEN - 1 );

                 if(!is_directory(optarg)){
                   printf("Error: %s is not a directory \n", optarg);  
                   exit(EXIT_FAILURE);
                 }
                 if(optarg[len -1] != '/'){
                   printf("Error: %s directory should finish with a / , e.g. /usr/local/flexric/  please add it\n", optarg);  
                   exit(EXIT_FAILURE);
                 }

                 memset(args->libs_dir, '\0', FR_CONF_FILE_LEN);
                 strncpy(args->libs_dir, optarg, len);

                 break;
               }
      case '?':{
                 printf("Error: unknown flag %c ??\n ",optopt);
                 print_usage();
                 exit(EXIT_FAILURE);
               }
      default:{
                assert(0!=0 && "Unforeseen code path");
                exit(EXIT_FAILURE);
              }

    }   
  }
}

static
void load_default_val(fr_args_t* args)
{
  assert(args != NULL);
  assert(strlen(default_conf_file) < FR_CONF_FILE_LEN && "Path too long");
  assert(strlen(default_libs_dir)  < FR_CONF_FILE_LEN && "Path too long");

  memset(args, '\0', sizeof(*args));
  strncpy(args->conf_file, default_conf_file, FR_CONF_FILE_LEN);
  strncpy(args->libs_dir, default_libs_dir, FR_CONF_FILE_LEN);
}

fr_args_t init_fr_args(int argc, char* argv[])
{
  assert(argc > -1);

  fr_args_t args = {0};
  load_default_val(&args);
  
  if(argc > 1){
    assert(argc < 6 && "Only -h -c -p flags supported");
    assert(argv != NULL);
    parse_args(argc, argv, &args);
  }
  assert(is_directory(args.libs_dir) == true && "Did you forget to: sudo make install ?");
  assert(is_regular_file(args.conf_file) == true && "Did you forget to: sudo make install ?");

  printf("Setting the config -c file to %s\n",args.conf_file);
  printf("Setting path -p for the shared libraries to %s\n",args.libs_dir);

  return args;
}

char* get_near_ric_ip(fr_args_t const* args)
{
  // fast path
  if(args->ip != NULL)
    return strdup(args->ip);

  char* line = NULL;
  defer({free(line);});
  size_t len = 0;
  ssize_t read;

  FILE * fp = fopen(args->conf_file, "r");
  
  if (fp == NULL){
    printf("%s not found. Did you forget to sudo make install?\n", args->conf_file);
    exit(EXIT_FAILURE);
  }
  
  defer({fclose(fp); } );
  
  char ip_addr[24] = {0};
  while ((read = getline(&line, &len, fp)) != -1) {
    const char* needle = "NEAR_RIC_IP =";
    char* ans = strstr(line, needle);
    if(ans != NULL){
      ans += strlen(needle); 
      ans = ltrim(ans);
      ans = rtrim(ans);
      assert(strlen(ans) <= sizeof(ip_addr));
      memcpy(ip_addr, ans , strlen(ans)); // \n character
      break;
    }    
  }
    
  if(valid_ip(ip_addr) == false){
    printf("IP address string invalid = %s Check the config file\n",ip_addr);
    exit(EXIT_FAILURE);
  }
  
  return strdup(ip_addr);
}

char* get_conf_db_dir(fr_args_t const* args)
{
  char* line = NULL;
  defer({free(line);});
  size_t len = 0;
  ssize_t read;

  FILE * fp = fopen(args->conf_file, "r");

  if (fp == NULL){
    printf("%s not found. Did you forget to sudo make install?\n", args->conf_file);
    exit(EXIT_FAILURE);
  }

  defer({fclose(fp); } );

  char db_dir[PATH_MAX] = {0};
  while ((read = getline(&line, &len, fp)) != -1) {
    const char* needle = "DB_DIR =";
    char* ans = strstr(line, needle);
    if(ans != NULL){
      ans += strlen(needle);
      ans = ltrim(ans);
      ans = rtrim(ans);
      assert(strlen(ans) <= sizeof(db_dir));
      memcpy(db_dir, ans , strlen(ans)); // \n character
      break;
    }
  }

  // TODO: valid_path()

  return strdup(db_dir);
}

char* get_conf_db_name(fr_args_t const* args)
{
  char* line = NULL;
  defer({free(line);});
  size_t len = 0;
  ssize_t read;

  FILE * fp = fopen(args->conf_file, "r");

  if (fp == NULL){
    printf("%s not found. Did you forget to sudo make install?\n", args->conf_file);
    exit(EXIT_FAILURE);
  }

  defer({fclose(fp); } );

  char db_name[24] = {0};
  while ((read = getline(&line, &len, fp)) != -1) {
    const char* needle = "DB_NAME =";
    char* ans = strstr(line, needle);
    if(ans != NULL){
      ans += strlen(needle);
      ans = ltrim(ans);
      ans = rtrim(ans);
      assert(strlen(ans) <= sizeof(db_name));
      memcpy(db_name, ans , strlen(ans)); // \n character
      break;
    }
  }

  // TODO: valid_db_name()

  return strdup(db_name);
}
