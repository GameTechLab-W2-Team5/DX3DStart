#pragma once
#include "ImGuiWindowWrapper.h"
#include "USceneManager.h"
#include "USceneComponent.h"

class UControlPanel : public ImGuiWindowWrapper
{
	USceneManager* SceneManager;
	
	// Spawn Primitive Section
	TArray<UClass*> registeredTypes;
	TArray<std::string> choiceStrList;
	TArray<const char*> choices;
	int32 primitiveChoiceIndex = 0;

	// Scene Management Section
	char sceneName[256] = "";

	// Camera Management Section


public:
	UControlPanel(USceneManager* sceneManager);
	void RenderContent() override;
	void PrimaryInformationSection();
	void SpawnPrimitiveSection();
	void SceneManagementSection();
	void CameraManagementSection();
	TUniquePtr<USceneComponent> CreateSceneComponentFromChoice(int index);
};

