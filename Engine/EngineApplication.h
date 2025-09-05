#pragma once
#include "stdafx.h"
#include "UApplication.h"
#include "UCamera.h"
#include "USphere.h"
// Simple application that inherits from UApplication
class EngineApplication : public UApplication
{
private:
    UCamera* camera;
    USphere* sphere;
    ID3D11Buffer* sphereVertexBuffer;
public:
    EngineApplication() = default;
    ~EngineApplication() = default;

protected:
    void Update(float deltaTime) override;
    void Render() override;
    bool OnInitialize() override;
};