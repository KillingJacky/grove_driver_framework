
#include "suli2.h"
#include "rpc_server.h"

//INCLUDE_START
#include "grove_example_gen.h"


void rpc_server_register_resources()
{
    //register resources of loaded grove modules
    uint8_t arg_types[MAX_INPUT_ARG_LEN];
    //REGISTER_START
    //GroveExample
    GroveExample *GroveExample1 = new GroveExample(P0_27, P0_28);

    memset(arg_types, TYPE_NONE, MAX_INPUT_ARG_LEN);
    rpc_server_register_method("GroveExample1", "temp", METHOD_READ, __grove_example_read_temp, GroveExample1, arg_types);
    rpc_server_register_method("GroveExample1", "uint8_value", METHOD_READ, __grove_example_read_uint8_value, GroveExample1, arg_types);
    rpc_server_register_method("GroveExample1", "humidity", METHOD_READ, __grove_example_read_humidity, GroveExample1, arg_types);
    rpc_server_register_method("GroveExample1", "acc", METHOD_READ, __grove_example_read_acc, GroveExample1, arg_types);
    rpc_server_register_method("GroveExample1", "compass", METHOD_READ, __grove_example_read_compass, GroveExample1, arg_types);

    memset(arg_types, TYPE_NONE, MAX_INPUT_ARG_LEN);
    arg_types[0] = TYPE_UINT8;
    rpc_server_register_method("GroveExample1", "acc_mode", METHOD_WRITE, __grove_example_write_acc_mode, GroveExample1, arg_types);
    memset(arg_types, TYPE_NONE, MAX_INPUT_ARG_LEN);
    arg_types[0] = TYPE_FLOAT;
    rpc_server_register_method("GroveExample1", "float_value", METHOD_WRITE, __grove_example_write_float_value, GroveExample1, arg_types);
    memset(arg_types, TYPE_NONE, MAX_INPUT_ARG_LEN);
    arg_types[0] = TYPE_INT;
    arg_types[1] = TYPE_FLOAT;
    arg_types[2] = TYPE_INT8;
    rpc_server_register_method("GroveExample1", "multi_value", METHOD_WRITE, __grove_example_write_multi_value, GroveExample1, arg_types);

    //END
}
