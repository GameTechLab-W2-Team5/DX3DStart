#pragma once

#include "UMesh.h"
#include "URenderer.h"
#include "UEngineSubsystem.h"

class UMeshManager : public UEngineSubsystem
{
	DECLARE_UCLASS(UMeshManager, UEngineSubsystem)
private:
	std::unordered_map<FString, TSharedPtr<UMesh>> meshes;

	TSharedPtr<UMesh> CreateMeshInternal(const TArray<FVertexPosColor>& vertices,
		D3D_PRIMITIVE_TOPOLOGY primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
public:
	UMeshManager();
	~UMeshManager();

	bool Initialize(URenderer* renderer);
	TSharedPtr<UMesh> RetrieveMesh(FString meshName);
};
