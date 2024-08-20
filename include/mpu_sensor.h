#ifndef __MPU_SENSOR__
#define __MPU_SENSOR__

#include "raylib.h"

typedef struct sensor_reading {
    long int timestamp;
    Vector3 accel;
    Vector3 gyro;
    float temp;
}sensor_reading;

typedef struct sensor_motion {
    sensor_reading *data_array;
    int data_len;
    int *recordings_per_second;
    int recording_len; // 60 seconds
}sensor_motion;

sensor_motion build_sensor_motion(char* filename);
void destroy_sensor_motion(sensor_motion sm);

#endif // __MPU_SENSOR__