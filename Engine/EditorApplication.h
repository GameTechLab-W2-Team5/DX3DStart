#pragma once
#include "stdafx.h"
#include "UApplication.h"
#include "USphereComp.h"
#include "URaycastManager.h"
#include "UControlPanel.h"
#include "USceneComponentPropertyWindow.h"

// Simple application that inherits from UApplication
class EditorApplication : public UApplication
{
private:
	UGizmoManager gizmoManager;
	TArray<TUniquePtr<USceneComponent>> sceneComponents;

	TUniquePtr<USceneComponent> selectedSceneComponent;
	TUniquePtr<UControlPanel> controlPanel;
	TUniquePtr<USceneComponentPropertyWindow> propertyWindow;

public:
	EditorApplication() = default;
	~EditorApplication() = default;

	TUniquePtr<UScene> CreateDefaultScene() override;

	void OnSceneChange() override;

protected:
	void Update(float deltaTime) override;
	void Render() override;
	void RenderGUI() override;
	bool OnInitialize() override;
	void OnResize(int32 width, int32 height) override;
};