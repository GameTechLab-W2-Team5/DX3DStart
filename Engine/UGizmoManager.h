#pragma once
#include "UPrimitiveComponent.h"
#include "UGizmoComponent.h"
#include "URaycastManager.h"
#include "UEngineSubsystem.h"

class UMeshManager; // 전방 선언
class URenderer;

enum class EAxis { None, X, Y, Z };

enum class ETranslationType { Location, Rotation, Scale };

struct FPlane
{
	FVector PointOnPlane;
	FVector Normal;
};

class UGizmoManager : public UEngineSubsystem
{
	DECLARE_UCLASS(UGizmoManager, UEngineSubsystem)
public:
	UGizmoManager();

	~UGizmoManager();

	bool Initialize(UMeshManager* meshManager);

	// 기즈모가 조작할 대상 객체를 설정합니다.
	void SetTarget(UPrimitiveComponent* target);

	// 매 프레임 호출되어 기즈모를 화면에 그립니다.
	void Draw(URenderer& renderer);

	void NextTranslation();

	UPrimitiveComponent* GetTarget() { return targetObject; }
	TArray<UGizmoComponent*> GetRaycastableGizmos();
	void BeginDrag(const FRay& mouseRay, EAxis selectedAxis);
	void UpdateDrag(const FRay& mouseRay);
	void EndDrag();

	void ChangeGizmoSpace();
	bool IsDragging() const { return isDragging; }

private:
	ETranslationType translationType = ETranslationType::Location;

	bool isWorldSpace = true;

	// 드래그 상태 변수
	bool isDragging = false;
	EAxis selectedAxis = EAxis::None;
	FVector dragRotationStartVector;
	FVector dragOffset;

	UMeshManager* meshManager;
	UPrimitiveComponent* targetObject = nullptr; // 현재 선택된 객체

	// 드래그 계산을 위해 저장해두는 정보
	FVector dragStartLocation;    // 드래그 시작 시 Target의 월드 위치
	FQuaternion dragStartQuaternion;
	FVector dragStartScale;
	FPlane movementPlane;         // 계산된 이동 평면

	// 유틸리티 함수
	FVector GetAxisVector(EAxis axis);
	FVector GetRotationVector(EAxis axis);

	// 역할에 따라 프리미티브를 분리해서 저장합니다.
	TUniquePtr<UGizmoComponent> gridPrimitive;              // 월드 그리드
	//TArray<TUniquePtr<UGizmoComponent>> transformGizmoPrimitives; // 이동/회전/크기 기즈모
	TArray<TUniquePtr<UGizmoComponent>> locationGizmos;
	TArray<TUniquePtr<UGizmoComponent>> rotationGizmos;
	TArray<TUniquePtr<UGizmoComponent>> scaleGizmos;

	FVector FindCirclePlaneIntersection(const FRay& mouseRay, const FPlane& plane);
};