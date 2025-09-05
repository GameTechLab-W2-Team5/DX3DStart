#pragma once
#include <vector>
#include <math.h>

struct FVertexSimpleOld
{
	float x, y, z;       // Position (3개)
	float r, g, b, a;    // Color (4개)
};

//정점 구조체 선언, 삼각형 배열 정의
// 1. Define the triangle vertices
struct FVertexSimple
{
	float x, y, z, w;       // Position (float4) - w 추가
	float r, g, b, a;       // Color (float4)


	// 변환 함수
	static std::vector<FVertexSimple> ConvertVertexData(const FVertexSimpleOld* oldVertices, int count)
	{
		std::vector<FVertexSimple> newVertices;
		newVertices.reserve(count);

		for (int i = 0; i < count; i++)
		{
			FVertexSimple newVertex;

			// Position: w = 1.0f 추가
			newVertex.x = oldVertices[i].x;
			newVertex.y = oldVertices[i].y;
			newVertex.z = oldVertices[i].z;
			newVertex.w = 1.0f;  // 동차좌표 w 컴포넌트

			// Color: 그대로 복사
			newVertex.r = oldVertices[i].r;
			newVertex.g = oldVertices[i].g;
			newVertex.b = oldVertices[i].b;
			newVertex.a = oldVertices[i].a;

			newVertices.push_back(newVertex);
		}

		return newVertices;
	}

};

//여기에 추가하세요.
#include "Sphere.h"
