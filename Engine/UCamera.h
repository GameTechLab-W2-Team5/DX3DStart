#pragma once
#include "stdafx.h"
#include "Vector.h"
#include "Matrix.h"
#include "Quaternion.h"
// =====================
// UCamera (RH, row-vector, Z-up)
// =====================


class UCamera
{
public:
    UCamera()
        : mEye(0.0f, 0.0f, 0.0f)
        , mRot(FQuaternion::Identity())
        , mView(FMatrix::IdentityMatrix())
        , mProj(FMatrix::IdentityMatrix())
        , mFovY(ToRad(60.0f)), mAspect(1.0f), mNearZ(0.1f), mFarZ(1000.0f)
        , mUseOrtho(false)
        , mOrthoWidth(10.0f)
        , mOrthoHeight(10.0f)
        , bLockRoll(true)
        , mPitch(0.0f)
    {
        RecalcAxesFromQuat();
        UpdateView();
        UpdateProj();
    }
    // ===== 행렬 Get =====
    const FMatrix& GetView() const { return mView; }
    const FMatrix& GetProj() const { return mProj; }
    float GetAspect() const { return mAspect; }
    float GetNearZ() const { return mNearZ; }
    float GetFarZ()  const { return mFarZ; }
    bool  IsOrtho()  const { return mUseOrtho; }
    const FVector& GetLocation() const { return mEye; }
    const FVector& GetRight()   const { return mRight; }
    const FQuaternion& GetRotation()   const { return mRot; }
    const FVector& GetUp()      const { return mUp; }
    const FVector& GetForward() const { return mForward; }
    void GetBasis(FVector& outRight, FVector& outForward, FVector& outUp) const {
        outRight = GetRight();
        outForward = GetForward();
        outUp = GetUp();
    }
    // ===== 투영 설정 =====
    void SetPerspective(float fovY, float aspect, float zn, float zf) {
        mFovY = fovY; mAspect = aspect; mNearZ = zn; mFarZ = zf;
        mProj = FMatrix::PerspectiveFovRHRow(fovY, aspect, zn, zf);
    }
    void SetPerspectiveDegrees(float fovYDeg, float aspect, float zn, float zf)
    {
        mUseOrtho = false;
        mFovY = ToRad(fovYDeg);
        mAspect = (aspect > 0.0f) ? aspect : 1.0f;
        mNearZ = zn; mFarZ = zf;
        UpdateProj();
    }
    float GetFOV() const { return mFovY * (180.0f / PI); }
    void  SetFOV(float deg) { SetPerspectiveDegrees(deg, mAspect, mNearZ, mFarZ); }
    // ===== 위치/자세 =====
    void SetLocation(const FVector& eye) { mEye = eye; UpdateView(); }
    void SetRotation(const FQuaternion& q) { mRot = q.Normalized(); UpdateView(); }
    void SetLockRoll(bool on) { bLockRoll = on; UpdateView(); }
    // target을 바라보도록 (RH, Z-up)
    // 첨언 : AI 한테 물어보면 틀렸다고 뭐라뭐라 합니다;; (근데 이거 맞음)
    void LookAt(const FVector& eye, const FVector& target, const FVector& up = FVector(0, 0, 1))
    {
        mEye = eye;
        FVector F = (eye - target).GetNormalized();
        FVector Uref = up.GetNormalized();
        // RH 크로스 사용: Right = up × forward, Up = forward × right
        FVector R = F.Cross(Uref).GetNormalized();
        FVector U = R.Cross(F).GetNormalized();
        // 열=축으로 3×3 회전행렬 구성 (col0=R, col1=U, col2=F)
        FMatrix M = FMatrix::IdentityMatrix();
        M.M[0][0] = R.X; M.M[0][1] = R.Y; M.M[0][2] = R.Z;  // col0 = Right
		M.M[1][0] = F.X; M.M[1][1] = F.Y; M.M[1][2] = F.Z;  // col1 = Forward
        M.M[2][0] = U.X; M.M[2][1] = U.Y; M.M[2][2] = U.Z;  // col2 = Up
        // 쿼터니언으로 저장 (행벡터 규약용 FromMatrixRow는 이 3×3을 그대로 기대)
        mRot = FQuaternion::FromMatrixRow(M);
        // (옵션) pitch 추정
        float fz = (F.Z < -1.f ? -1.f : (F.Z > 1.f ? 1.f : F.Z));
        mPitch = asinf(fz);
        // 뷰 갱신
        RecalcAxesFromQuat();
        UpdateView();
    } 
    // 로컬축 기준 이동 (dx=right, dy=forward, dz=up)
    void MoveLocal(float dx, float dy, float dz, float deltaTime, bool boost = false,
        float baseSpeed = 3.0f, float boostMul = 3.0f)
    {
        RecalcAxesFromQuat(); // 안전: 최신 회전 반영
        float speed = baseSpeed * (boost ? boostMul : 1.0f);
        FVector delta = (mRight * dx + mForward * dy + mUp * dz) * (speed * deltaTime);
        mEye = mEye + delta;
        UpdateView();
    }
    // w,h는 뷰 공간 가로/세로 범위(직교 박스 크기)
    void SetOrtho(float w, float h, float zn, float zf, bool leftHanded=false)
    {
        mUseOrtho = true;
        mOrthoWidth = (w > 0.0f) ? w : 1.0f;
        mOrthoHeight = (h > 0.0f) ? h : 1.0f;
        mNearZ = zn; mFarZ = zf;
        UpdateProj(leftHanded);
    }
    void SetAspect(float aspect) { mAspect = (aspect > 0.0f) ? aspect : 1.0f; UpdateProj(); }
    // 회전: yawZ는 "세계 Z축" 기준, pitch는 "카메라 Right(현재)" 기준인데 음수값 넣어야 정상 작동;
    void AddYawPitch(float yawZ, float pitch)
    {
        if (yawZ == 0.f && pitch == 0.f) return;
        FQuaternion qYaw = FQuaternion::FromAxisAngle(FVector(0, 0, 1), yawZ);
        const float kMax = ToRad(89.0f);
        float newPitch = std::clamp(mPitch + pitch, -kMax, kMax);
        float dp = newPitch - mPitch;
        // yaw 후의 Right축을 "계산으로" 구함(축만 돌림)
        FVector rightAfterYaw = qYaw.Rotate(mRight);
        FQuaternion qPitch = (fabsf(dp) > 1e-6f)
            ? FQuaternion::FromAxisAngle(rightAfterYaw, dp)
            : FQuaternion::Identity();
        // 원하는 순서: 먼저 yaw(월드 Z), 그 다음 pitch(새 Right)
        mRot = (qPitch * qYaw * mRot).Normalized();
        mPitch = newPitch;
        RecalcAxesFromQuat();
        UpdateView();
    }
    //// GUI 표시/편집용: Yaw(세계 Z), Pitch(카메라 Right)
    void GetYawPitch(float& yawZ, float& pitch) const {
        // forward로부터 구면좌표 (Z-up)
        FVector f = mForward;
        float clampedZ = (f.Z<-1.f? -1.f: (f.Z>1.f? 1.f: f.Z));
        pitch = asinf(clampedZ);
        yawZ = atan2f(f.Y, f.X);
        if (yawZ > PI) yawZ -= 2.f * PI;
        if (yawZ < -PI) yawZ += 2.f * PI;
    }
    void SetYawPitch(float yawZ, float pitch)
    {
        // 1) Yaw (World Z) : qYaw ⊗ mRot
        if (yawZ != 0.0f) {
            FQuaternion qYaw = FQuaternion::FromAxisAngle(FVector(0, 0, 1), yawZ);
            mRot = (qYaw * mRot).Normalized();
        }

        // 2) Pitch (Local Right) : 현재 Right 축을 월드축으로 잡고 앞에서 곱해도 동일
        if (pitch != 0.0f) {
            const float kMax = ToRad(89.0f);
            float newPitch = mPitch + pitch;
            if (newPitch > kMax) newPitch = kMax;
            if (newPitch < -kMax) newPitch = -kMax;

            float dp = newPitch - mPitch;
            if (fabsf(dp) > 1e-6f) {
                RecalcAxesFromQuat(); // 최신 Right 구하기
                FQuaternion qPitch = FQuaternion::FromAxisAngle(mRight, dp);
                mRot = (qPitch * mRot).Normalized();
                mPitch = newPitch;
            }
        }

        RecalcAxesFromQuat();
        UpdateView();
    }
    FVector GetEulerXYZRad() const {
        return mRot.ToEulerXYZ(); // 앞서 넣은 ToEulerXYZ() 사용 (라디안)
    }
    FVector GetEulerXYZDeg() const {
        FVector r = GetEulerXYZRad();
        const float R2D = 180.0f / PI;
        return FVector(r.X * R2D, r.Y * R2D, r.Z * R2D);
    }
    void SetEulerXYZRad(float rx, float ry, float rz) {
        // 롤 잠금 옵션
        if (bLockRoll) rz = 0.0f;
        mRot = FQuaternion::FromEulerXYZ(rx, ry, rz).Normalized();
        // 축/뷰 갱신
        RecalcAxesFromQuat();
        // pitch(편의용) 재계산: forward.z = sin(pitch)
        float fz = std::clamp(mForward.Z, -1.0f, 1.0f);
        mPitch = asinf(fz);
        UpdateView();
    }
    void SetEulerXYZDeg(float rxDeg, float ryDeg, float rzDeg) {
        const float D2R = PI / 180.0f;
        SetEulerXYZRad(rxDeg * D2R, ryDeg * D2R, rzDeg * D2R);
    }
private:
    // ---- 내부 상태 ----
    FVector mEye;      // 월드 위치
    FQuaternion mRot;  // 자세(orientation)

    // 파생/캐시 축 (뷰/이동에 사용)
    FVector mRight;    // q·(+X)
    FVector mUp;       // q·(+Z)
    FVector mForward;  // q·(-Y)  ← 규약!

    // 투영 파라미터
    float   mFovY;
    float   mAspect;
    float   mNearZ, mFarZ;
    bool    mUseOrtho;
    float   mOrthoWidth, mOrthoHeight;

    // 누적 pitch
    float mPitch;

    // 행렬
    FMatrix mView;
    FMatrix mProj;
    bool  bLockRoll;
    // ---- 유틸 ----
    static inline float ToRad(float d) { return d * (float)(PI / 180.0); }

    // === 쿼터니언 → 축 재계산(규약 고정) ===
    void RecalcAxesFromQuat()
    {
        // 로컬 단위축을 회전
        mRight = mRot.Rotate(FVector(1, 0, 0)).GetNormalized(); // +X
        mForward = mRot.Rotate(FVector(0, 1, 0)).GetNormalized(); // +Y
        mUp = mRot.Rotate(FVector(0, 0, 1)).GetNormalized(); // +Z
    }

    void UpdateProj(bool leftHanded=false)
    {
        if (!mUseOrtho) {
            // RH + row-vector + D3D depth [0,1]
            mProj = FMatrix::PerspectiveFovRHRow(mFovY, mAspect, mNearZ, mFarZ);
        } else {
            // 직교 (요청 시 LH도 지원)
            if (!leftHanded) mProj = FMatrix::OrthoRHRow(mOrthoWidth, mOrthoHeight, mNearZ, mFarZ);
            else             mProj = FMatrix::OrthoLHRow(mOrthoWidth, mOrthoHeight, mNearZ, mFarZ);
        }
    }
    void UpdateView()
    {
        // f = (eye - target) = -forward → forward = -f
        // 여기서 f(Back)는 카메라가 보는 -Z 방향과 대응시키기 위해 사용
        FVector s = mRight;                 // Right
        FVector u = mUp;                    // Up
        FVector f = (-mForward).GetNormalized(); // Back = -Forward
        FMatrix V = FMatrix::IdentityMatrix();
        // 회전 성분: 열에 [s u f] 배치 (row-vector 규약)
        V.M[0][0] = s.X; V.M[0][1] = u.X; V.M[0][2] = f.X;
        V.M[1][0] = s.Y; V.M[1][1] = u.Y; V.M[1][2] = f.Y;
        V.M[2][0] = s.Z; V.M[2][1] = u.Z; V.M[2][2] = f.Z;
        // 평행이동(마지막 행)
        V.M[3][0] = -(mEye.Dot(s));
        V.M[3][1] = -(mEye.Dot(u));
        V.M[3][2] = -(mEye.Dot(f));
        V.M[3][3] = 1.0f;
        mView = V;
    }
};
