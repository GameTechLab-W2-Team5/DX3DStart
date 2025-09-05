#pragma once
#include <math.h>

struct FVertexSimpleOld
{
    float x, y, z;       // Position (3��)
    float r, g, b, a;    // Color (4��)
};

//���� ����ü ����, �ﰢ�� �迭 ����
// 1. Define the triangle vertices
struct FVertexSimple
{
    float x, y, z, w;       // Position (float4) - w �߰�
    float r, g, b, a;       // Color (float4)


    // ��ȯ �Լ�
    static std::vector<FVertexSimple> ConvertVertexData(const FVertexSimpleOld* oldVertices, int count)
    {
        std::vector<FVertexSimple> newVertices;
        newVertices.reserve(count);

        for (int i = 0; i < count; i++)
        {
            FVertexSimple newVertex;

            // Position: w = 1.0f �߰�
            newVertex.x = oldVertices[i].x;
            newVertex.y = oldVertices[i].y;
            newVertex.z = oldVertices[i].z;
            newVertex.w = 1.0f;  // ������ǥ w ������Ʈ

            // Color: �״�� ����
            newVertex.r = oldVertices[i].r;
            newVertex.g = oldVertices[i].g;
            newVertex.b = oldVertices[i].b;
            newVertex.a = oldVertices[i].a;

            newVertices.push_back(newVertex);
        }

        return newVertices;
    }

};

// Structure for a 3D vector
struct FVector
{
	float x, y, z;
	FVector(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}

    FVector operator+(const FVector& other) const {
        return { x + other.x, y + other.y, z + other.z };
    }

    FVector operator-(const FVector& other) const {
        return { x - other.x, y - other.y, z - other.z };
    }

    FVector operator*(float scalar) const {
        return { x * scalar, y * scalar, z * scalar };
    }

    FVector operator/ (float scalar) const {
        return { x / scalar, y / scalar, z / scalar };
    }
    
    float dot(const FVector& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    float length() const {
        return sqrtf(dot(*this));
    }

    FVector normalized() const {
        float len = length();
        return len > 0 ? (*this) * (1.0f / len) : FVector{ 0, 0, 0 };
    }
};

//���⿡ �߰��ϼ���.
#include "Sphere.h"
