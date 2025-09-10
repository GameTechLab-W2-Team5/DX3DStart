#pragma once
#include "ISerializable.h"
#include "TArray.h"
#include "UMeshManager.h"
#include "USceneManager.h"
#include "json.hpp"
#include "UGizmoManager.h"

class UCamera;
class URaycastManager;

class UScene : public UObject
{
    DECLARE_UCLASS(UScene, UObject)
protected:
    int32 backBufferWidth, backBufferHeight;
    int32 version;
    int32 primitiveCount;
    bool isInitialized;
    TArray<TUniquePtr<USceneComponent>> objects;

    // Reference from outside
    URenderer* renderer;
    UMeshManager* meshManager;
    UInputManager* inputManager;
    //URaycastManager* RaycastManager;
    //UGizmoManager* GizmoManager;

    //UScene owns camera
    TUniquePtr<UCamera> camera;

    virtual void RenderGUI() {}
    virtual void OnShutdown() {
        char buffer[256];
        sprintf_s(buffer, "UScene::OnShutdown - clearing %zu objects\n", objects.size());
        OutputDebugStringA(buffer);
        
        for (size_t i = 0; i < objects.size(); ++i) {
            if (objects[i]) {
                // 안전하게 UUID만 출력 (UClass 정보는 이미 소멸 중일 수 있음)
                sprintf_s(buffer, "Clearing scene object %zu: UUID %u\n", i, objects[i]->UUID);
                OutputDebugStringA(buffer);
            }
        }
        
        objects.clear();
        camera.reset();
        
        OutputDebugStringA("UScene::OnShutdown completed\n");
    }
public:
    UScene();
    virtual ~UScene();
    virtual bool Initialize(URenderer* r, UMeshManager* mm, UInputManager* im = nullptr);

    virtual void Render();
    virtual void Update(float deltaTime);
    virtual bool OnInitialize();

    bool IsInitialized() { return isInitialized; }

    int32 GetObjectCount() { return primitiveCount; }

    static TUniquePtr<UScene> Create(json::JSON data);

    void AddObject(TUniquePtr<USceneComponent> obj);
    void SetVersion(int32 v) { version = v; }

    json::JSON Serialize() const override;

    bool Deserialize(const json::JSON& data) override;
    
    const TArray<TUniquePtr<USceneComponent>>& GetObjects() const { return objects; }
    UCamera* GetCamera() { return camera.get(); }
    URenderer* GetRenderer() { return renderer; }
    UInputManager* GetInputManager() { return inputManager; }
};