#pragma once
#include "stdafx.h"
#include "URenderer.h"
#include "UPrimitive.h"
#include "FVertexSimple.h"

class USphere : public UPrimitive
{

public:
	USphere(FVector pos = { 0, 0, 0 }, FVector scl = { 1, 1, 1 }, UMesh* sphereMesh = nullptr)
	{
		location = pos;
		scale = scl;
		mesh = sphereMesh;
		type = "Sphere";
	}

	void UpdatePhysics(float t, bool bUsingGravity, float restitution) override
	{
		// 물리 업데이트 로직 (나중에 구현)
	}

	bool OnCollision(UPrimitive* other, float restitution) override
	{
		// 충돌 처리 (나중에 구현)
		return false;
	}

	void Draw(URenderer& renderer) override
	{
		if (!mesh || !mesh->VertexBuffer)
		{
			return;
		}
		UpdateConstantBuffer(renderer);
		renderer.DrawMesh(mesh);
	}
};