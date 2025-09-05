#pragma once
#include "stdafx.h"
#include "UMesh.h"
#include "FMatrix.h"
#include "FQuaternion.h"
#include "FVector.h"
#include "URenderer.h"

class UPrimitive
{
protected:
	// 상수 버퍼 데이터 구조
	struct ConstantBufferData
	{
		float mvp[4][4];
	};
public:
	virtual void UpdatePhysics(float t, bool bUsingGravity, float restitution) = 0;
	virtual void Draw(URenderer& renderer) = 0;
	virtual bool OnCollision(UPrimitive* other, float restitution) = 0;
	virtual ~UPrimitive() {}

	void UpdateConstantBuffer(URenderer& renderer) 
	{
		ConstantBufferData cbData;

		UCamera* camera = renderer.GetCurrentCamera();

		FMatrix s = { {
			{scale.x,0,0,0},
			{0,scale.y,0,0},
			{0,0,scale.z,0},
			{0,0,0,1}
		} };


		FMatrix r = rotation.ToMatrix();

		FMatrix t = {{
			{1,0,0,location.x},
			{0,1,0,location.y},
			{0,0,1,location.z},
			{0,0,0,1}  // Row-major: translation을 마지막 열에
		}};

		FMatrix model = t * r * s;

		FMatrix view = camera->GetViewMatrix();

		FMatrix projection = camera->GetPerspectiveMatrix();

		FMatrix mvp = model * view * projection;  // MV * P 순서로 테스트

		// Transpose 제거하고 테스트
		for (int i = 0;i < 4;++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				cbData.mvp[i][j] = mvp.M[i][j];
			}
		}

		// 상수 버퍼 업데이트
		renderer.UpdateConstantBuffer(&cbData, sizeof(cbData));
	}


	FVector GetLocation()
	{
		return location;
	}

	void SetLocation(const FVector& loc)
	{
		location = loc;
	}

	FVector GetScale()
	{
		return scale;
	}

	void SetScale(const FVector& scl)
	{
		scale = scl;
	}

	FQuaternion GetRotation()
	{
		return rotation;
	}

	void SetRotation(const FQuaternion& rot)
	{
		rotation = rot;
	}

protected:

	UMesh* mesh;
	FVector location, scale;
	FQuaternion rotation;
	std::string type;

};