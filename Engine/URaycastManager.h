#pragma once
#include <optional>
#include "UCamera.h"
#include "UInputManager.h"
#include "UPrimitiveComponent.h"
#include "UPlaneComp.h"
#include "FVertexPosColor.h"
#include "UEngineSubsystem.h"

class FVector;

struct FRay
{
	FVector Origin;
	FVector Direction;
};

class URaycastManager : public UEngineSubsystem
{
	DECLARE_UCLASS(URaycastManager, UEngineSubsystem)
public:
	URaycastManager();
	URaycastManager(URenderer* renderer, UCamera* camera, UInputManager* inputManager);
	~URaycastManager();

	bool Initialize(URenderer* renderer, UInputManager* inputManager)
	{
		if (!renderer || !inputManager) return false;
		Renderer = renderer;
		InputManager = inputManager;
		return true;
	}

	void SetRenderer(URenderer* renderer) { Renderer = renderer; }
	void SetInputManager(UInputManager* inputManager) { InputManager = inputManager; }

	template <typename T>
	bool RayIntersectsMeshes(UCamera* camera, TArray<T*>& components, T*& hitComponent);

	std::optional<FVector> RayIntersectsTriangle(FVector triangleVertices[3]);

	FRay CreateRayFromScreenPosition(UCamera* camera);

private:
	URenderer* Renderer;
	UInputManager* InputManager;

	float MouseX, MouseY;
	FVector RayOrigin, RayDirection;

	FVector GetRaycastOrigin(UCamera* camera);
	FVector GetRaycastDirection(UCamera* camera);

	FVector TransformVertexToWorld(const FVertexPosColor4& vertex, const FMatrix& world);
};
