#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpu_sensor.h"
#include "json_utils.h"

void destroy_sensor_motion(sensor_motion sm)
{
    free(sm.data_array);
}

sensor_motion build_sensor_motion(char* filename)
{
    TraceLog(LOG_INFO, "reading file: %s\n", filename);
    FILE* jsonfile;
    jsonfile = fopen(filename, "r");
    if(!jsonfile)
    {
        TraceLog(LOG_FATAL, "cannot open file %s to read.", filename);
        exit(-1);
    }

    char linebuffer[MAX_LINE_CHARS];

    int total_lines = 0;
    while(fgets(linebuffer, MAX_LINE_CHARS, jsonfile))
        total_lines++;
    
    TraceLog(LOG_INFO, "total_lines: %d\n", total_lines);

    fseek(jsonfile, 0, SEEK_SET);

    sensor_reading* data_array = (sensor_reading*) malloc(sizeof(sensor_reading) * total_lines);

    TraceLog(LOG_INFO, "allocated memory size: %d\n", sizeof(sensor_reading) * total_lines);

    int min_timestamp = INT_MAX, max_timestamp = 0;

    int ln = 0;
    int lines_to_read = total_lines;
    // lines_to_read = 10;
    while(fgets(linebuffer, MAX_LINE_CHARS, jsonfile) && ln < lines_to_read)
    {
        sensor_reading *data = &data_array[ln];
        
        linebuffer[MAX_LINE_CHARS - 1] = '\0';
        char* capture_start = NULL, *capture_end = NULL;
        int idx = 0;
        // printf("%s", linebuffer);

        idx = strcspn(linebuffer, ":") + 2;
        capture_start = &linebuffer[idx];
        idx = strcspn(capture_start, ",");
        capture_end = &capture_start[idx];
        data->timestamp = catoi(capture_start, capture_end);
        if(min_timestamp > data->timestamp)
            min_timestamp = data->timestamp;
        if(max_timestamp < data->timestamp)
            max_timestamp = data->timestamp;
        // int timestamp = catoi(capture_start, capture_end);
        // printf("timestamp: %d\n", timestamp);

        idx = strcspn(capture_end, "z") + 4;
        capture_start = &capture_end[idx];
        idx = strcspn(capture_start, ",");
        capture_end = &capture_start[idx];
        data->accel.z = catof(capture_start, capture_end);
        // float acclz = catof(capture_start, capture_end);
        // printf("acclz: %f\n", acclz);

        idx = strcspn(capture_end, "y") + 4;
        capture_start = &capture_end[idx];
        idx = strcspn(capture_start, ",");
        capture_end = &capture_start[idx];
        data->accel.y = catof(capture_start, capture_end);
        // float accly = catof(capture_start, capture_end);
        // printf("accly: %f\n", accly);

        idx = strcspn(capture_end, "x") + 4;
        capture_start = &capture_end[idx];
        idx = strcspn(capture_start, "}");
        capture_end = &capture_start[idx];
        data->accel.x = catof(capture_start, capture_end);
        // float acclx = catof(capture_start, capture_end);
        // printf("acclx: %f\n", acclx);

        idx = strcspn(capture_end, "z") + 4;
        capture_start = &capture_end[idx];
        idx = strcspn(capture_start, ",");
        capture_end = &capture_start[idx];
        data->gyro.z = catof(capture_start, capture_end);
        // float gyroz = catof(capture_start, capture_end);
        // printf("gyroz: %f\n", gyroz);

        idx = strcspn(capture_end, "y") + 4;
        capture_start = &capture_end[idx];
        idx = strcspn(capture_start, ",");
        capture_end = &capture_start[idx];
        data->gyro.y = catof(capture_start, capture_end);
        // float gyroy = catof(capture_start, capture_end);
        // printf("gyroy: %f\n", gyroy);

        idx = strcspn(capture_end, "x") + 4;
        capture_start = &capture_end[idx];
        idx = strcspn(capture_start, "}");
        capture_end = &capture_start[idx];
        data->gyro.x = catof(capture_start, capture_end);
        // float gyrox = catof(capture_start, capture_end);
        // printf("gyrox: %f\n", gyrox);

        idx = strcspn(capture_end, ":") + 2;
        capture_start = &capture_end[idx];
        idx = strcspn(capture_start, "}");
        capture_end = &capture_start[idx];
        data->temp = catof(capture_start, capture_end);
        // float temperature = catof(capture_start, capture_end);
        // printf("temperature: %f\n", temperature);


        ln++;
    }

    fclose(jsonfile);
    sensor_motion sm;
    sm.data_array = data_array;
    sm.data_len   = ln;
    sm.recording_len = max_timestamp - min_timestamp;

    return sm;
}

