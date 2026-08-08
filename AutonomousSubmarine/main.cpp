#include "../OpticalSensorFirmware/SonarSensorHub.h"
#include "../ObjectLogic/GODS_EYE.h"
#include "../WaypointSystemHUAC/WaypointSystemHUAC.h"
#include <iostream>
#include <thread>
#include <chrono>

void ExectureAutonomyDecisionEngine(const GODS_EYE& perception, WaypointSystemHUAC& spatialMap, const Vector3& currentSubPos) {
	constexpr float COLLISION_THRESHOLD = 0.50f;


	float clearanceLeft = perception.GetPerimeterClearance(SonarChannel::Stationary_Left);
	float clearanceFront = perception.GetPerimeterClearance(SonarChannel::Stationary_Front);
	float clearanceRight = perception.GetPerimeterClearance(SonarChannel::Stationary_Right);


	if (perception.CheckEmergencyAscent()) {
		std::cout << "[FAILSAFE ACTIVATED] HARD HULL LEAK FLAG ENGAGED. OVERRIDING MISSION FOR ASCENT.\n";
		spatialMap.PushEntity(SpatialObjectType::Mission_WAYPOINT, Vector3(currentSubPos.x, currentSubPos.y, 0.0f), "Emergency Surface Goal"
			return;
	}
}