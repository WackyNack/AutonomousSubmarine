// Things to code:


// 1. Logic integration - This code will be the full connector that takes the data provided from the other sources and takes the compilation from main.cpp to direct thruster control ----------------- Added
// 2. Thruster logic			
//					 - Thrusters must have smart logic that will force them to automatically determine the amount of thrust/power they need		----------------------- Added
//					 - This will also require me to create limits and maximums for thrust				------------------ Added
//					 - This also includes failsafes to prevent thruster burnout							------------------ Added
//					 - Implement "power-ramping" (thruster won't go from 0% to 100% in 10 ms)			------------------ Added
//					 - Tether tension checks for diving to ensure the tension doesn't drag the float underwater or snap the tether.		--------------------- Can't do without proper components
// 3. Code the mutex, so thrust isn't accidentally interrupted and causes the hull to halt movement (unless given an exception like an object)
// 4. Ensure the code is able to provide data and integrate into other code	-------------------------- Added
//					 - This means everything else will also have their source code and header files tweaked and added upon to supplement the thrusters
// 5. Constant updating loop providing data on the current status of the thrusters	----------------- Added
//					 - I will likely build a website where all this can be viewed to prevent looking at log/console clutter when the program runs
// 6. Speed and RPM tracking ------------------ Can't do without proper componenets
//					 - Will need to check parts list if we have the capability for that
// 7. Communication checks       ------------------ Added
//					 - Are the thrusting recieving commands properly from the obstacle logic and AI?
// 8. Manual ovveride control over thrusters if need be ---------- Added
//					 - Allows us to stop current program commands from the AI if need be
//					 - Ideally, this won't stop information given by the AI and obstacle logic
// 9. Introduce failsafes to prevent data/file corruption	------------ Added
// 10. Create  timekeeps to monitor/predict thruster resilience		---------- Deemed unecessary for now, but will be added in the future if needed
//					 - Rough idea whether or not to stop the program and go manual or execute ascension
// 11. Introduce heartbeat code to prevent prepetual, unstoppable thrust --------- Added

#pragma once
#include <iostream>
#include <stdexcept>
#include <mutex>
#include <algorithm>
#include "../WaypointSystemHUAC/WaypointSystemHUAC.h"
#include "../WaypointSystemHUAC/Vector.h"
#include "../ObjectLogic/GODS_EYE.h"
#include "../OpticalSensorFirmware/SonarSensorHub.h"
#include <chrono>

enum class propulsionMode : uint8_t {
	MANUAL_OVERRIDE,
	AUTONOMOUS_OVVERIDE,
	FORCE_ASCENT,
	DIVING_HALT
};

enum class thrusterChannel {
	THRUSTERONE,
	THRUSTERTWO,
	THRUSTERTHREE,
	THRUSTERFOUR,
	THRUSTERFIVE,
	THRUSTERSIX
};

struct Thruster_Frame {
	thrusterChannel channel;
	float Thruster_TOTALPOWER;
	bool Thruster_PILOT_OVERRIDE;
};

class Thruster_Controller {
public:
	Thruster_Controller() noexcept;
	~Thruster_Controller() = default;

	Thruster_Controller(const Thruster_Controller&) = delete;
	Thruster_Controller& operator=(const Thruster_Controller&) = delete;

	[[nodiscard]] propulsionMode CurrentPropulsionMode() const noexcept;
	[[nodiscard]] Vector3 CurrentThrustForce() const noexcept;

	Thruster_Frame motor_frames[6];



	void THRUSTER_CONTROL_PIPELINE(thrusterChannel channel, propulsionMode mode, float Thruster_TOTALPOWER, bool Thruster_Pilot_OVERRIDE) noexcept;
	void Tether_TensionCheck(bool criticalTetherFlag) noexcept;
	void Thruster_PowerRamping(float deltaTime);
	void Thruster_Controller_Hull_Check() noexcept;

private:
	mutable std::mutex controllerMutex;

	Vector3 Thruster_PAST_OUTPUT{ 0.0f, 0.0f, 0.0f };
	Vector3 Thruster_CURRENT_OUTPUT{ 0.0f, 0.0f, 0.0f };
	Vector3 Thruster_Pilot_HEADING{ 0.0f, 0.0f, 0.0f };

	propulsionMode currentMode{ propulsionMode::AUTONOMOUS_OVVERIDE };
	bool is_MANUAL_OVVERIDE_ACTIVE{ false };
	bool tetherCompromised{ false }; // Fully declared for the tracking flags

	static constexpr float MAXTHRUST = 400.0f;
	static constexpr float MINTHRUST = 0.0f;
	static constexpr float MAXRAMP = 50.0f;

	unsigned long last_heart_beat_time = 0;
};
