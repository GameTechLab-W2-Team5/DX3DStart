#include "stdafx.h"
#include "UApplication.h"
#include "EngineApplication.h"

// Engine entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    // Create application instance
    EngineApplication app;

    // Initialize engine systems
    if (!app.Initialize(hInstance, L"Simple Engine", 1024, 1024))
    {
        MessageBox(nullptr, L"Failed to initialize application", L"Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    // Run main loop
    app.Run();

    // Cleanup handled automatically by destructor
    return 0;
}