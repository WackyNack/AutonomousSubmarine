#pragma once
#include <vector>
#include <memory>
#include "../WaypointSystemHUAC/Vector.h"

enum class SonarChannel : uint8_t {
	Stationary_Left,
	Stationary_Front,
	Stationary_Right,
	GIMBAL_MSS
};

struct PreceptionFrame {
	SonarChannel channel;
	float rangeMeters;
	float panDegrees;
	float tiltDegrees;
};

class GODS_EYE {
public:
	GODS_EYE() noexcept;

	void IngestTelemetry(SonarChannel channel, float range, float pan = 0.0f, float tilt = 0.0f) noexcept;
	void ExecutePerceptionPipeline(bool criticalLeakFlag) noexcept;

	[[nodiscard]] bool CheckEmergencyAscent() const noexcept { return hardwareHullCompromised; }
	[[nodiscard]] float GetPerimeterClearance(SonarChannel channel) const noexcept;
	[[nodiscard]] std::vector<Vector3> ExtractMSSPointCloud() const noexcept;

private:
	std::vector<PreceptionFrame> pipelineBuffer;
	std::vector<Vector3> proccessedPointCloud;

	float leftRangeCache{ 99.0f };
	float frontRangeCache{ 99.0f };
	float rightRangeCache{ 99.0f };
	bool hardwareHullCompromised{ false };

	void IsolatePerimeterThresholds() noexcept;
	void Compute3DCoordinateProjections() noexcept;
};
