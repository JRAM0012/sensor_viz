#include <stdlib.h>
#include <string.h>
#include "utils.h"

// #define CRUDE_IMP

#ifdef CRUDE_IMP
int catoi(char* start, char* end)
{
    int val = 0;
    int string_len = end - start;
    for(size_t idx = 0; idx < string_len; idx++)
    {
        // printf("idx: %d, char: (%c, %d) , val: %d\n", idx, start[idx], start[idx], val);
        switch(start[idx])
        {
            case '1': {val = val*10 + 1; break;}
            case '2': {val = val*10 + 2; break;}
            case '3': {val = val*10 + 3; break;}
            case '4': {val = val*10 + 4; break;}
            case '5': {val = val*10 + 5; break;}
            case '6': {val = val*10 + 6; break;}
            case '7': {val = val*10 + 7; break;}
            case '8': {val = val*10 + 8; break;}
            case '9': {val = val*10 + 9; break;}
            case '0': {val = val*10 + 0; break;}
        }
    }
    // printf("---\n");
    return val;
}

float catof(char* start, char* end)
{
    float val = 0;
    int string_len = end - start;
    int des = 0;
    bool should_invert = false;
    for(size_t idx = 0; idx < string_len; idx++)
    {
        // printf("idx: %d, char: (%c, %d) , val: %f\n", idx, start[idx], start[idx], val);
        switch(start[idx])
        {
            case '1': {val = val*10 + 1;     break;}
            case '2': {val = val*10 + 2;     break;}
            case '3': {val = val*10 + 3;     break;}
            case '4': {val = val*10 + 4;     break;}
            case '5': {val = val*10 + 5;     break;}
            case '6': {val = val*10 + 6;     break;}
            case '7': {val = val*10 + 7;     break;}
            case '8': {val = val*10 + 8;     break;}
            case '9': {val = val*10 + 9;     break;}
            case '0': {val = val*10 + 0;     break;}
            case '.': {des = idx;            break;}
            case '-': {should_invert = true; break;}
        }
    }
    if(should_invert) val = -val;
    val = val / pow(10, (string_len - des - 1));
    // printf("---\n");
    return val;
}

#else
#define CUSTOM_A_TO_D 20
char custom_array_to_data[CUSTOM_A_TO_D];

int catoi(char* start, char* end)
{
    int val = 0;
    int string_len = end - start;
    memset(custom_array_to_data, 0, CUSTOM_A_TO_D);
    strncpy(custom_array_to_data, start, string_len);
    custom_array_to_data[CUSTOM_A_TO_D] = '\0';
    val = atoi(custom_array_to_data);
    // printf("custom char %s, %d\n", custom_array_to_data, val);
    return val;
}


float catof(char* start, char* end)
{
    float val = 0;
    int string_len = end - start;
    memset(custom_array_to_data, '0', CUSTOM_A_TO_D);
    strncpy(custom_array_to_data, start, string_len);
    custom_array_to_data[CUSTOM_A_TO_D] = '\0';
    val = atof(custom_array_to_data);
    // printf("custom char %s, %f\n", custom_array_to_data, val);
    return val;
}

#endif