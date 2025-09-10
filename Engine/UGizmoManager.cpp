#include "stdafx.h"
#include "UGizmoManager.h"
#include "UMeshManager.h"
#include "UApplication.h"
#include "UPrimitiveComponent.h"
#include "UGizmoComponent.h"
#include "UGizmoGridComp.h"
#include "URaycastManager.h"
#include "UGizmoArrowComp.h"
#include "UGizmoRotationHandleComp.h"
#include "UGizmoScaleHandleComp.h"
#include "UObject.h"

IMPLEMENT_UCLASS(UGizmoManager, UEngineSubsystem)
UGizmoManager::UGizmoManager()
{
}

UGizmoManager::~UGizmoManager()
{
	char buffer[256];
	sprintf_s(buffer, "UGizmoManager destructor - clearing %zu location, %zu rotation, %zu scale gizmos\n", 
		locationGizmos.size(), rotationGizmos.size(), scaleGizmos.size());
	OutputDebugStringA(buffer);
	
	gridPrimitive.reset();
	locationGizmos.clear();
	rotationGizmos.clear();
	scaleGizmos.clear();
	
	OutputDebugStringA("UGizmoManager destructor completed\n");
}

bool UGizmoManager::Initialize(UMeshManager* meshManager)
{
	// --- 1. 그리드 생성 ---
	// 그리드는 항상 원점에 고정
	gridPrimitive = MakeUnique<UGizmoGridComp>();

	// =================================================

	TUniquePtr<UGizmoArrowComp> arrowX = MakeUnique<UGizmoArrowComp>();
	arrowX->Axis = EAxis::X;
	TUniquePtr<UGizmoArrowComp> arrowY = MakeUnique<UGizmoArrowComp>();
	arrowY->Axis = EAxis::Y;
	TUniquePtr<UGizmoArrowComp> arrowZ = MakeUnique<UGizmoArrowComp>();
	arrowZ->Axis = EAxis::Z;

	arrowX->SetColor({ 1, 0, 0, 1 });
	arrowY->SetColor({ 0, 1, 0, 1 });
	arrowZ->SetColor({ 0, 0, 1, 1 });

	arrowX->SetOriginRotation({ 0.0f, 0.0f, 90.0f });
	arrowY->SetOriginRotation({ 0.0f, 0.0f, 0.0f });
	arrowZ->SetOriginRotation({ -90.0f, 0.0f, 0.0f });

	locationGizmos.push_back(std::move(arrowX));
	locationGizmos.push_back(std::move(arrowZ));
	locationGizmos.push_back(std::move(arrowY));

	// =================================================

	TUniquePtr<UGizmoRotationHandleComp> rotationX = MakeUnique<UGizmoRotationHandleComp>();
	rotationX->Axis = EAxis::X;
	TUniquePtr<UGizmoRotationHandleComp> rotationY = MakeUnique<UGizmoRotationHandleComp>();
	rotationY->Axis = EAxis::Y;
	TUniquePtr<UGizmoRotationHandleComp> rotationZ = MakeUnique<UGizmoRotationHandleComp>();
	rotationZ->Axis = EAxis::Z;

	rotationX->SetColor({ 1, 0, 0, 1 });
	rotationY->SetColor({ 0, 1, 0, 1 });
	rotationZ->SetColor({ 0, 0, 1, 1 });

	rotationX->SetOriginRotation({ 0.0f, 0.0f, 90.0f });
	rotationY->SetOriginRotation({ 0.0f, 0.0f, 0.0f });
	rotationZ->SetOriginRotation({ -90.0f, 0.0f, 0.0f });

	rotationGizmos.push_back(std::move(rotationX));
	rotationGizmos.push_back(std::move(rotationZ));
	rotationGizmos.push_back(std::move(rotationY));

	// =================================================

	TUniquePtr<UGizmoScaleHandleComp> scaleX = MakeUnique<UGizmoScaleHandleComp>();
	scaleX->Axis = EAxis::X;
	TUniquePtr<UGizmoScaleHandleComp> scaleY = MakeUnique<UGizmoScaleHandleComp>();
	scaleY->Axis = EAxis::Y;
	TUniquePtr<UGizmoScaleHandleComp> scaleZ = MakeUnique<UGizmoScaleHandleComp>();
	scaleZ->Axis = EAxis::Z;

	scaleX->SetColor({ 1, 0, 0, 1 });
	scaleY->SetColor({ 0, 1, 0, 1 });
	scaleZ->SetColor({ 0, 0, 1, 1 });

	scaleX->SetOriginRotation({ 0.0f, 0.0f, 90.0f });
	scaleY->SetOriginRotation({ 0.0f, 0.0f, 0.0f });
	scaleZ->SetOriginRotation({ -90.0f, 0.0f, 0.0f });

	scaleGizmos.push_back(std::move(scaleX));
	scaleGizmos.push_back(std::move(scaleZ));
	scaleGizmos.push_back(std::move(scaleY));

	// =================================================

	if (!gridPrimitive->Init(meshManager) || !locationGizmos[0]->Init(meshManager) || !locationGizmos[1]->Init(meshManager) || !locationGizmos[2]->Init(meshManager)
		|| !rotationGizmos[0]->Init(meshManager) || !rotationGizmos[1]->Init(meshManager) || !rotationGizmos[2]->Init(meshManager)
		|| !scaleGizmos[0]->Init(meshManager) || !scaleGizmos[1]->Init(meshManager) || !scaleGizmos[2]->Init(meshManager))
	{
		return false;
	}

	return true;
}

void UGizmoManager::SetTarget(UPrimitiveComponent* target)
{
	targetObject = target;
}

void UGizmoManager::ChangeGizmoSpace()
{
	if (isDragging)
	{
		UE_LOG("Now Dragging Can't Change Space");
		return;
	}

	isWorldSpace = !isWorldSpace;
}

void UGizmoManager::NextTranslation()
{
	if (isDragging)
	{
		UE_LOG("Now Dragging Can't Change Translation");
		return;
	}

	switch (translationType)
	{
	case ETranslationType::Location:
		translationType = ETranslationType::Rotation;
		break;
	case ETranslationType::Rotation:
		translationType = ETranslationType::Scale;
		break;
	case ETranslationType::Scale:
		translationType = ETranslationType::Location;
		break;
	}
}

TArray<UGizmoComponent*> UGizmoManager::GetRaycastableGizmos()
{
	TArray<UGizmoComponent*> result;
	if (targetObject == nullptr)
	{
		return result;
	}

	// 현재 모드에 따라 올바른 기즈모를 그립니다.
	TArray<TUniquePtr<UGizmoComponent>>* currentGizmos = nullptr;

	switch (translationType)
	{
	case ETranslationType::Location:
		currentGizmos = &locationGizmos;
		break;
	case ETranslationType::Rotation:
		currentGizmos = &rotationGizmos;
		break;
	case ETranslationType::Scale:
		currentGizmos = &scaleGizmos;
		break;
	}
	
	if (currentGizmos) {
		for (const auto& gizmo : *currentGizmos) {
			result.push_back(gizmo.get());
		}
	}
	return result;
}

void UGizmoManager::Draw(URenderer& renderer)
{
	// --- 파트 1: 타겟 유무와 상관없이 항상 그리는 요소 ---
	if (gridPrimitive)
	{
		gridPrimitive->Draw(renderer);
	}

	// --- 파트 2: 타겟이 있을 때만 그리는 요소 ---

	if (targetObject == nullptr) return;

	// 현재 모드에 따라 올바른 기즈모를 그립니다.
	TArray<TUniquePtr<UGizmoComponent>>* currentGizmos = nullptr;

	switch (translationType)
	{
	case ETranslationType::Location:
		currentGizmos = &locationGizmos;
		break;
	case ETranslationType::Rotation:
		currentGizmos = &rotationGizmos;
		break;
	case ETranslationType::Scale:
		currentGizmos = &scaleGizmos;
		break;
	}

	if (currentGizmos)
	{
		for (const auto& gizmoPart : *currentGizmos)
		{
			if (gizmoPart)
			{
				// Todo: 이동을 update에서 처리해야 하나??
				gizmoPart->SetPosition(targetObject->GetPosition());

				// Scale 은 항상 local
				if (translationType != ETranslationType::Scale && isWorldSpace)
				{
					gizmoPart->ResetQuaternion();
				}
				else
				{
					gizmoPart->SetQuaternion(targetObject->RelativeQuaternion);
				}
				gizmoPart->DrawOnTop(renderer);
			}
		}
	}
}

//레이와 이동 평면의 3D 교차점 찾기
FVector UGizmoManager::FindCirclePlaneIntersection(const FRay& ray, const FPlane& plane)
{
	float denominator = ray.Direction.Dot(plane.Normal);

	// 레이가 평면과 평행에 가까우면 계산 오류 방지
	if (abs(denominator) < 0.0001f) return { 0,0,0 };

	float t = (plane.PointOnPlane - ray.Origin).Dot(plane.Normal) / denominator;
	FVector intersectionPoint = ray.Origin + ray.Direction * t;

	return intersectionPoint;
}

void UGizmoManager::BeginDrag(const FRay& mouseRay, EAxis axis)
{
	isDragging = true;
	selectedAxis = axis;
	dragStartLocation = targetObject->GetPosition();
	dragStartQuaternion = targetObject->GetQuaternion();
	dragStartScale = targetObject->GetScale();

	// 사용자가 선택한 이동 축 벡터 (예: X축이면 (1,0,0))
	FVector axisDir = GetAxisVector(selectedAxis);

	if (translationType == ETranslationType::Location)
	{
		// --- [위치] 이동 평면 생성 ---
		movementPlane.PointOnPlane = dragStartLocation;

		// 로컬 스페이스 모드일 경우, 축 벡터를 오브젝트의 회전만큼 회전시킵니다.
		if (!isWorldSpace)
		{
			axisDir = targetObject->GetQuaternion().Rotate(axisDir);
		}
	}
	else if (translationType == ETranslationType::Scale)
	{
		// --- [스케일] 이동 평면 생성 ---
		movementPlane.PointOnPlane = dragStartLocation;

		// 스케일은 항상 로컬 축을 기준으로 하므로, isWorldSpace 플래그와 상관없이 축을 회전시킵니다.
		axisDir = targetObject->GetQuaternion().Rotate(axisDir);
	}
	else // ETranslationType::Rotation
	{
		// 1. 회전이 일어날 평면을 정의합니다. 법선 벡터가 곧 회전 축입니다.
		movementPlane.PointOnPlane = dragStartLocation;
		movementPlane.Normal = GetAxisVector(selectedAxis);

		FVector intersectionPoint = FindCirclePlaneIntersection(mouseRay, movementPlane);
		FVector startToIntersectionVec = intersectionPoint - dragStartLocation;
		float projectedLength = startToIntersectionVec.Dot(GetRotationVector(selectedAxis));
		FVector newPosition = dragStartLocation + GetRotationVector(selectedAxis) * projectedLength;

		dragOffset = dragStartLocation - newPosition;

		dragRotationStartVector = movementPlane.Normal.Cross(intersectionPoint - dragStartLocation);

		if (!isWorldSpace) // 로컬 스페이스 회전일 경우, 축도 함께 회전시켜 줍니다.
		{
			movementPlane.Normal = targetObject->GetQuaternion().Rotate(movementPlane.Normal);
		}

		return; // 회전 로직은 여기서 종료
	}

	// --- 위치 및 스케일 공통 로직 ---
	// 카메라 위치에서 객체를 향하는 벡터 (시선 벡터)
	FVector camToObjectDir = (dragStartLocation - mouseRay.Origin).Normalized();

	// 이동 축과 시선 벡터에 동시에 수직인 벡터를 찾고,
	// 다시 외적하여 평면의 법선 벡터를 계산
	FVector tempVec = axisDir.Cross(camToObjectDir);
	movementPlane.Normal = axisDir.Cross(tempVec).Normalized();

	// 선택한 곳을 offset 으로 저장해서 드래그 시 중심점으로 이동하지 않게 하기
	FVector intersectionPoint = FindCirclePlaneIntersection(mouseRay, movementPlane);
	FVector startToIntersectionVec = intersectionPoint - dragStartLocation;
	float projectedLength = startToIntersectionVec.Dot(axisDir);
	FVector newPosition = dragStartLocation + axisDir * projectedLength;

	dragOffset = dragStartLocation - newPosition;
}

void UGizmoManager::UpdateDrag(const FRay& mouseRay)
{
	if (!isDragging) return;

	// --- 1. 마우스 레이와 이동 평면의 3D 교차점 찾기 ---
	FVector intersectionPoint = FindCirclePlaneIntersection(mouseRay, movementPlane);
	FVector startToIntersectionVec = intersectionPoint - dragStartLocation;
	FVector axisDir = GetAxisVector(selectedAxis);

	if (translationType == ETranslationType::Location)
	{
		// --- [위치] 업데이트 로직 ---
		// 로컬 스페이스 모드일 경우, 축 벡터를 오브젝트의 회전만큼 회전시킵니다.
		if (!isWorldSpace)
		{
			axisDir = targetObject->GetQuaternion().Rotate(axisDir);
		}

		float projectedLength = startToIntersectionVec.Dot(axisDir);
		FVector newPosition = dragStartLocation + axisDir * projectedLength;

		targetObject->SetPosition(newPosition + dragOffset);
	}
	else if (translationType == ETranslationType::Scale)
	{
		// --- [스케일] 업데이트 로직 ---
		// 스케일은 항상 로컬 축을 기준으로 하므로, 축을 회전시킵니다.
		axisDir = targetObject->GetQuaternion().Rotate(axisDir);

		// 드래그한 거리를 계산합니다.
		float projectedLength = startToIntersectionVec.Dot(axisDir);

		// 드래그 시작 시의 오프셋을 반영한 순수 이동량을 계산합니다.
		float offsetLength = dragOffset.Dot(axisDir);
		float finalDragAmount = projectedLength + offsetLength;

		// 너무 급격하게 변하지 않도록 민감도를 조절할 수 있습니다.
		float scaleSensitivity = 0.1f;
		finalDragAmount *= scaleSensitivity;

		FVector newScale = dragStartScale;
		switch (selectedAxis)
		{
		case EAxis::X:
			newScale.X += finalDragAmount;
			break;
		case EAxis::Y:
			newScale.Y += finalDragAmount;
			break;
		case EAxis::Z:
			newScale.Z += finalDragAmount;
			break;
		}

		// 스케일 값이 0 이하로 내려가지 않도록 방지합니다.
		newScale.X = max(0.01f, newScale.X);
		newScale.Y = max(0.01f, newScale.Y);
		newScale.Z = max(0.01f, newScale.Z);

		targetObject->SetScale(newScale);
	}
	else // ETranslationType::Rotation
	{
		// --- 1. 마우스 레이와 이동 평면의 3D 교차점 찾기 ---
		FVector intersectionPoint = FindCirclePlaneIntersection(mouseRay, movementPlane);

		// --- 2. 3D 교차점을 이동 축으로 투영하기 ---
		FVector startToIntersectionVec = intersectionPoint - dragStartLocation;
		FVector axisDir = dragRotationStartVector;
		float projectedLength = startToIntersectionVec.Dot(axisDir);
		float angle = projectedLength;

		// 4. 회전 축과 각도로 새로운 회전 쿼터니언을 생성합니다.
		FQuaternion deltaRotation;

		if (isWorldSpace)
		{
			deltaRotation = dragStartQuaternion.RotatedWorldAxisAngle(GetAxisVector(selectedAxis), -angle);
		}
		else
		{
			deltaRotation = dragStartQuaternion.RotatedLocalAxisAngle(GetAxisVector(selectedAxis), -angle);
		}

		// 5. 드래그 시작 시의 회전값에 새로운 회전을 곱하여 최종 회전을 계산합니다.
		targetObject->SetQuaternion(deltaRotation);
	}
}

void UGizmoManager::EndDrag()
{
	isDragging = false;
	selectedAxis = EAxis::None;
}

FVector UGizmoManager::GetAxisVector(EAxis axis)
{
	switch (axis)
	{
	case EAxis::X:
		return { 1,0,0 };
	case EAxis::Y:
		return { 0,1,0 };
	case EAxis::Z:
		return { 0,0,1 };
	default:
		return { 0,0,0 };
	}
}

FVector UGizmoManager::GetRotationVector(EAxis axis)
{
	switch (axis)
	{
	case EAxis::X:
		return { 0,0,1 };
	case EAxis::Y:
		return { 1,0,0 };
	case EAxis::Z:
		return { 0,1,0 };
	default:
		return { 0,0,0 };
	}
}