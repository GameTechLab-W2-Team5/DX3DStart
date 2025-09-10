#pragma once
#include "UEngineSubsystem.h"

class UScene;
class UApplication;

class USceneManager : public UEngineSubsystem
{
    DECLARE_UCLASS(USceneManager, UEngineSubsystem)
private:
    UApplication* application;
    TUniquePtr<UScene> currentScene;
public:
    ~USceneManager() override;
    bool Initialize(UApplication* _application);

    UScene* GetScene();
    void SetScene(TUniquePtr<UScene> scene);

    void RequestExit();
    void LoadScene(const FString& path = "");
    void SaveScene(const FString& path = "");
};

