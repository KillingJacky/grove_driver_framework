

#include "GroveExample.h"
#include "GroveExample_Generated.h"
#include "rpc_server.h"
#include "rpc_stream.h"


void __grove_example_read_temp(uint8_t *input)
{
    int temp;
    if(grove_example_read_temp(&temp))
    {
        writer_print(TYPE_INT, &temp);
    } else
    {
        writer_print(TYPE_STRING, "Failed");
    }
}

void __grove_example_read_uint8_value(uint8_t *input)
{
    uint8_t value;
    if(grove_example_read_uint8_value(&value))
    {
        writer_print(TYPE_UINT8, &value);
    } else
    {
        writer_print(TYPE_STRING, "Failed");
    }
}

void __grove_example_read_humidity(uint8_t *input)
{
    float humidity;
    if(grove_example_read_humidity(&humidity))
    {
        writer_print(TYPE_FLOAT, &humidity);
    } else
    {
        writer_print(TYPE_STRING, "Failed");
    }
}

void __grove_example_read_acc(uint8_t *input)
{
    float ax;
    float ay;
    float az;
    if(grove_example_read_acc(&ax, &ay, &az))
    {
        writer_print(TYPE_FLOAT, &ax);
        writer_print(TYPE_FLOAT, &ay);
        writer_print(TYPE_FLOAT, &az);
    } else
    {
        writer_print(TYPE_STRING, "Failed");
    }
}

void __grove_example_read_compass(uint8_t *input)
{
    float cx;
    float cy;
    float cz;
    int degree;
    if(grove_example_read_compass(&cx, &cy, &cz, &degree))
    {
        writer_print(TYPE_FLOAT, &cx);
        writer_print(TYPE_FLOAT, &cy);
        writer_print(TYPE_FLOAT, &cz);
        writer_print(TYPE_INT, &degree);
    } else
    {
        writer_print(TYPE_STRING, "Failed");
    }
}

void __grove_example_write_acc_mode(uint8_t *input)
{
    uint8_t mode;
    mode = *((uint8_t *)input); input += sizeof(uint8_t);
    if(grove_example_write_acc_mode(mode))
    {
        writer_print(TYPE_STRING, "OK");
    } else
    {
        writer_print(TYPE_STRING, "Failed");
    }
}

void __grove_example_write_float_value(uint8_t *input)
{
    float f;
    f = *((float *)input); input += sizeof(float);
    if(grove_example_write_float_value(f))
    {
        writer_print(TYPE_STRING, "OK");
    } else
    {
        writer_print(TYPE_STRING, "Failed");
    }
}
