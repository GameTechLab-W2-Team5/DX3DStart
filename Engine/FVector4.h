#pragma once


// Structure for a 3D vector
struct FVector4
{
	float x, y, z, w;
	FVector4(float _x = 0, float _y = 0, float _z = 0, float _w = 0) : x(_x), y(_y), z(_z), w(_w) {}

	FVector4 operator+(const FVector4& other) const
	{
		return { x + other.x, y + other.y, z + other.z, w + other.w };
	}

	FVector4 operator-(const FVector4& other) const
	{
		return { x - other.x, y - other.y, z - other.z, w - other.w };
	}

	FVector4 operator*(float scalar) const
	{
		return { x * scalar, y * scalar, z * scalar, w * scalar };
	}

	FVector4 operator/ (float scalar) const
	{
		return { x / scalar, y / scalar, z / scalar, w / scalar };
	}

	float dot(const FVector4& other) const
	{
		return x * other.x + y * other.y + z * other.z + w * other.w;
	}

	float length() const
	{
		return sqrtf(dot(*this));
	}

	FVector4 normalized() const
	{
		float len = length();
		return len > 0 ? (*this) * (1.0f / len) : FVector4{ 0, 0, 0, 0 };
	}
};

