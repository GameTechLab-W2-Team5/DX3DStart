#include "stdafx.h"
#include "UControlPanel.h"
#include "USceneComponent.h"
#include "UCamera.h"
#include "USceneManager.h"
#include "UScene.h"

UControlPanel::UControlPanel(USceneManager* sceneManager)
    : ImGuiWindowWrapper("Jungle Control Panel"), SceneManager(sceneManager)
{
    registeredTypes = USceneComponentFactory::GetRegisteredTypes();
    for (const auto& registeredType : registeredTypes)
    {
        choices.push_back(registeredType.c_str());
    }
}

void UControlPanel::RenderContent()
{
    PrimaryInformationSection();
    ImGui::Separator();
    SpawnPrimitiveSection();
    ImGui::Separator();
    SceneManagementSection();
    ImGui::Separator();
    CameraManagementSection();
}

void UControlPanel::PrimaryInformationSection()
{
    ImGui::Text("Hello Jungle World!");

    float frameRate = ImGui::GetIO().Framerate;
    ImGui::Text("FPS %.0f (%.0f ms)", frameRate, 1000.0f / frameRate);
}

void UControlPanel::SpawnPrimitiveSection()
{
    ImGui::Combo("Primitive", &primitiveChoiceIndex, choices.data(), static_cast<int>(choices.size()));

    int objectCount = SceneManager->GetScene()->GetObjectCount();
    if (ImGui::Button("Spawn"))
    {
        USceneComponent* sceneComponent = USceneComponentFactory::Create(choices[primitiveChoiceIndex]);
        if (sceneComponent != nullptr)
        {
        sceneComponent->SetPosition(FVector(
            -5.0f + static_cast<float>(rand()) / RAND_MAX * 10.0f,
            -5.0f + static_cast<float>(rand()) / RAND_MAX * 10.0f,
            -5.0f + static_cast<float>(rand()) / RAND_MAX * 10.0f
        ));
        sceneComponent->SetScale(FVector(
            0.1f + static_cast<float>(rand()) / RAND_MAX * 0.7f,
            0.1f + static_cast<float>(rand()) / RAND_MAX * 0.7f,
            0.1f + static_cast<float>(rand()) / RAND_MAX * 0.7f
        ));
        sceneComponent->SetRotation(FVector(
            -90.0f + static_cast<float>(rand()) / RAND_MAX * 180.0f,
            -90.0f + static_cast<float>(rand()) / RAND_MAX * 180.0f,
            -90.0f + static_cast<float>(rand()) / RAND_MAX * 180.0f
        ));
        SceneManager->GetScene()->AddObject(sceneComponent);
        }
    }
    ImGui::SameLine();
    ImGui::BeginDisabled();
    ImGui::SameLine();
    ImGui::InputInt("Number of spawned primitives", &objectCount, 0);
    ImGui::EndDisabled();
}

void UControlPanel::SceneManagementSection()
{
    ImGui::InputText("Scene Name", sceneName, sizeof(sceneName));

    if (ImGui::Button("New scene"))
    {
        // TODO : Make New Scene
    }

    if (ImGui::Button("Save scene") && strcmp(sceneName, "") != 0)
    {
        std::filesystem::path _path("./data/");
        std::filesystem::create_directory(_path);
        SceneManager->SaveScene(_path.string() + std::string(sceneName) + ".json");
    }
    if (ImGui::Button("Load scene") && strcmp(sceneName, "") != 0)
    {
        SceneManager->LoadScene("./data/" + std::string(sceneName) + ".json");
    }
}

void UControlPanel::CameraManagementSection()
{
    UCamera* camera = SceneManager->GetScene()->GetCamera();
    bool isOrthogonal = camera->IsOrtho();
    ImGui::Checkbox("Orthogonal", &isOrthogonal);
    if (isOrthogonal)
    {
        // 원하는 직교 크기로 (예시: 월드 단위 10x10)
        camera->SetOrtho(10.0f, 10.0f, camera->GetNearZ(), camera->GetFarZ(), /*leftHanded=*/false);
    }
    else
    {
        camera->SetPerspectiveDegrees(camera->GetFOV(),
            camera->GetAspect(), camera->GetNearZ(), camera->GetFarZ());
    }

    // === FOV (perspective일 때만 활성화) ===
    float fovDeg = camera->GetFOV();
    float tableWidth = ImGui::GetContentRegionAvail().x;
    ImGui::SetNextItemWidth(tableWidth * 0.75f);
    ImGui::BeginDisabled(isOrthogonal);
    if (ImGui::InputFloat("##fov", &fovDeg, 0.0f, 0.0f, "%.3f"))
    {
        camera->SetFOV(fovDeg); // proj 재빌드 내부에서 함
    }
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::Text("FOV");

    // === 위치 ===
    FVector pos = camera->GetLocation();
    float cameraLocation[3] = { pos.X, pos.Y, pos.Z };

    // === 회전(Yaw, Pitch, Roll=0 표기) ===
    FQuaternion q = camera->GetRotation();
    FVector eulerRotation = camera->GetEulerXYZDeg();
    float cameraRotation[3] = {
        eulerRotation.X, eulerRotation.Y, eulerRotation.Z
    };

    bool rotationEdited = false;

    // 나머지는 테이블로
    if (ImGui::BeginTable("EditableCameraTable", 4, ImGuiTableFlags_None))
    {
        // Camera Location 행
        ImGui::TableNextRow();
        for (int i = 0; i < 3; i++)
        {
            ImGui::TableSetColumnIndex(i);
            ImGui::SetNextItemWidth(-1);
            ImGui::InputFloat(("##loc" + std::to_string(i)).c_str(),
                &cameraLocation[i], 0.0f, 0.0f, "%.3f");
        }
        ImGui::TableSetColumnIndex(3);
        ImGui::Text("Camera Location");

        // Camera Rotation 행
        ImGui::TableNextRow();
        

        for (int i = 0; i < 3; i++)
        {
            ImGui::TableSetColumnIndex(i);
            ImGui::SetNextItemWidth(-1);
            if (ImGui::InputFloat(("##rot" + std::to_string(i)).c_str(),
                &cameraRotation[i], 0.0f, 0.0f, "%.3f"))
            {
                // 드래그 중 계속 true가 됨
            }
            if (ImGui::IsItemDeactivatedAfterEdit())
            {
                rotationEdited = true;
            }
        }

        ImGui::TableSetColumnIndex(3);
        ImGui::Text("Camera Rotation");

        ImGui::EndTable();
    }

    // === 변경사항을 카메라에 반영 ===
    // 위치
    camera->SetLocation(FVector(cameraLocation[0], cameraLocation[1], cameraLocation[2]));
    if (rotationEdited)
    {
        camera->SetEulerXYZDeg(cameraRotation[0], cameraRotation[1], cameraRotation[2]);
    }

    // 회전 (roll은 무시)
    //float newYawRad = cameraRotation[0] * 3.14159265f / 180.0f;
    //float newPitchRad = cameraRotation[1] * 3.14159265f / 180.0f;
    //camera->SetYawPitch(newYawRad, newPitchRad);
}
