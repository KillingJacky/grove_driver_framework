

#ifndef __RPC_SERVER_H__
#define __RPC_SERVER_H__

#include "suli2.h"

typedef enum
{
    TYPE_NONE, TYPE_BOOL, TYPE_UINT8, TYPE_INT8, TYPE_UINT16, TYPE_INT16, TYPE_INT, TYPE_UINT32, TYPE_INT32, TYPE_FLOAT, TYPE_STRING
}type_t;

typedef enum
{
    METHOD_READ, METHOD_WRITE
}method_dir_t;

enum
{
    PARSE_GET_POST, PARSE_GROVE_NAME, PARSE_METHOD, CHECK_POST_ARGS, PRE_PARSE_ARGS, PARSE_ARGS, PARSE_CALL
};

enum
{
    REQ_GET, REQ_POST
};

typedef void (*method_ptr_t)(uint8_t *input);

struct resource_s;

#define MAX_INPUT_ARG_LEN               4
typedef struct resource_s
{
    char                   *grove_name;
    char                   *method_name;
    method_dir_t           rw;
    method_ptr_t           method_ptr;
    uint8_t                arg_types[MAX_INPUT_ARG_LEN];
    struct resource_s      *next;
}resource_t;

void rpc_server_init();

void rpc_server_register_method(char *grove_name, char *method_name, method_dir_t rw, method_ptr_t ptr, uint8_t *arg_types);

void rpc_server_register_resources();

void rpc_server_loop();

#endif
