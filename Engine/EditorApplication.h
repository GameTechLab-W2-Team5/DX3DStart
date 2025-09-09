﻿#pragma once
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
	TArray<USceneComponent*> sceneComponents;

	bool bIsGizmoDragging = false;
	USceneComponent* selectedSceneComponent;

	UControlPanel* controlPanel;
	USceneComponentPropertyWindow* propertyWindow;
	
public:
	EditorApplication() = default;
	~EditorApplication()
	{
		delete controlPanel;
		delete propertyWindow;
		controlPanel = nullptr;
		propertyWindow = nullptr;
	}
	UScene* CreateDefaultScene() override;

	void OnSceneChange() override;

protected:
	void Update(float deltaTime) override;
	void Render() override;
	void RenderGUI() override;
	bool OnInitialize() override;
	void OnResize(int width, int height) override;
};