#include <stddef.h>
#include "raygui.h"
#include "animator.h"

static int scrub_bar_width = 50;

void animator_draw_ui(animation* sample_animation)
{
    animator_scale(sample_animation);
    animator_timeline_slider(sample_animation);
    animator_play_button(sample_animation);
    animator_next_frame(sample_animation);
    animator_previous_frame(sample_animation);
    animator_smooth_animation_toggle(sample_animation);
}

void animator_smooth_animation_toggle(animation* sample_animation)
{
    GuiToggle((Rectangle) {100.0f, 10.0f, 60.0f, 10.0f}, "smooth", &sample_animation->smooth_animation);
}

void animator_scale(animation *sample_animation)
{
    GuiSlider((Rectangle) {100.0f, 100.0f, 110.0f, 15.0f}, "scale", NULL, &sample_animation->scale_acceleration, 1.0f, 100.0f);
}

void animator_timeline_slider(animation *sample_animation)
{
    float frame_index = (float) sample_animation->frame_index;
    int active = GuiSlider( (Rectangle) { 0.0f, GetScreenHeight() - scrub_bar_width, GetScreenWidth(), scrub_bar_width }, NULL, NULL, &frame_index, 0, sample_animation->end_index);
    sample_animation->frame_index = (int) frame_index;
}

void animator_previous_frame(animation *sample_animation)
{
    if(GuiButton((Rectangle) {100.0f, GetScreenHeight()  - scrub_bar_width - 40.0f, 110.0f, 30.0f}, "Previous Frame" )
        || IsKeyPressed(KEY_Q)
        || IsKeyDown(KEY_Q))
    {
        sample_animation->playing = false;
        sample_animation->frame_index--;
    }
}

void animator_next_frame(animation *sample_animation)
{
    if(GuiButton((Rectangle) {300.0f, GetScreenHeight()  - scrub_bar_width - 40.0f, 110.0f, 30.0f}, "Next Frame" )
    || IsKeyPressed(KEY_E)
    || IsKeyDown(KEY_E))
    {
        sample_animation->playing = false;
        sample_animation->frame_index++;
    }
}


void animator_play_button(animation* sample_animation)
{
    if(GuiButton((Rectangle) { 200.0f, GetScreenHeight()  - scrub_bar_width - 40.0f, 110.0f, 30.f }, sample_animation->playing == false ? "Play" : "Resume" )
    || IsKeyPressed(KEY_SPACE))
    {
        sample_animation->playing = !sample_animation->playing;
        sample_animation->start_time = GetTime();
    }
}