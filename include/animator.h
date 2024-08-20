#ifndef __ANIMATOR__
#define __ANIMATOR__

#include "mpu_sensor.h"

typedef struct animation {
    int frame_index;
    unsigned int end_index;
    sensor_motion sm;
    bool playing;
    float scale_acceleration;
    int start_time;
    bool smooth_animation;
}animation;

void animator_draw_ui(animation* sample_animation);
void animator_scale(animation *sample_animation);
void animator_timeline_slider(animation *sample_animation);
void animator_smooth_animation_toggle(animation *sample_animation);
void animator_previous_frame(animation *sample_animation);
void animator_next_frame(animation *sample_animation);
void animator_play_button(animation* sample_animation);

#endif // __ANIMATOR__