

#include "stdlib.h"
#include "rpc_stream.h"
#include "rpc_server.h"


resource_t *p_first_resource;
resource_t *p_cur_resource;

int parse_stage;

void rpc_server_init()
{
    //init rpc stream
    stream_init();

    //init rpc server
    p_first_resource = NULL;
    p_cur_resource = NULL;
    parse_stage = PARSE_GET_POST;

    rpc_server_register_resources();
}

void rpc_server_register_method(char *grove_name, char *method_name, method_dir_t rw, method_ptr_t ptr, uint8_t *arg_types)
{
    resource_t *p_res = (resource_t*)(sizeof(resource_t));
    if (!p_res) return;

    if (p_first_resource == NULL)
    {
        p_first_resource = p_res;
        p_cur_resource = p_res;
    } else
    {
        p_cur_resource->next = p_res;
        p_cur_resource = p_res;
    }

    p_cur_resource->grove_name = grove_name;
    p_cur_resource->method_name = method_name;
    p_cur_resource->rw = rw;
    p_cur_resource->method_ptr = ptr;
    p_cur_resource->next = NULL;
    memcpy(p_cur_resource->arg_types, arg_types, sizeof(p_cur_resource->arg_types));

}

resource_t* __find_resource(char *name, char *method)
{
    resource_t *ptr;
    for (ptr = p_first_resource; ptr; ptr = ptr->next)
    {
        if (strncmp(name, ptr->grove_name, 33) == 0 && strncmp(method, ptr->method_name, 33) == 0)
        {
            return ptr;
        }
    }
    return NULL;
}

int __convert_arg(uint8_t *arg_buff, char **buff, int type)
{
    int i;
    uint32_t ui;
    switch (type)
    {
        case TYPE_BOOL:
            {
                i = atoi((const char *)(*buff));
                ui = abs(i);
                memcpy(arg_buff, &ui, 1);
                return 1;
                break;
            }
        case TYPE_UINT8:
            {
                i = atoi((const char *)(*buff));
                ui = abs(i);
                memcpy(arg_buff, &ui, 1);
                return 1;
                break;
            }
        case TYPE_UINT16:
            {
                i = atoi((const char *)(*buff));
                ui = abs(i);
                memcpy(arg_buff, &ui, 2);
                return 2;
                break;
            }
        case TYPE_UINT32:
            {
                int32_t l = atol((const char *)(*buff));
                ui = abs(l);
                memcpy(arg_buff, &ui, 4);
                return 4;
                break;
            }
        case TYPE_INT8:
            {
                i = atoi((const char *)(*buff));
                char c = i;
                memcpy(arg_buff, &c, 1);
                return 1;
                break;
            }
        case TYPE_INT16:
            {
                i = atoi((const char *)(*buff));
                memcpy(arg_buff, &i, 2);
                return 2;
                break;
            }
        case TYPE_INT32:
            {
                int32_t l = atol((const char *)(*buff));
                memcpy(arg_buff, &l, 4);
                return 4;
                break;
            }
        case TYPE_FLOAT:
            {
                float f = atof((const char *)(*buff));
                memcpy(arg_buff, &f, 4);
                return 4;
                break;
            }
        case TYPE_STRING:
            {
                memcpy(arg_buff, buff, 4);
                return 4;
                break;
            }
        default:
            break;
    }
    return 0;
}

int req_type;

char buff[33];
int  offset = 0;
int  arg_index = 0;
char grove_name[33];
char method_name[33];
char ch;
uint8_t arg_buff[4 * MAX_INPUT_ARG_LEN];
int arg_offset;
resource_t *p_resource;

void rpc_server_loop()
{
    switch (parse_stage)
    {
        case PARSE_GET_POST:
            {
                buff[0] = buff[1]; buff[1] = buff[2]; buff[2] = buff[3];
                buff[3] = stream_read();
                if (memcpy(buff, "GET", 3) == 0 || memcpy(buff, "get", 3) == 0)
                {
                    req_type = REQ_GET;
                }
                if (memcpy(buff, "POST", 4) == 0 || memcpy(buff, "post", 4) == 0)
                {
                    req_type = REQ_POST;
                    stream_read();  //read " " out
                }
                ch = stream_read();
                if (ch != '/')
                {
                    //error request format
                    writer_print(TYPE_STRING, "BAD REQUEST");
                } else
                {
                    parse_stage = PARSE_GROVE_NAME;
                    p_resource = NULL;
                    offset = 0;
                }
                break;
            }
        case PARSE_GROVE_NAME:
            {
                ch = stream_read();
                if (ch != '/' && offset <= 31)
                {
                    buff[offset++] = ch;
                } else
                {
                    buff[offset] = '\0';
                    memcpy(grove_name, buff, offset+1);
                    while (ch != '/')
                    {
                        ch = stream_read();
                    }
                    parse_stage = PARSE_METHOD;
                    offset = 0;
                }
                break;
            }
        case PARSE_METHOD:
            {
                ch = stream_read();
                if (ch == '\r' || ch == '\n')
                {
                    buff[offset] = '\0';
                    memcpy(method_name, buff, offset + 1);
                    parse_stage = PARSE_CALL;
                }
                else if (ch == '/')
                {
                    buff[offset] = '\0';
                    memcpy(method_name, buff, offset + 1);
                    parse_stage = PRE_PARSE_ARGS;
                }
                else if (offset >= 32)
                {
                    buff[offset] = '\0';
                    memcpy(method_name, buff, offset + 1);
                    while (ch != '/' && ch != '\r' && ch != '\n')
                    {
                        ch = stream_read();
                    }
                    if (ch == '\r' || ch == '\n') parse_stage = PARSE_CALL;
                    else parse_stage = PRE_PARSE_ARGS;
                }
                else
                {
                    buff[offset++] = ch;
                }
                break;
            }
        case PRE_PARSE_ARGS:
            {
                p_resource = __find_resource((char *)grove_name, (char *)method_name);
                if (!p_resource)
                {
                    writer_print(TYPE_STRING, "METHOD NOT FOUND");
                    parse_stage = PARSE_GET_POST;
                    break;
                }
                parse_stage = PARSE_ARGS;
                arg_index = 0;
                arg_offset = 0;
                offset = 0;
                break;
            }
        case PARSE_ARGS:
            {
                ch = stream_read();
                if (ch == '\r' || ch == '\n' || ch == '/')
                {
                    buff[offset] = '\0';
                } else if (offset >= 32)
                {
                    buff[offset] = '\0';
                    while (ch != '/' && ch != '\r' && ch != '\n')
                    {
                        ch = stream_read();
                    }

                } else
                {
                    buff[offset++] = ch;
                }

                if (ch == '/')
                {
                    char *p = buff;
                    int len = __convert_arg(arg_buff + arg_offset, &p, p_resource->arg_types[arg_index++]);
                    arg_offset += len;
                }
                if (ch == '\r' || ch == '\n')
                {
                    if (arg_index<3 && p_resource->arg_types[arg_index+1] != TYPE_NONE)
                    {
                        writer_print(TYPE_STRING, "MISSING ARGS");
                        parse_stage = PARSE_GET_POST;
                        break;
                    }
                    char *p = buff;
                    int len = __convert_arg(arg_buff + arg_offset, &p, p_resource->arg_types[arg_index++]);
                    arg_offset += len;
                    parse_stage = PARSE_CALL;
                }
                break;
            }
        case PARSE_CALL:
            {
                if(!p_resource)
                    p_resource = __find_resource((char *)grove_name, (char *)method_name);

                if (!p_resource)
                {
                    writer_print(TYPE_STRING, "METHOD NOT FOUND");
                    parse_stage = PARSE_GET_POST;
                    break;
                }
                writer_print(TYPE_STRING, "{");
                p_resource->method_ptr(arg_buff);
                writer_print(TYPE_STRING, "}");

                parse_stage = PARSE_GET_POST;
                break;
            }
        default:
            break;
    }

}
