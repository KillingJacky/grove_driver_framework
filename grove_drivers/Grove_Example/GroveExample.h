


#ifndef __GROVE_EXAMPLE_H__
#define __GROVE_EXAMPLE_H__

#include "suli2.h"

//GROVE_NAME        "Grove_Example"
//IF_TYPE           I2C

void grove_example_init(int pinsda, int pinscl);

bool grove_example_read_temp(int *temp);

bool grove_example_read_uint8_value(uint8_t *value);

bool grove_example_read_humidity(float *humidity);

bool grove_example_read_acc(float *ax, float *ay, float *az);

bool grove_example_read_compass(float *cx, float *cy, float *cz, int *degree);

bool grove_example_write_acc_mode(uint8_t mode);

bool grove_example_write_float_value(float f);

void _grove_example_internal_function(float x, uint8_t *buff, int len);

#endif
