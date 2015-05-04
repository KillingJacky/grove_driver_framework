
#include "suli2.h"
#include "rpc_server.h"

//INCLUDE_START
#include "GroveExample_Generated.h"


void rpc_server_register_resources()
{
    //register resources of loaded grove modules
    uint8_t arg_types[MAX_INPUT_ARG_LEN];
    //REGISTER_START
    //GroveExample
    grove_example_init(P0_27, P0_28);
    
    memset(arg_types, TYPE_NONE, MAX_INPUT_ARG_LEN);
    rpc_server_register_method("GroveExample", "temp", METHOD_READ, __grove_example_read_temp, arg_types);
    rpc_server_register_method("GroveExample", "uint8_value", METHOD_READ, __grove_example_read_uint8_value, arg_types);
    rpc_server_register_method("GroveExample", "humidity", METHOD_READ, __grove_example_read_humidity, arg_types);
    rpc_server_register_method("GroveExample", "acc", METHOD_READ, __grove_example_read_acc, arg_types);
    rpc_server_register_method("GroveExample", "compass", METHOD_READ, __grove_example_read_compass, arg_types);

    memset(arg_types, TYPE_NONE, MAX_INPUT_ARG_LEN);
    arg_types[0] = TYPE_UINT8;
    rpc_server_register_method("GroveExample", "acc_mode", METHOD_WRITE, __grove_example_write_acc_mode, arg_types);
    memset(arg_types, TYPE_NONE, MAX_INPUT_ARG_LEN);
    arg_types[0] = TYPE_FLOAT;
    rpc_server_register_method("GroveExample", "float_value", METHOD_WRITE, __grove_example_write_float_value, arg_types);
    memset(arg_types, TYPE_NONE, MAX_INPUT_ARG_LEN);
    arg_types[0] = TYPE_INT;
    arg_types[1] = TYPE_FLOAT;
    arg_types[2] = TYPE_INT8;
    rpc_server_register_method("GroveExample", "multi_value", METHOD_WRITE, __grove_example_write_multi_value, arg_types);

    //END
}
