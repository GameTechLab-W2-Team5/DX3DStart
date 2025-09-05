#pragma once
#include <cmath>
#include "FVector.h"
#include "FMatrix.h"  

struct FQuaternion
{
    float w, x, y, z;

    FQuaternion(float _w = 0, float _x = 0, float _y = 0, float _z = 0)
        : w(_w), x(_x), y(_y), z(_z) {
    }

    // 덧셈
    FQuaternion operator+(const FQuaternion& other) const
    {
        return { w + other.w, x + other.x, y + other.y, z + other.z };
    }

    // 뺄셈
    FQuaternion operator-(const FQuaternion& other) const
    {
        return { w - other.w, x - other.x, y - other.y, z - other.z };
    }

    // 스칼라 곱
    FQuaternion operator*(float scalar) const
    {
        return { w * scalar, x * scalar, y * scalar, z * scalar };
    }

    // 쿼터니언 곱 (Hamilton product)
    FQuaternion operator*(const FQuaternion& q) const
    {
        return {
            w * q.w - x * q.x - y * q.y - z * q.z,  // new w
            w * q.x + x * q.w + y * q.z - z * q.y,  // new x
            w * q.y - x * q.z + y * q.w + z * q.x,  // new y
            w * q.z + x * q.y - y * q.x + z * q.w   // new z
        };
    }

    // 내적
    float dot(const FQuaternion& other) const
    {
        return w * other.w + x * other.x + y * other.y + z * other.z;
    }

    // 크기
    float length() const
    {
        return std::sqrt(dot(*this));
    }

    // 정규화
    FQuaternion normalized() const
    {
        float len = length();
        if (len == 0) return { 1, 0, 0, 0 }; // 단위 쿼터니언 반환
        return { w / len, x / len, y / len, z / len };
    }

    // 켤레
    FQuaternion conjugate() const
    {
        return { w, -x, -y, -z };
    }

    // 역수 (단위 쿼터니언일 때는 켤레 == 역수)
    FQuaternion inverse() const
    {
        float lenSq = dot(*this);
        if (lenSq == 0) return { 1, 0, 0, 0 };
        return conjugate() * (1.0f / lenSq);
    }


    // EulerAngle -> FQuaternion (pitch, yaw, roll 순서)
    static FQuaternion FromEuler(const FVector& euler)
    {
        float pitch = euler.x;  // X축 회전
        float yaw = euler.y;    // Y축 회전  
        float roll = euler.z;   // Z축 회전

        float cy = cosf(yaw * 0.5f);
        float sy = sinf(yaw * 0.5f);
        float cp = cosf(pitch * 0.5f);
        float sp = sinf(pitch * 0.5f);
        float cr = cosf(roll * 0.5f);
        float sr = sinf(roll * 0.5f);

        FQuaternion q;
        q.w = cr * cp * cy + sr * sp * sy;
        q.x = sr * cp * cy - cr * sp * sy;
        q.y = cr * sp * cy + sr * cp * sy;
        q.z = cr * cp * sy - sr * sp * cy;

        return q;
    }

    // FQuaternion -> EulerAngle (pitch, yaw, roll)
    FVector ToEuler() const
    {
        FVector euler;

        // pitch (X-axis rotation)
        float sinp = 2.0f * (w * x + y * z);
        if (fabsf(sinp) >= 1)
            euler.x = copysignf(3.14159265359f / 2.0f, sinp); // 90도 or -90도
        else
            euler.x = asinf(sinp);

        // yaw (Y-axis rotation)
        float siny_cosp = 2.0f * (w * y - z * x);
        float cosy_cosp = 1.0f - 2.0f * (x * x + y * y);
        euler.y = atan2f(siny_cosp, cosy_cosp);

        // roll (Z-axis rotation)
        float sinr_cosp = 2.0f * (w * z + x * y);
        float cosr_cosp = 1.0f - 2.0f * (y * y + z * z);
        euler.z = atan2f(sinr_cosp, cosr_cosp);

        return euler;
    }

    // Quaternion을 회전 행렬로 변환
    static FMatrix ToMatrix(const FQuaternion& q)
    {
        float x = q.x;
        float y = q.y;
        float z = q.z;
        float w = q.w;

        FMatrix result = { {
            {1 - 2 * (y * y + z * z),    2 * (x * y - w * z),        2 * (x * z + w * y),        0},
            {2 * (x * y + w * z),        1 - 2 * (x * x + z * z),    2 * (y * z - w * x),        0},
            {2 * (x * z - w * y),        2 * (y * z + w * x),        1 - 2 * (x * x + y * y),    0},
            {0,                          0,                          0,                          1}
        } };

        return result;
    }

    // 편의 함수 - 멤버 함수 버전
    FMatrix ToMatrix() const
    {
        return ToMatrix(*this);
    }
};
