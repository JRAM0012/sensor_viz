#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define MAX_LINE_CHARS 254

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

int catoi(char*, char*);
float catof(char*, char*);

sensor_motion build_sensor_motion(char* filename);
void destroy_sensor_motion(sensor_motion sm);

int main()
{
    char* filename = "C:/Users/jayar/source/sensor_viz/bin/json_data.txt";
    sensor_motion sm = build_sensor_motion(filename);

    typedef struct animation {
        int frame_index;
        unsigned int end_index;
        sensor_motion sm;
        bool playing;
    }animation;

    animation sample_animation;
    sample_animation.sm = sm;
    sample_animation.frame_index = 0;
    sample_animation.end_index = sm.data_len;
    sample_animation.playing = false;


    int screenWidth = 1270, screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "sensorViz");
    SetTargetFPS(144);

    int start_time = 0;

    Camera3D camera3   = { 0 };
    camera3.position   = (Vector3) { 10.0f, 10.0f, 1.0f  };
    camera3.target     = (Vector3) {  0.0f,  0.0f,  0.0f };
    camera3.up         = (Vector3) {  0.0f,  1.0f,  0.0f };
    camera3.fovy       = 45.0f;
    camera3.projection = CAMERA_PERSPECTIVE;

    Model model;
    // model = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));
    model = LoadModel("C:/Users/jayar/source/sensor_viz/bin/assets/plane.obj");
    Texture2D texture = LoadTexture("C:/Users/jayar/source/sensor_viz/bin/assets/plane_texture_diffuse.png");
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    Vector3 position = (Vector3) { 0.0f, 0.0f, 0.0f };

    float scale_acceleration = 1.0f;

    while(!WindowShouldClose())
    {
        UpdateCamera(&camera3, CAMERA_CUSTOM);
    
        BeginDrawing();
    
        ClearBackground(RAYWHITE);

        int scrub_bar_width = 50;
        float frame_index = (float) sample_animation.frame_index;
        int val = GuiSlider( (Rectangle) { 0.0f, GetScreenHeight() - scrub_bar_width, GetScreenWidth(), scrub_bar_width }, NULL, NULL, &frame_index, 0, sample_animation.end_index);
        sample_animation.frame_index = (int) frame_index;

        GuiSlider((Rectangle) {100.0f, 100.0f, 110.0f, 15.0f}, "scale", NULL, &scale_acceleration, 1.0f, 10.0f);

        if(GuiButton((Rectangle) {100.0f, GetScreenHeight()  - scrub_bar_width - 40.0f, 110.0f, 30.0f}, "Previous Frame" ))
        {
            sample_animation.frame_index -= 1;
        }

        if(GuiButton((Rectangle) { 200.0f, GetScreenHeight()  - scrub_bar_width - 40.0f, 110.0f, 30.f }, sample_animation.playing == false ? "Play" : "Resume" ))
        {
            sample_animation.playing = !sample_animation.playing;
            start_time = GetTime();
        }

        if(GuiButton((Rectangle) {300.0f, GetScreenHeight()  - scrub_bar_width - 40.0f, 110.0f, 30.0f}, "Next Frame" ))
        {
            sample_animation.frame_index += 1;
        }

        DrawText(TextFormat("FPS: %d", GetFPS()), 12, 12, 12, RED);
        DrawText(TextFormat("frame id: %d", sample_animation.frame_index), 12, 22, 12, RED);
        DrawText(TextFormat("val: %d", val), 12, 42, 12, RED);

        BeginMode3D(camera3);

            DrawModel(model, position, 0.1f, RED);
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

        position.x = scale_acceleration * sample_animation.sm.data_array[sample_animation.frame_index].accel.x;
        position.y = scale_acceleration * sample_animation.sm.data_array[sample_animation.frame_index].accel.y;
        position.z = scale_acceleration * sample_animation.sm.data_array[sample_animation.frame_index].accel.z;


    }
    CloseWindow();

    destroy_sensor_motion(sm);
    // return 0;
}

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