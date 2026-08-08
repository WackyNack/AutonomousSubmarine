#pragma once
#include <iostream>
#include <chrono>

[[nodiscard]] unsigned long getTheTimeinMillis() noexcept;
void TriggerPingThing(int pin) noexcept;
[[nodiscard]] float ReadDistanceThing(int pin) noexcept;

class SonarSensorHub {
private:
    const int TRIGGER_PINS[3] = { 2, 4, 6 };
    const int ECHO_PINS[3] = { 3, 5, 7 };

    int active_sensor_index = 0;
    const int SENSOR_TOTAL = 3;
    unsigned long recent_ping_time = 0;
    const unsigned long PING_INTERVAL_MS = 12;

public:
    SonarSensorHub() noexcept = default;
    ~SonarSensorHub() = default;

    float recent_distance[3] = { 0.0f, 0.0f, 0.0f };

    void initialization() noexcept;
    void update() noexcept;

    [[nodiscard]] int GetActiveSensorIndex() const noexcept { return active_sensor_index; }
};
