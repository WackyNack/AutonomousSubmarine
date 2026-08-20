#pragma once
#include <vector>
#include <memory>
#include "../WaypointSystemHUAC/Vector.h"

enum class SonarChannel : uint8_t {
	Stationary_Left,
	Stationary_Front,
	Stationary_Right,
};

struct PreceptionFrame {
	SonarChannel channel;
	float rangeMeters;
};

class GODS_EYE {
public:
	GODS_EYE() noexcept;

	void IngestTelemetry(SonarChannel channel, float range) noexcept;
	void ExecutePerceptionPipeline(bool criticalLeakFlag) noexcept;

	[[nodiscard]] bool CheckEmergencyAscent() const noexcept { return hardwareHullCompromised; }
	[[nodiscard]] float GetPerimeterClearance(SonarChannel channel) const noexcept;

private:
	std::vector<PreceptionFrame> pipelineBuffer;

	float leftRangeCache{ 99.0f };
	float frontRangeCache{ 99.0f };
	float rightRangeCache{ 99.0f };
	bool hardwareHullCompromised{ false };

	void IsolatePerimeterThresholds() noexcept;
};
