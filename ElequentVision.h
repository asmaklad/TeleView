#ifndef ELEQUENTVISION_H
#define ELEQUENTVISION_H

// suorces
// https://github.com/eloquentarduino/EloquentArduino/blob/1.1.8/examples/ESP32CameraNaiveMotionDetection/ESP32CameraNaiveMotionDetection.ino
// https://eloquentarduino.github.io/2020/05/easier-faster-pure-video-esp32-cam-motion-detection/?utm_source=old_version

#include "esp_camera.h"
#include "camera_pins.h"

#define FRAME_SIZE FRAMESIZE_QVGA
#define WIDTH 320
#define HEIGHT 240
#define BLOCK_SIZE 10
#define W (WIDTH / BLOCK_SIZE)
#define H (HEIGHT / BLOCK_SIZE)
#define BLOCK_DIFF_THRESHOLD 0.2
#define IMAGE_DIFF_THRESHOLD 0.1
#define EV_DEBUG 0


uint16_t prev_frame[H][W] = { 0 };
uint16_t current_frame[H][W] = { 0 };


bool setup_camera(framesize_t);
bool capture_still();
bool motion_detect();
void update_frame();
void print_frame(uint16_t frame[H][W]);


/**

void setup() {
    Serial.begin(115200);
    Serial.println(setup_camera(FRAME_SIZE) ? "OK" : "ERR INIT");
}

void loop() {
    if (!capture_still()) {
        Serial.println("Failed capture");
        delay(3000);

        return;
    }

    if (motion_detect()) {
        Serial.println("Motion detected");
    }

    update_frame();
    Serial.println("=================");
}


/**
 *
 */
bool setup_camera(framesize_t frameSize) {

    sensor_t *sensor = esp_camera_sensor_get();
    sensor->set_pixformat(sensor, PIXFORMAT_GRAYSCALE );
    sensor->set_framesize(sensor, frameSize);

    return true;
}

/**
 * Capture image and do down-sampling
 */
bool capture_still() {
    camera_fb_t *frame_buffer = esp_camera_fb_get();

    if (!frame_buffer)
        return false;

    // set all 0s in current frame
    for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++)
            current_frame[y][x] = 0;


    // down-sample image in blocks
    for (uint32_t i = 0; i < WIDTH * HEIGHT; i++) {
        const uint16_t x = i % WIDTH;
        const uint16_t y = floor(i / WIDTH);
        const uint8_t block_x = floor(x / BLOCK_SIZE);
        const uint8_t block_y = floor(y / BLOCK_SIZE);
        const uint8_t pixel = frame_buffer->buf[i];
        const uint16_t current = current_frame[block_y][block_x];

        // average pixels in block (accumulate)
        current_frame[block_y][block_x] += pixel;
    }

    // average pixels in block (rescale)
    for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++)
            current_frame[y][x] /= BLOCK_SIZE * BLOCK_SIZE;

#if DEBUG
    Serial.println("Current frame:");
    print_frame(current_frame);
    Serial.println("---------------");
#endif
    esp_camera_fb_return(frame_buffer);
    return true;
}


/**
 * Compute the number of different blocks
 * If there are enough, then motion happened
 */
bool motion_detect() {
    uint16_t changes = 0;
    const uint16_t blocks = (WIDTH * HEIGHT) / (BLOCK_SIZE * BLOCK_SIZE);

    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            float current = current_frame[y][x];
            float prev = prev_frame[y][x];
            float delta = abs(current - prev) / prev;

            if (delta >= BLOCK_DIFF_THRESHOLD) {
#if EV_DEBUG
                Serial.print("diff\t");
                Serial.print(y);
                Serial.print('\t');
                Serial.println(x);
#endif

                changes += 1;
            }
        }
    }

    Serial.print("Changed ");
    Serial.print(changes);
    Serial.print(" out of ");
    Serial.println(blocks);

    return (1.0 * changes / blocks) > IMAGE_DIFF_THRESHOLD;
}


/**
 * Copy current frame to previous
 */
void update_frame() {
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            prev_frame[y][x] = current_frame[y][x];
        }
    }
}

/**
 * For serial debugging
 * @param frame
 */
void print_frame(uint16_t frame[H][W]) {
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            Serial.print(frame[y][x]);
            Serial.print('\t');
        }

        Serial.println();
    }
}

#endif //ELEQUENTVISION_H