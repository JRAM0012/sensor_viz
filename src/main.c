#include <math.h>
#include <stdbool.h>
#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "mpu_sensor.h"
#include "animator.h"

int main()
{
    char* filename = "C:/Users/jayar/source/sensor_viz/bin/assets/json_data.txt";
    sensor_motion sm = build_sensor_motion(filename);

    animation sample_animation;
    sample_animation.sm = sm;
    sample_animation.frame_index = 0;
    sample_animation.end_index = sm.data_len;
    sample_animation.playing = false;
    sample_animation.scale_acceleration = 1.0f;
    sample_animation.smooth_animation = false;

    int screenWidth = 1270, screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "sensorViz");
    SetTargetFPS(144);

    Camera3D camera3   = { 0 };
    camera3.position   = (Vector3) { 10.0f, 10.0f, 1.0f  };
    camera3.target     = (Vector3) {  0.0f,  0.0f,  0.0f };
    camera3.up         = (Vector3) {  0.0f,  1.0f,  0.0f };
    camera3.fovy       = 45.0f;
    camera3.projection = CAMERA_PERSPECTIVE;

    Model model;
    Texture2D texture;
    model = LoadModel("C:/Users/jayar/source/sensor_viz/bin/assets/drone.obj");
    texture = LoadTexture("C:/Users/jayar/source/sensor_viz/bin/assets/drone_texture.png");
    
    // model = LoadModel("C:/Users/jayar/source/sensor_viz/bin/assets/plane.obj");
    // texture = LoadTexture("C:/Users/jayar/source/sensor_viz/bin/assets/plane_texture_diffuse.png");
    
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    Vector3 angle = (Vector3) { 0.0f, 0.0f, 0.0f };
    float x = TextToFloat("1.2");
    Vector3 tempf3;

    while(!WindowShouldClose())
    {
        UpdateCamera(&camera3, CAMERA_CUSTOM);
    
        BeginDrawing();
    
        ClearBackground(RAYWHITE);

        animator_draw_ui(&sample_animation);

        DrawText(TextFormat("FPS: %d", GetFPS()), 12, 12, 12, RED);
        DrawText(TextFormat("frame id: %d", sample_animation.frame_index), 12, 22, 12, RED);
        DrawText(TextFormat("smooth: %d", sample_animation.smooth_animation), 12, 32, 12, RED);
        DrawText(TextFormat("ax: %f, ay: %f, az: %f", angle.x, angle.y, angle.z), 12, 42, 12, RED);
        DrawText(TextFormat("tx: %f, ty: %f, tz: %f", tempf3.x, tempf3.y, tempf3.z), 12, 52, 12, RED);

        model.transform = MatrixRotateXYZ((Vector3) { angle.x, angle.y, angle.z } );

        BeginMode3D(camera3);

            DrawModel(model, (Vector3) { 0.0f, 0.0f, 0.0f }, 1.0f, RED);
            DrawGrid(10, 1.0f);

        EndMode3D();


        EndDrawing();

        if(sample_animation.playing)
        {
            // sample_animation.frame_index = (int) GetTime() - start_time;
            sample_animation.frame_index++;
            if(sample_animation.frame_index > sample_animation.end_index - 1)
            {
                sample_animation.playing = false;
                sample_animation.frame_index = 0;
            }
        }
        Vector3* current_sample = &sample_animation.sm.data_array[sample_animation.frame_index].accel;
        Vector3* previous_sample = &sample_animation.sm.data_array[sample_animation.frame_index - 1].accel;
        if(sample_animation.smooth_animation && sample_animation.frame_index > 0)
        {
            tempf3.x = sample_animation.scale_acceleration * (current_sample->x * 0.1f + previous_sample->x * 0.9f);
            tempf3.y = sample_animation.scale_acceleration * (current_sample->y * 0.1f + previous_sample->y * 0.9f);
            tempf3.z = sample_animation.scale_acceleration * (current_sample->z * 0.1f + previous_sample->z * 0.9f);
        } else {
            tempf3.x = sample_animation.scale_acceleration * current_sample->x;
            tempf3.y = sample_animation.scale_acceleration * current_sample->y;
            tempf3.z = sample_animation.scale_acceleration * current_sample->z;
        }
        // https://youtu.be/RZd6XDx5VXo?t=764 Accelerometers and Gyroscopes - Sensor Fusion #1, Phil's Lab #33
        // https://youtu.be/p7tjtLkIlFo?t=428 How to Compute Roll and Pitch From Accelerometers, MicWro Engr
        angle.x = asinf(tempf3.x / -9.8f);
        angle.y = 0.0f;
        angle.z = atan2f(tempf3.y, tempf3.z);
        if(isnan(angle.x)) angle.x = 0.0f;
        if(isnan(angle.z)) angle.z = 0.0f;
    }
    CloseWindow();

    destroy_sensor_motion(sm);
    // return 0;
}

