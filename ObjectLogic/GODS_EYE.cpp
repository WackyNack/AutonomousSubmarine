#include "GODS_EYE.h"
#include <cmath>
#include <algorithm>

GODS_EYE::GODS_EYE() noexcept : hardwareHullCompromised(false) {}

// FIX: Remove default arguments here (C++ only allows them in the header declaration!)
void GODS_EYE::IngestTelemetry(SonarChannel channel, float range) noexcept {
	PreceptionFrame frame{ channel, range};
	pipelineBuffer.push_back(frame);
} // FIXED: Closed the curly brace properly here!

// FIX: Seperate and correctly scope function
void GODS_EYE::ExecutePerceptionPipeline(bool criticalLeakFlag) noexcept {
	if (criticalLeakFlag) {
		hardwareHullCompromised = true;
		return;
	}

	IsolatePerimeterThresholds();
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

float GODS_EYE::GetPerimeterClearance(SonarChannel channel) const noexcept {
	if (channel == SonarChannel::Stationary_Left) return leftRangeCache;
	if (channel == SonarChannel::Stationary_Front) return frontRangeCache; // FIX: tracking index parameter
	if (channel == SonarChannel::Stationary_Right) return rightRangeCache;
	return 99.0f;
}

//Note: I used  Claude to help figure out my bugs