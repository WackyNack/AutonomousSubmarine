#pragma once
#include <stdexcept>
#include <cmath>

struct Vector3 {
	float x{ 0.0f };
	float y{ 0.0f };
	float z{ 0.0f };

	constexpr Vector3() noexcept = default;
	constexpr Vector3(float tx, float ty, float tz) noexcept : x(tx), y(ty), z(tz) {}
	
	[[nodiscard]] constexpr Vector3 operator+(const Vector3& rhs) const noexcept { return { x + rhs.x , y + rhs.y , z + rhs.z };}
	[[nodiscard]] constexpr Vector3 operator-(const Vector3& rhs) const noexcept { return { x - rhs.x , y - rhs.y , z - rhs.z }; }
	[[nodiscard]] constexpr Vector3 operator*(float scalar) const noexcept { return { x * scalar , y * scalar , z * scalar }; }

	[[nodiscard]] float Length() const noexcept { return std::sqrt(x * x + y * y + z * z); }
	[[nodiscard]] float DistanceTo(const Vector3& target) const noexcept { return (*this - target).Length(); }

	[[nodiscard]] Vector3 Normalize() const {

		const float len = Length();
		if (len < 1e-6f) return { 0.0f, 0.0f, 0.0f };
		return { x / len, y / len, z / len };
	}
};

// Things to note:
// The "1e-6f" is an epsilon that is meant to keep the code from crashing in case it decides to calulate a distance of 0 (yknow what happens when you do x/0)
// no discard will warn in case a function doesn't have a variable assigned
// constexpr will calcualte vectors and math during compile time
// const noexcept will prevent crashes
// For example, it guarantees adding rhs won't crash og vector coords
// but it allows raw numbers from floating points
// Everything else you should be able to understand pretty easy