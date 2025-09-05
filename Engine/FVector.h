#pragma once


// Structure for a 3D vector
struct FVector
{
	float x, y, z;
	FVector(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}

	FVector operator+(const FVector& other) const
	{
		return { x + other.x, y + other.y, z + other.z };
	}

	FVector operator-(const FVector& other) const
	{
		return { x - other.x, y - other.y, z - other.z };
	}

	FVector operator*(float scalar) const
	{
		return { x * scalar, y * scalar, z * scalar };
	}

	FVector operator/ (float scalar) const
	{
		return { x / scalar, y / scalar, z / scalar };
	}

	float dot(const FVector& other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}

	float length() const
	{
		return sqrtf(dot(*this));
	}

	FVector normalized() const
	{
		float len = length();
		return len > 0 ? (*this) * (1.0f / len) : FVector{ 0, 0, 0 };
	}
};

