#pragma once
#include "stdafx.h"
#include "UApplication.h"
#include "SimpleApplication.h"
#include "UMeshFactory.h"
// Simple application that inherits from UApplication
void SimpleApplication::Update(float deltaTime)
{
    // Basic update logic
    UApplication::Update(deltaTime);

    // Exit on ESC key
    if (GetInputManager().IsKeyPressed(VK_ESCAPE))
    {
        RequestExit();
    }

    // �� ȸ�� �ִϸ��̼� (���û���)
    static float rotation = 0.0f;
    rotation += deltaTime * 1.0f; // 1 radian per second

    // �� ��ġ�� �ð��� ���� ����
    sphere->SetPosition({ cos(rotation) * 0.4f, sin(rotation) * 0.4f, 0.0f});
    sphere2->SetPosition({ 0.0f, 0.0f, 0.0f });
}

void SimpleApplication::Render() 
{
    // Basic rendering - nothing for now
    // 3D objects would be rendered here
        // �� �׸���
    if (sphere)
    {
        sphere->Draw(GetRenderer());
    }
    if (sphere2)
    {
        sphere2->Draw(GetRenderer());
    }
}

bool SimpleApplication::OnInitialize()
{
    // Factory���� ���� Mesh ����
    UMesh* sharedSphereMesh = UMeshFactory::CreateSphereMesh(GetRenderer());

    // Sphere �ν��Ͻ� ����
    sphere = new USphere({ 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 0.5f }, sharedSphereMesh);
    sphere2 = new USphere({ 0.3f, 0.3f, 0.3f }, { 0.2f, 0.2f, 0.2f }, sharedSphereMesh);

    return true;
}