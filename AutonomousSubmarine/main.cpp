#include "../OpticalSensorFirmware/SonarSensorHub.h"
#include "../ObjectLogic/GODS_EYE.h"
#include "../WaypointSystemHUAC/WaypointSystemHUAC.h"
#include "../ObjectLogic/ThrusterController.h"
#include <iostream>
#include <thread>
#include <chrono>

unsigned long fetchMainClockTime() {
	static const auto start = std::chrono::steady_clock::now();
	const auto now = std::chrono::steady_clock::now();
	return static_cast<unsigned long>(std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count());
}

void ExecuteAutonomyDecisionEngine(const GODS_EYE& perception, WaypointSystemHUAC& spatialMap, const Vector3& currentSubPos) {
	constexpr float COLLISION_THRESHOLD = 0.50f;


	float clearanceLeft = perception.GetPerimeterClearance(SonarChannel::Stationary_Left);
	float clearanceFront = perception.GetPerimeterClearance(SonarChannel::Stationary_Front);
	float clearanceRight = perception.GetPerimeterClearance(SonarChannel::Stationary_Right);


	if (perception.CheckEmergencyAscent()) {
		std::cout << "[FAILSAFE ACTIVATED] HARD HULL LEAK FLAG ENGAGED. OVERRIDING MISSION FOR ASCENT.\n";
		spatialMap.PushEntity(SpatialObjectType::Mission_WAYPOINT, Vector3(currentSubPos.x, currentSubPos.y, 0.0f), "Emergency Surface Goal");
			
	}

	if (clearanceFront < COLLISION_THRESHOLD) {
		std::cout << "[WARNING] COLLISION THREAT AHEAD\n" "Clearance:" << clearanceFront << "m. RECALCULATING ROUTES...\n";
	
		Vector3 hazardEstimation = currentSubPos + Vector3(0.0f, clearanceFront, 0.0f);
		spatialMap.PushEntity(SpatialObjectType::Dynamic_OBSTACLE, hazardEstimation, "Front Perimeter Obstacle Wall", 0.50f);

		if (clearanceLeft > clearanceRight) {
			std::cout << "[BRAIN] SWERVING LEFT (Clearance:" << clearanceLeft << "m). RECALCULATING POSITION...\n";
		}
		else {
			std::cout << "[BRAIN] SWERVING RIGHT (Clearance:" << clearanceRight << "m). RECALCULATING POSITION...\n";
			}
		}
	else {
		std::cout << "[STATUS] FRONT PATH CLEAR (" << clearanceFront << "m) CONTINUING NAVIGATION...\n";

		
	
	}
}

int main() {

	SonarSensorHub hardwareHub;
	GODS_EYE perceptionEngine;
	WaypointSystemHUAC spatialMAP;
	Thruster_Controller Thruster_System;

	std::cout << "=======================================================================================\n";
	std::cout << "                          PREPARING AUTONOMOUS ROBOTICS                                \n";
	std::cout << "=======================================================================================\n";

	hardwareHub.initialization();

	Vector3 PlacementSubPosition{ 0.0f, 0.0f, -1.0f };

	unsigned long timeOfLastTick = fetchMainClockTime(); // Track baseline frame history for power ramping physics

	spatialMAP.PushEntity(SpatialObjectType::Mission_WAYPOINT, Vector3{15.0f, 0.0f, -1.0f}, "Target Destination Marker");
	for (int executionCycle = 1; executionCycle <= 10; ++executionCycle) {
		std::cout << "SUBMARINE CURRENT TICK" << executionCycle << "\n";

		unsigned long timeOfCurrentTick = fetchMainClockTime();
		float deltaTime = static_cast<float>(timeOfCurrentTick - timeOfLastTick) / 1000.0f; // Convert ms to seconds)
		if (deltaTime <= 0.0f) deltaTime = 0.016f; // Prevent division by zero, assume ~60Hz if no time has passed
		timeOfLastTick = timeOfCurrentTick;

		hardwareHub.update();

			perceptionEngine.IngestTelemetry(SonarChannel::Stationary_Right, hardwareHub.recent_distance[2]);
			perceptionEngine.IngestTelemetry(SonarChannel::Stationary_Front, hardwareHub.recent_distance[1]);
			perceptionEngine.IngestTelemetry(SonarChannel::Stationary_Left, hardwareHub.recent_distance[0]);

			float simulatedPanServoAngle = -20.0f + (executionCycle * 4.0f);
			perceptionEngine.IngestTelemetry(SonarChannel::GIMBAL_MSS, 2.4f, simulatedPanServoAngle, 0.0f);

			bool mavLinkSignal_LEAKS = (executionCycle == 8);
			perceptionEngine.ExecutePerceptionPipeline(mavLinkSignal_LEAKS);

			bool simulatedTetherTensionSensor = false;
			if (PlacementSubPosition.z < -4.0f) { // checks if sub sinks past safe layout margins and flags it.
				simulatedTetherTensionSensor = true;
			}
			Thruster_System.Tether_TensionCheck(simulatedTetherTensionSensor);

			propulsionMode activeTargetMode = propulsionMode::AUTONOMOUS_OVVERIDE;
			if (perceptionEngine.CheckEmergencyAscent()) {
				activeTargetMode = propulsionMode::FORCE_ASCENT;
			}

			float simulatedTargetPowerValue = 120.0f;
			Thruster_System.THRUSTER_CONTROL_PIPELINE(thrusterChannel::THRUSTERONE, activeTargetMode, simulatedTargetPowerValue, false);

			if ((executionCycle % 3) == 0) {
				std::cout << "Logging Submarine's periodic breadcrumbs...\n";
			}

			PlacementSubPosition.x += 1.5f;
			spatialMAP.PushEntity(SpatialObjectType::PATH_BREADCRUMB, PlacementSubPosition, "Breadrumb Trail Marker");
			std::cout << "[LOG] Submarine position logged at (" << PlacementSubPosition.x << "," << PlacementSubPosition.y << "," << PlacementSubPosition.z << ")\n";

			ExecuteAutonomyDecisionEngine(perceptionEngine, spatialMAP, PlacementSubPosition);

			std::this_thread::sleep_for(std::chrono::milliseconds(60));



	}
	
	std::cout << "[LOG] MISSION LOOP COMPLETE...\n";

		return 0;
}
 

