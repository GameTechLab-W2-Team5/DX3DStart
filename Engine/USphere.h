#pragma once
#include "stdafx.h"
#include "URenderer.h"
#include "UPrimitive.h"
#include "FVertexSimple.h"

class USphere : public UPrimitive
{
private:
    FVector position;
    FVector scale;

    // ��� ���� ������ ����
    struct ConstantBufferData
    {
        float Offset[4];    // float4�� ���� (w�� 0.0f)
        float Scale[4];     // float4�� ���� (w�� 1.0f)
    };

public:
    USphere(FVector pos = { 0, 0, 0 }, FVector scl = { 1, 1, 1 }, UMesh* sphereMesh = nullptr)
        : position(pos), scale(scl)
    {
        mesh = sphereMesh;
    }

    void UpdatePhysics(float t, bool bUsingGravity, float restitution) override
    {
        // ���� ������Ʈ ���� (���߿� ����)
    }

    bool OnCollision(UPrimitive* other, float restitution) override
    {
        // �浹 ó�� (���߿� ����)
        return false;
    }

    void UpdateConstantBuffer(URenderer& renderer) override
    {
        ConstantBufferData cbData;

        // Offset ���� (float4)
        cbData.Offset[0] = position.x;
        cbData.Offset[1] = position.y;
        cbData.Offset[2] = position.z;
        cbData.Offset[3] = 0.0f;  // w ������Ʈ

        // Scale ���� (float4)
        cbData.Scale[0] = scale.x;
        cbData.Scale[1] = scale.y;
        cbData.Scale[2] = scale.z;
        cbData.Scale[3] = 1.0f;   // w ������Ʈ

        // ��� ���� ������Ʈ
        renderer.UpdateConstantBuffer(&cbData, sizeof(cbData));
    }

    void Draw(URenderer& renderer) override
    {
        if (!mesh || !mesh->VertexBuffer) {
            return;
        }
        // �� ���� �����Ǿ����ϴ�!
        UpdateConstantBuffer(renderer);
        renderer.DrawMesh(mesh);
    }

    // ��ġ�� ������ ���� �Լ���
    void SetPosition(const FVector& pos) { position = pos; }
    void SetScale(const FVector& scl) { scale = scl; }
    FVector GetPosition() const { return position; }
    FVector GetScale() const { return scale; }
};