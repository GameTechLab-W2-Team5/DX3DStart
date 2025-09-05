#pragma once
#include "stdafx.h"
#include "UApplication.h"
#include "EngineApplication.h"
#include "UMeshFactory.h"
// Simple application that inherits from UApplication
void EngineApplication::Update(float deltaTime)
{
    // 기존 업데이트
    UApplication::Update(deltaTime);

    if (GetInputManager().IsKeyPressed(VK_ESCAPE))
        RequestExit();

    static float x;
    x += deltaTime;


    // 상수 버퍼 업데이트 (scale, rotation, location)
    sphere->UpdateConstantBuffer(GetRenderer());
}


void EngineApplication::Render() 
{
    // Basic rendering - nothing for now
    // 3D objects would be rendered here
    // 구 그리기
    if (sphere)
    {
        sphere->Draw(GetRenderer());
    }
}

bool EngineApplication::OnInitialize()
{
    // 카메라를 더 멀리 이동
    camera = new UCamera(
        { 0.0f, 0.0f, -1.0f },       // 카메라 위치
        FQuaternion::FromEuler({ 0,0,0 }), // 회전: 바라보는 방향 초기화
        45.0f,                       // FOV
        (float)windowWidth / (float)windowHeight,
        0.1f, 100.0f               // Near / Far plane
        );

    GetRenderer().SetActiveCamera(camera);

    // Factory에서 공유 Mesh 생성
    UMesh* sharedSphereMesh = UMeshFactory::CreateSphereMesh(GetRenderer());

    // Sphere 인스턴스를 더 작은 크기로 생성
    sphere = new USphere(
        { 0.0f, 0.0f, 0.0f }, // 위치: 월드 중심
        { 0.5f, 0.5f, 0.5f }, // 스케일: 반으로 줄임
        sharedSphereMesh       // Mesh 참조
        );


    return true;
}