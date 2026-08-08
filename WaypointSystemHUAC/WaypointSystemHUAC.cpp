#include "WaypointSystemHUAC.h"
#include <chrono>
#include <algorithm>

void WaypointSystemHUAC::PushEntity(SpatialObjectType type, const Vector3& position, const std::string& name, float safetyRadius) noexcept {

	std::lock_guard<std::mutex> lock(mapMutex);

	const uint64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

	globalSpatialMap.push_back({
		++internationalUidCounter,
		type,
		position,
		name,
		now,
		safetyRadius,
		(type != SpatialObjectType::Dynamic_OBSTACLE) //force obstacles to expire unless verified
		});
		
}

void WaypointSystemHUAC::PruneTransientObstacles() noexcept {
	std::lock_guard<std::mutex> lock(mapMutex);
	globalSpatialMap.erase(
		std::remove_if(globalSpatialMap.begin(), globalSpatialMap.end(),
			[](const SpatialEntity& entity) {
				return entity.type == SpatialObjectType::Dynamic_OBSTACLE && !entity.isActiveThreat;}),
				globalSpatialMap.end()
				);
}

std::vector<SpatialEntity> WaypointSystemHUAC::GetEntitiesByType(SpatialObjectType type) const noexcept {
	std::lock_guard<std::mutex> lock(mapMutex);
	std::vector<SpatialEntity> filtered;

	for (const auto& entity : globalSpatialMap) {
		if (entity.type == type) {
			filtered.push_back(entity);
		}
		return filtered;
	}
}

std::optional<SpatialEntity> WaypointSystemHUAC::ClosestThreat(const Vector3& subPosition, float evaluationRadius) const noexcept {
	std::lock_guard<std::mutex> lock(mapMutex);
	std::optional<SpatialEntity> closestEntity = std::nullopt;
	float minimumDistance = evaluationRadius;

	for (const auto& entity : globalSpatialMap) {
		if (entity.type == SpatialObjectType::Dynamic_OBSTACLE) {
			const float distance = subPosition.DistanceTo(entity.position);
			if (distance < minimumDistance) {
				minimumDistance = distance;
				closestEntity = entity;
			}
		}
	}
	return closestEntity;
}