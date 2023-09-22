#ifndef FLEXRIC_CONFIGURATION_FILE_H
#define FLEXRIC_CONFIGURATION_FILE_H 

#include <stdint.h>
#define FR_CONF_FILE_LEN 128

typedef struct {
  // Option 1: directly pass IP argument 
  const char *ip;

  // Option 2: read from file
  char conf_file[FR_CONF_FILE_LEN];
  char libs_dir[FR_CONF_FILE_LEN];
} fr_args_t;

fr_args_t init_fr_args(int argc, char* argv[]);

char* get_near_ric_ip(fr_args_t const*);

char* get_conf_db_dir(fr_args_t const*);

char* get_conf_db_name(fr_args_t const*);

#endif

