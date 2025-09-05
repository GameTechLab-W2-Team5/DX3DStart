#pragma once

// Windows ���� (���� ����)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

// C++ ǥ�� ���̺귯��
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <cassert>
#include <cmath>

// DirectX ����
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

// ImGui ���� (DirectX ���Ŀ�)
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

// ���̺귯�� ��ŷ
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

// Engine forward declarations
class UApplication;
class URenderer;
class UInputManager;
class UGUI;
class UTimeManager;

// Common macros
#define SAFE_RELEASE(p) if(p) { p->Release(); p = nullptr; }
#define SAFE_DELETE(p) if(p) { delete p; p = nullptr; }
#define SAFE_DELETE_ARRAY(p) if(p) { delete[] p; p = nullptr; }

// Engine namespace (optional)
namespace Engine
{
    // Common engine types and utilities can go here
}