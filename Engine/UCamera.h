#pragma once
#include "stdafx.h"
#include "FQuaternion.h"
#include "FVector.h"
#include "FMatrix.h"
class UCamera
{
private:
    FQuaternion rotation;
    FVector location;

    // === Perspective Projection 변수들 ===
    float fov;              // Field of View (도 단위)
    float aspectRatio;      // 화면 비율 (width/height)
    float nearPlane;        // Near clipping plane
    float farPlane;         // Far clipping plane

public:
    UCamera(
        const FVector& pos = { 0, 0, 0 },
        const FQuaternion& rot = FQuaternion::FromEuler({ 0, 0, 0 }),
        float _fov = 45.0f,
        float _aspectRatio = 16.0f / 9.0f,
        float _nearPlane = 0.1f,
        float _farPlane = 1000.0f)
        : location(pos), rotation(rot), fov(_fov), aspectRatio(_aspectRatio),
        nearPlane(_nearPlane), farPlane(_farPlane)
    {
    }

    FMatrix GetViewMatrix() const {
        // Translation 행렬
        FMatrix translation = { {
            {1, 0, 0, -location.x},
            {0, 1, 0, -location.y},
            {0, 0, 1, -location.z},
            {0, 0, 0, 1}
        } };

        // 뷰 행렬 = Rotation * Translation
        return rotation.inverse().ToMatrix() * translation;
    }
    
    FMatrix GetPerspectiveMatrix() const {
        float fovRadians = fov * 3.14159f / 180.0f;
        float tanHalfFov = tanf(fovRadians * 0.5f);
        
        FMatrix proj = {};  // 모든 요소를 0으로 초기화
        proj.M[0][0] = 1.0f / (aspectRatio * tanHalfFov);
        proj.M[1][1] = 1.0f / tanHalfFov;
        proj.M[2][2] = farPlane / (farPlane - nearPlane);
        proj.M[2][3] = 1.0f;
        proj.M[3][2] = -(farPlane * nearPlane) / (farPlane - nearPlane);
        
        return proj;
    }





    // 기본 설정 메서드들
    void SetRotation(const FQuaternion& rot) { rotation = rot; }
    void SetPosition(const FVector& pos) { location = pos; }
    FVector GetPosition() const { return location; }

    // 편의 메서드 (필요한 것만)
    void LookAt(const FVector& target) {
        // 추후 구현 예정
    }
};
