#pragma once
#include <Servo.h>


enum class DoorState
{
    CLOSE,
    OPEN,
};


/**
 * 문 제어 클래스
 */
class Door
{
public:
    Door(int servo_pin, int led_pin);

    void open();
    void close();

private:
    /**
     * 문 각도 설정
     * @param new_angle 설정할 각도
     * @param smooth_steps 부드럽게 이동할 단계 수 (기본값 50)
     */
    void set_door_angle(float new_angle, int smooth_steps = 50);
    float get_door_angle();

private:
    Servo servo;
    int led_pin;

    float current_angle = 0;      // 현재 각도

    const int open_angle = 30; // 문 열림 각도
    const int close_angle = 100; // 문 닫힘 각도
};


Door::Door(int servo_pin, int led_pin)
    : led_pin(led_pin)
{
    servo.attach(servo_pin);
    pinMode(led_pin, OUTPUT);
}

inline void Door::open()
{
    // TODO: Implement this function
    set_door_angle(open_angle);
}

inline void Door::close()
{
    // TODO: Implement this function
    set_door_angle(close_angle);
}

inline void Door::set_door_angle(float new_angle, int smooth_steps)
{
    const float diff_angle = new_angle - current_angle;

    for (int i = 0; i < smooth_steps; ++i) {
        servo.write(static_cast<int>(current_angle += diff_angle / smooth_steps));
    }
}

inline float Door::get_door_angle()
{
    return current_angle;
}
