#pragma once
#include "stdafx.h"
#include "URenderer.h"
#include "UInputManager.h"
#include "UGUI.h"
#include "UTimeManager.h"

class UApplication
{
private:
	// Core systems
	HWND hWnd;
	URenderer renderer;
	UInputManager inputManager;
	UGUI gui;
	UTimeManager timeManager;

	// Application state
	bool bIsRunning;
	bool bIsInitialized;

	// Window properties
	static WCHAR WindowClass[];
	static WCHAR DefaultTitle[];

public:
	UApplication();
	virtual ~UApplication();

	// Application lifecycle
	bool Initialize(HINSTANCE hInstance, const std::wstring& title = L"Engine Application",
		int width = 1024, int height = 768);
	void Run();
	void Shutdown();

	// Core update loop - can be overridden by derived classes
	virtual void Update(float deltaTime);
	virtual void Render();

	// System access
	URenderer& GetRenderer() { return renderer; }
	UInputManager& GetInputManager() { return inputManager; }
	UGUI& GetGUI() { return gui; }
	UTimeManager& GetTimeManager() { return timeManager; }

	// Window management
	HWND GetWindowHandle() const { return hWnd; }

	// Application state
	bool IsRunning() const { return bIsRunning; }
	void RequestExit() { bIsRunning = false; }
	float GetAspectRatio(){ return windowWidth / windowHeight; }

protected:

	std::wstring windowTitle;
	int windowWidth;
	int windowHeight;

	// Internal methods
	bool CreateMainWindow(HINSTANCE hInstance);
	void ProcessMessages();
	void InternalUpdate();
	void InternalRender();

	// Window procedure
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	// Override points for derived classes
	virtual bool OnInitialize() { return true; }
	virtual void OnShutdown() {}
};