#pragma once

#include <vector>
#include <array>
#include <cmath>

static const float c_pi = 3.14159265359f;

using float2 = std::array<float, 2>;
using float3 = std::array<float, 3>;

inline float2 operator * (const float2& A, float B)
{
	float2 ret;
	for (size_t i = 0; i < A.size(); ++i)
		ret[i] = A[i] * B;
	return ret;
}

inline float2 operator / (const float2& A, float B)
{
	float2 ret;
	for (size_t i = 0; i < A.size(); ++i)
		ret[i] = A[i] / B;
	return ret;
}

inline float2 operator - (const float2& A, float B)
{
	float2 ret;
	for (size_t i = 0; i < A.size(); ++i)
		ret[i] = A[i] - B;
	return ret;
}

inline float2 operator + (const float2& A, const float2& B)
{
	float2 ret;
	for (size_t i = 0; i < A.size(); ++i)
		ret[i] = A[i] + B[i];
	return ret;
}

inline float2 operator - (const float2& A, const float2& B)
{
	float2 ret;
	for (size_t i = 0; i < A.size(); ++i)
		ret[i] = A[i] - B[i];
	return ret;
}

inline float Length(const float2& V)
{
	float dist = 0.0f;
	for (size_t i = 0; i < V.size(); ++i)
		dist += V[i] * V[i];
	return std::sqrt(dist);
}

inline float LengthToroidal(const float2& V)
{
	float dist = 0.0f;
	for (size_t i = 0; i < V.size(); ++i)
	{
		float dx = V[i];
		if (dx < 0.0f)
			dx += 1.0f;
		if (dx > 0.5f)
			dx = 1.0f - dx;
		dist += dx * dx;
	}
	return std::sqrt(dist);
}

inline float2 Normalize(const float2& V)
{
	return V / Length(V);
}

inline float Dot(const float2& A, const float2& B)
{
	float ret = 0.0f;
	for (size_t i = 0; i < A.size(); ++i)
		ret += A[i] * B[i];
	return ret;
}

template <typename T>
inline T Clamp(T value, T themin, T themax)
{
	if (value <= themin)
		return themin;
	else if (value >= themax)
		return themax;
	else
		return value;
}

inline float Lerp(float A, float B, float t)
{
	return A * (1.0f - t) + B * t;
}

inline float SmoothStep(float edge0, float edge1, float x)
{
	x = Clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
	return x * x * (3.0f - 2.0f * x);
}
