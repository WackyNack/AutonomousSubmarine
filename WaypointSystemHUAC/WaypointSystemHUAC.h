#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <mutex>
#include <memory>
#include <optional>
#include "Vector.h"

enum class SpatialObjectType : uint8_t {

	Mission_WAYPOINT,
	Dynamic_OBSTACLE,
	Sensor_Target_ROI, // region of interest
	PATH_BREADCRUMB //digital marker tracking sub
};

struct SpatialEntity {
	uint32_t uid;
	SpatialObjectType type;
	Vector3 position;
	std::string identifier;
	uint64_t time_stamp_MS;
	float safetyRadius;
	bool isActiveThreat;
};

class WaypointSystemHUAC {
public:
	WaypointSystemHUAC() noexcept = default;
	~WaypointSystemHUAC() = default;

	WaypointSystemHUAC(const WaypointSystemHUAC&) = delete;
	WaypointSystemHUAC& operator=(const WaypointSystemHUAC&) = delete;

	void PushEntity(SpatialObjectType type, const Vector3& position, const std::string& name, float safetyRadius = 0.5f) noexcept;
	void PruneTransientObstacles() noexcept;

	[[nodiscard]] std::vector<SpatialEntity> GetEntitiesByType(SpatialObjectType type) const noexcept;
	[[nodiscard]] std::optional<SpatialEntity> ClosestThreat(const Vector3& subPosition, float evaluationRadius) const noexcept;
	
private:
	mutable std::mutex mapMutex;
	std::vector<SpatialEntity> globalSpatialMap;
	uint32_t internationalUidCounter{ 0 }; //assigns a uid for each unknown object
};
