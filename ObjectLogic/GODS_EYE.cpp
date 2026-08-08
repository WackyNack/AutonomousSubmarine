#include "GODS_EYE.h"
#include <cmath>
#include <algorithm>

GODS_EYE::GODS_EYE() noexcept : hardwareHullCompromised(false) {}

// FIX: Remove default arguments here (C++ only allows them in the header declaration!)
void GODS_EYE::IngestTelemetry(SonarChannel channel, float range, float pan, float tilt) noexcept {
	PreceptionFrame frame{ channel, range, pan, tilt };
	pipelineBuffer.push_back(frame);
} // FIXED: Closed the curly brace properly here!

// FIX: Seperate and correctly scope function
void GODS_EYE::ExecutePerceptionPipeline(bool criticalLeakFlag) noexcept {
	if (criticalLeakFlag) {
		hardwareHullCompromised = true;
		return;
	}

	IsolatePerimeterThresholds();
	Compute3DCoordinateProjections();
	pipelineBuffer.clear();
}

// FIX: Move outside of IngestTelemetry scope
void GODS_EYE::IsolatePerimeterThresholds() noexcept {
	for (const auto& frame : pipelineBuffer) {
		switch (frame.channel) {
		case SonarChannel::Stationary_Left:  leftRangeCache = frame.rangeMeters;  break;
		case SonarChannel::Stationary_Front: frontRangeCache = frame.rangeMeters; break;
		case SonarChannel::Stationary_Right: rightRangeCache = frame.rangeMeters; break;
		default: break;
		}
	}
}

void GODS_EYE::Compute3DCoordinateProjections() noexcept {
	proccessedPointCloud.clear();
	constexpr float degToRad = 3.14159265f / 180.0f; // FIX: Add missing semicolon

	for (const auto& frame : pipelineBuffer) {
		if (frame.channel == SonarChannel::GIMBAL_MSS && frame.rangeMeters > 0.1f) {
			const float radPan = frame.panDegrees * degToRad;
			const float radTilt = frame.tiltDegrees * degToRad; // FIX: 'flaot' to 'float', corrected spelling to match header

			const float cx = frame.rangeMeters * std::sin(radPan) * std::cos(radTilt);
			const float cy = frame.rangeMeters * std::cos(radPan) * std::cos(radTilt); // FIXED: 'cost' to 'cos'
			const float cz = frame.rangeMeters * std::sin(radTilt); // FIX: replace bad assignment with value assignment

			proccessedPointCloud.push_back(Vector3(cx, cy, cz)); // FIX: use matching variables
		}
	}
}

float GODS_EYE::GetPerimeterClearance(SonarChannel channel) const noexcept {
	if (channel == SonarChannel::Stationary_Left) return leftRangeCache;
	if (channel == SonarChannel::Stationary_Front) return frontRangeCache; // FIX: tracking index parameter
	if (channel == SonarChannel::Stationary_Right) return rightRangeCache;
	return 99.0f;
}

std::vector<Vector3> GODS_EYE::ExtractMSSPointCloud() const noexcept {
	return proccessedPointCloud; // RETURN: vector instance
}

//Note: I used  Claude to help figure out my bugs