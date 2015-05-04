

#include "suli2.h"
#include "GroveExample.h"

I2C_T i2c;

void grove_example_init(int pinsda, int pinscl)
{
    suli_i2c_init(&i2c, pinsda, pinscl);
}

bool grove_example_read_temp(int *temp)
{
    *temp = 30;
    return true;
}

bool grove_example_read_uint8_value(uint8_t *value)
{
    *value = 255;
    return true;
}

bool grove_example_read_humidity(float *humidity)
{
    *humidity = 52.5;
    return true;
}

bool grove_example_read_acc(float *ax, float *ay, float *az)
{
    *ax = 12.3; *ay = 45.6; *az = 78.0;
    return true;
}
bool grove_example_read_compass(float *cx, float *cy, float *cz, int *degree)
{
    *cx = 12.3; *cy = 45.6; *cz = 78.0; *degree = 90;
    return true;
}

bool grove_example_write_acc_mode(uint8_t mode)
{
    suli_i2c_write(&i2c, 0x00, &mode, 1);
    return true;
}

bool grove_example_write_float_value(float f)
{
    return false;
}

bool grove_example_write_multi_value(int a, float b, int8_t c)
{
    _grove_example_internal_function(b);
    return true;
}

void _grove_example_internal_function(float x)
{
    ;
}
