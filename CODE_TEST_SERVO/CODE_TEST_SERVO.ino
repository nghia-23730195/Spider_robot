#include <Servo.h>

#define NUM_LEGS 8
Servo servos[NUM_LEGS];  // Mảng chứa các servo

// Chân kết nối servo với Arduino (A0 - A7)
const int servoPins[NUM_LEGS] = {A0, A1, A2, A3, A4, A5, 4, 7};

// Góc trung bình của các chân
int neutralPos[NUM_LEGS] = {90, 90, 90, 90, 90, 90, 90, 90};

// Chuyển động của từng chân
int stepPattern[4][NUM_LEGS] = {
    {120,  60, 120, 60, 120, 60, 120, 60},  // Bước 1
    {90,  90, 90,  90, 90,  90, 90,  90},  // Về giữa
    {60, 120, 60, 120, 60, 120, 60, 120},  // Bước 2
    {90,  90, 90,  90, 90,  90, 90,  90}   // Về giữa
};

void setup() {
    for (int i = 0; i < NUM_LEGS; i++) {
        servos[i].attach(servoPins[i]);
        servos[i].write(neutralPos[i]); // Đặt về vị trí ban đầu
    }
    delay(1000);
}

void loop() {
    for (int step = 0; step < 4; step++) {
        for (int i = 0; i < NUM_LEGS; i++) {
            servos[i].write(stepPattern[step][i]); // Di chuyển servo theo bước
        }
        delay(300); // Tốc độ di chuyển (điều chỉnh theo yêu cầu)
    }
}
