#include "ThrusterController.h"
#include <cmath>

propulsionMode Thruster_Controller::CurrentPropulsionMode() const noexcept {
	std::lock_guard<std::mutex> lock(controllerMutex);
	return currentMode;
}

Vector3 Thruster_Controller::CurrentThrustForce() const noexcept {
	std::lock_guard<std::mutex> lock(controllerMutex);
	return Thruster_CURRENT_OUTPUT;
}

// FIXED: Removed the invalid constructor initializers and return handlers [INDEX]
Thruster_Controller::Thruster_Controller() noexcept : tetherCompromised(false) {
	Thruster_Controller_Hull_Check();
}

void Thruster_Controller::Thruster_Controller_Hull_Check() noexcept {}

void Thruster_Controller::Tether_TensionCheck(bool criticalTetherFlag) noexcept {
	std::lock_guard<std::mutex> lock(controllerMutex);
	if (criticalTetherFlag) {
		tetherCompromised = true;
		currentMode = propulsionMode::DIVING_HALT; // FIXED: Assigned directly to class variable [INDEX]
	}
}

// FIXED: Signature strictly returns data type 'void' to align with the header file definition [INDEX]
void Thruster_Controller::Thruster_PowerRamping(float deltaTime) {
	Vector3 desiredOutput = Thruster_CURRENT_OUTPUT;
	Vector3 rampedOutput;
	float maxDelta = MAXRAMP * deltaTime;

	// X Axis (Surge) Ramping Math
	float deltaX = desiredOutput.x - Thruster_PAST_OUTPUT.x;
	if (std::abs(deltaX) > maxDelta) {
		rampedOutput.x = Thruster_PAST_OUTPUT.x + (deltaX > 0 ? maxDelta : -maxDelta);
	}
	else {
		rampedOutput.x = desiredOutput.x;
	}

	// Y Axis (Sway) Ramping Math
	float deltaY = desiredOutput.y - Thruster_PAST_OUTPUT.y;
	if (std::abs(deltaY) > maxDelta) {
		rampedOutput.y = Thruster_PAST_OUTPUT.y + (deltaY > 0 ? maxDelta : -maxDelta);
	}
	else {
		rampedOutput.y = desiredOutput.y;
	}

	// Z Axis (Heave) Ramping Math
	float deltaZ = desiredOutput.z - Thruster_PAST_OUTPUT.z;
	if (std::abs(deltaZ) > maxDelta) {
		rampedOutput.z = Thruster_PAST_OUTPUT.z + (deltaZ > 0 ? maxDelta : -maxDelta);
	}
	else {
		rampedOutput.z = desiredOutput.z;
	}

	Thruster_PAST_OUTPUT = rampedOutput;
	Thruster_CURRENT_OUTPUT = rampedOutput; // Lock the ramped outputs straight down into our engine register
}

void Thruster_Controller::THRUSTER_CONTROL_PIPELINE(thrusterChannel channel, propulsionMode mode, float Thruster_TOTALPOWER, bool Thruster_Pilot_OVERRIDE) noexcept {
	std::lock_guard<std::mutex> lock(controllerMutex);
	currentMode = mode;
	is_MANUAL_OVVERIDE_ACTIVE = Thruster_Pilot_OVERRIDE;

	unsigned long currentClockTime = getTheTimeinMillis();

		if (mode == propulsionMode::MANUAL_OVERRIDE && Thruster_Pilot_OVERRIDE) {
			last_heart_beat_time = currentClockTime;
		}

		if (last_heart_beat_time != 0 && (currentClockTime - last_heart_beat_time) > 500) {
			std::cout << "[ERROR] Manual override heartbeat lost! Switching to DIVING_HALT mode for safety.\n";
			currentMode = propulsionMode::DIVING_HALT;
		}
	
	// FIXED: Remapped nested channel assignment arrays cleanly [INDEX]
	switch (channel) {
	case thrusterChannel::THRUSTERONE:   motor_frames[0].channel = channel; break;
	case thrusterChannel::THRUSTERTWO:   motor_frames[1].channel = channel; break;
	case thrusterChannel::THRUSTERTHREE: motor_frames[2].channel = channel; break;
	case thrusterChannel::THRUSTERFOUR:  motor_frames[3].channel = channel; break;
	case thrusterChannel::THRUSTERFIVE:  motor_frames[4].channel = channel; break;
	case thrusterChannel::THRUSTERSIX:   motor_frames[5].channel = channel; break;
	default: break;
	}

	if (currentMode == propulsionMode::MANUAL_OVERRIDE && is_MANUAL_OVVERIDE_ACTIVE) {
		Thruster_CURRENT_OUTPUT = Thruster_Pilot_HEADING * Thruster_TOTALPOWER;
	}
	else if (currentMode == propulsionMode::AUTONOMOUS_OVVERIDE) {
		Thruster_CURRENT_OUTPUT = Vector3(0.0f, 0.0f, Thruster_TOTALPOWER);
	}
	else if (currentMode == propulsionMode::FORCE_ASCENT) {
		Thruster_CURRENT_OUTPUT = Vector3(0.0f, 0.0f, MAXTHRUST);
	}
	else if (currentMode == propulsionMode::DIVING_HALT) {
		Thruster_CURRENT_OUTPUT = Vector3(0.0f, 0.0f, 0.0f);
	}

	// FIXED: Running your newly repaired power ramping algorithm smoothly [INDEX]
	Thruster_PowerRamping(0.1f);

	Thruster_CURRENT_OUTPUT.x = std::clamp(Thruster_CURRENT_OUTPUT.x, MINTHRUST, MAXTHRUST);
	Thruster_CURRENT_OUTPUT.y = std::clamp(Thruster_CURRENT_OUTPUT.y, MINTHRUST, MAXTHRUST);
	Thruster_CURRENT_OUTPUT.z = std::clamp(Thruster_CURRENT_OUTPUT.z, MINTHRUST, MAXTHRUST);
}
