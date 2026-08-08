#include "SonarSensorHub.h"
#include <thread>
#include <cstdlib>

unsigned long getTheTimeinMillis() noexcept {
    static const auto start = std::chrono::steady_clock::now();
    const auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
}

void TriggerPingThing(int pin) noexcept {
    std::cout << "[HARDWARE] Pulsing Trigger Pin: " << pin << '\n';
}

float ReadDistanceThing(int pin) noexcept {
    // Standard simulation of raw acoustic reflections mapping between 0.5m and 4.5m
    return 0.5f + (static_cast<float>(rand() % 100) / 25.0f);
}

void SonarSensorHub::initialization() noexcept {
    std::cout << "[FIRMWARE] Activating Sequenced Sonar Hub Layer...\n";
    recent_ping_time = getTheTimeinMillis();
    TriggerPingThing(TRIGGER_PINS[active_sensor_index]);
}

void SonarSensorHub::update() noexcept {
    const unsigned long currentTime = getTheTimeinMillis();

    if (currentTime - recent_ping_time >= PING_INTERVAL_MS) {
        recent_distance[active_sensor_index] = ReadDistanceThing(ECHO_PINS[active_sensor_index]);

        // Shift our active pointer pointer to the next pin to keep the ping sequence cycling
        active_sensor_index = (active_sensor_index + 1) % SENSOR_TOTAL;

        // Fire the next pin in line
        TriggerPingThing(TRIGGER_PINS[active_sensor_index]);
        recent_ping_time = currentTime;
    }
}
