#include <DynamicOutput/Output.hpp>
#include <Mod/CppUserModBase.hpp>
#include <UE4SSProgram.hpp>
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/UObject.hpp>
#include <Unreal/UAssetRegistry.hpp>
#include <Unreal/UAssetRegistryHelpers.hpp>
#include <Unreal/UClass.hpp>
#include <Unreal/FProperty.hpp>
#include <Unreal/Property/FStructProperty.hpp>
#include <Unreal/Property/FBoolProperty.hpp>

#include "AssetUtils.h"
#include "BPMacros.hpp"
#include "GuiComponent_List.h"
#include "World.hpp"
#include "UClassHelper.hpp"
#include "TypeDefine.h"
#include "UScriptStruct.hpp"

//------------
#define MGetComponentsByClass(OBJ, NAME) \
*(OBJ)->GetComponentsByClass(UClassHelper::Get().GetClass(NAME)).FindByPredicate([](auto item) { return item != nullptr; })

#define MCallFunction_Begin(OBJ, NAME) \
if (true) { \
auto MCallFunction_Name = STR(NAME); \
if (const auto MCallFunction_FUNC = (OBJ)->GetFunctionByName(FName(MCallFunction_Name))) { \
auto MCallFunction_OBJTEMP = OBJ;

#define MCallFunction_Begin_InChain(OBJ, NAME) \
if (true) { \
auto MCallFunction_Name = STR(NAME); \
if (const auto MCallFunction_FUNC = (OBJ)->GetFunctionByNameInChain(FName(MCallFunction_Name))) { \
auto MCallFunction_OBJTEMP = OBJ;

#define MCallFunction_Call \
MCallFunction_OBJTEMP->ProcessEvent(MCallFunction_FUNC, &params);

#define MCallFunction_End } else Output::send<LogLevel::Error>(STR("[MCallFunction] {} 404!\n"), MCallFunction_Name); }
//----------------

namespace MyMods {
    using namespace RC;
    using namespace Unreal;

    /**
    * MyExampleMod: UE4SS c++ mod class defintion
    */
    class MyExampleMod : public RC::CppUserModBase {
    public:
        MyExampleMod() {
            ModVersion = STR("0.1");
            ModName = STR("MyExampleMod");
            ModAuthors = STR("UE4SS");
            ModDescription = STR("A basic template C++ mod");
            // Do not change this unless you want to target a UE4SS version
            // other than the one you're currently building with somehow.
            //ModIntendedSDKVersion = STR("2.6");

            Output::send<LogLevel::Normal>(STR("[MyExampleMod]: Init.\n"));

            UE4SS_ENABLE_IMGUI()

            register_tab(STR("模型烘焙提取"), [](CppUserModBase *instance) {
                const auto mod = dynamic_cast<MyExampleMod *>(instance);
                if (!mod) return;
                // 设置滚动条
                ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 20.0f);
                ImGui::BeginChild("滚动条");
                // 初始化
                ImGui::SeparatorText("初始化");
                if (ImGui::Button("初始化场景")) mod->init_scene();
                ImGui::SameLine();
                if (ImGui::RadioButton("自由模式", &mod->GameMode, 0)) mod->set_game_mode();
                ImGui::SameLine();
                if (ImGui::RadioButton("烘焙模式", &mod->GameMode, 1)) mod->set_game_mode();
                // 场景设置
                ImGui::SeparatorText("场景设置");
                if (ImGui::Combo("窗口模式", &mod->WindowModeOptionsSelectedIndex, mod->WindowModeOptions, IM_ARRAYSIZE(mod->WindowModeOptions)))
                    mod->set_resolution();
                if (ImGui::Combo("窗口分辨率", &mod->ResolutionOptionsSelectedIndex, mod->ResolutionOptions, IM_ARRAYSIZE(mod->ResolutionOptions)))
                    mod->set_resolution();
                ImGui::Text("(重新显示需重载关卡)");
                if (ImGui::Checkbox("隐藏所有Actor", &mod->HideActor)) mod->set_hidden_params();
                ImGui::SameLine();
                if (ImGui::Checkbox("隐藏UI", &mod->HideUI)) mod->set_hidden_params();
                if (ImGui::InputFloat("零点高度", &mod->ZeroHeight, 100, 200)) { mod->set_mesh_params(); mod->set_camera_params(); mod->set_pawn_params(); }
                // 摄像机
                ImGui::SeparatorText("相机");
                if (ImGui::InputFloat("相机高度", &mod->CameraHeight, 100, 200)) mod->set_camera_params();
                ImGui::SameLine();
                if (ImGui::Button("##重置")) mod->CameraHeight = 117.91f;
                if (ImGui::InputFloat("相机FPV", &mod->CameraFOV, 1, 10)) mod->set_camera_params();
                ImGui::SameLine();
                if (ImGui::Button("##重置")) mod->CameraFOV = 46;
                // 灯光
                ImGui::SeparatorText("灯光");
                if (ImGui::InputFloat("灯光亮度", &mod->LightIntensity, 1, 10)) mod->set_light_params();
                if (ImGui::InputFloat4("灯光颜色", mod->LightColor)) mod->set_light_params();
                // 资产列表
                ImGui::SeparatorText("模型资产列表");
                if (ImGui::Button("刷新模型资产")) mod->find_asset();
                ImGui::InputText("搜索模型资产", mod->GuiCAL_StaticMeshAsset.SearchStr, 256);
                mod->GuiCAL_StaticMeshAsset.Draw();
                // 材质列表
                ImGui::SeparatorText("模型材质列表");
                if (ImGui::Button("加载材质")) mod->load_material();
                mod->GuiCAL_StaticMeshMaterials.Draw();
                // 操作按钮
                if (ImGui::Button("post_proces_volume_test")) {
                    mod->post_proces_volume_test();
                }
                if (ImGui::Button("ttttt1111")) mod->ttttt1111();
                if (ImGui::Button("ttttt2222")) mod->ttttt2222();

                // 滚动条结束
                ImGui::BeginChild("末尾占位", ImVec2(0, 100));
                ImGui::EndChild();
                ImGui::EndChild();
                ImGui::PopStyleVar();
            });
        }

        ~MyExampleMod() override = default;

        auto on_program_start() -> void override {
            UClassHelper::Get().Init();
            GuiCAL_StaticMeshAsset.GetMenuItems().emplace_back( "查看模型", [this](FAssetData* data, const int) { this->display_static_mesh(data); } );
            GuiCAL_StaticMeshMaterials.GetMenuItems().emplace_back( "设置材质", [this](UObject** obj, const int) { this->display_material(*obj); } );
        }

        auto on_dll_load(std::wstring_view dll_name) -> void override {
        }

        auto on_unreal_init() -> void override {
            // find_asset();
        }

        auto post_proces_volume_test() -> void {
            const auto Registry = Cast<UAssetRegistry>(UAssetRegistryHelpers::GetAssetRegistry().ObjectPointer);
            if (!Registry) {
                Output::send<LogLevel::Error>(STR("[find_asset] Can not get Asset Registry!\n"));
                return;
            }
            UObject* asset = nullptr;
            for (auto ret = AssetUtils::GetAssetsByClass(Registry, FPath(/Script/Engine,Material)); auto &item : ret) {
                Output::send<LogLevel::Normal>(STR("[find_asset] Asset Name: {}\n"), item.AssetName().ToString()); // Debug
                if (item.AssetName().ToString() == L"NewMaterial") {
                    asset = UAssetRegistryHelpers::GetAsset(item);
                    Output::send<LogLevel::Normal>(STR("[post_proces_volume_test] GetAsset !\n"));
                    break;
                }
            }
            if (!asset) return;
            const FVector ZeroPos(0, 0, 0);
            const FRotator ZeroRot(0, 0, 0);
            PostProcessVolume = World->SpawnActor(UClassHelper::Get().GetClass("PostProcessVolume"), &ZeroPos, &ZeroRot);
            Output::send<LogLevel::Normal>(STR("[post_proces_volume_test] 1. SpawnActor!\n"));
            if (!PostProcessVolume) return;
            FProperty* Settings_FP = PostProcessVolume->GetPropertyByName(FromCharTypePtr<TCHAR>(L"Settings"));
            Output::send<LogLevel::Normal>(STR("[post_proces_volume_test] 2. Settings_FP!\n"));
            if (!Settings_FP) return;
            void* Settings_Ptr = Settings_FP->ContainerPtrToValuePtr<void>(PostProcessVolume);
            Output::send<LogLevel::Normal>(STR("[post_proces_volume_test] 3. Settings_Ptr!\n"));
            if (!Settings_Ptr) return;
            const auto Settings_FSP = CastField<FStructProperty>(Settings_FP);
            Output::send<LogLevel::Normal>(STR("[post_proces_volume_test] 4. Settings_FSP!\n"));
            if (!Settings_FSP) return;
            UScriptStruct* Settings = Settings_FSP->GetStruct();
            Output::send<LogLevel::Normal>(STR("[post_proces_volume_test] 5. Settings!\n"));
            if (!Settings) return;
            FProperty* WeightedBlendables_FP = Settings->GetPropertyByName(FromCharTypePtr<TCHAR>(L"WeightedBlendables"));
            Output::send<LogLevel::Normal>(STR("[post_proces_volume_test] 6. WeightedBlendables_FP!\n"));
            if (!WeightedBlendables_FP) return;
            void* WeightedBlendables_Ptr = WeightedBlendables_FP->ContainerPtrToValuePtr<void>(Settings_Ptr);
            Output::send<LogLevel::Normal>(STR("[post_proces_volume_test] 7. WeightedBlendables_Ptr!\n"));
            auto WeightedBlendables_FSP = CastField<FStructProperty>(WeightedBlendables_FP);
            Output::send<LogLevel::Normal>(STR("[post_proces_volume_test] 8. WeightedBlendables_FSP!\n"));
            if (!WeightedBlendables_FSP) return;
            UScriptStruct* WeightedBlendables = WeightedBlendables_FSP->GetStruct();
            Output::send<LogLevel::Normal>(STR("[post_proces_volume_test] 9. WeightedBlendables!\n"));
            if (!WeightedBlendables) return;
            FProperty* Array_FP = WeightedBlendables->GetPropertyByName(FromCharTypePtr<TCHAR>(L"Array"));
            Output::send<LogLevel::Normal>(STR("[post_proces_volume_test] 10. Array_FP!\n"));
            if (!Array_FP) return;
            void* Array_Ptr = Array_FP->ContainerPtrToValuePtr<void>(WeightedBlendables_Ptr);
            Output::send<LogLevel::Normal>(STR("[post_proces_volume_test] 11. Array_Ptr!\n"));
            if (!Array_Ptr) return;
            const auto Array_Value = static_cast<TArray<FWeightedBlendable>*>(Array_Ptr);
            Output::send<LogLevel::Normal>(STR("[post_proces_volume_test] 12. Array_Value!\n"));
            auto num = Array_Value->Num();
            Output::send<LogLevel::Normal>(STR("[post_proces_volume_test] Array_Value.Num: {}!\n"), num);
            FWeightedBlendable newItem { 1.0f, asset };
            Array_Value->Add(newItem);
            num = Array_Value->Num();
            Output::send<LogLevel::Normal>(STR("[post_proces_volume_test] Array_Value.Num: {}!\n"), num);
            auto bUnbound_FP = PostProcessVolume->GetPropertyByName(FromCharTypePtr<TCHAR>(L"bUnbound"));
            Output::send<LogLevel::Normal>(STR("[post_proces_volume_test] 13. bUnbound_FP!\n"));
            if (!bUnbound_FP) return;
            auto bUnbound_FBP = CastField<FBoolProperty>(bUnbound_FP);
            Output::send<LogLevel::Normal>(STR("[post_proces_volume_test] 14. bUnbound_FBP!\n"));
            if (!bUnbound_FBP) return;
            bUnbound_FBP->SetPropertyValue(PostProcessVolume, true);
            Output::send<LogLevel::Normal>(STR("[post_proces_volume_test] 15. bUnbound_FBP.Set!\n"));
        }

        auto ttttt1111() -> void {
            MCallFunction_Begin_InChain(PostProcessVolume, "SetActorHiddenInGame")
            struct {
                bool bNewHidden;
            } params { true };
            MCallFunction_Call
            MCallFunction_End
        }

        auto ttttt2222() -> void {
            MCallFunction_Begin_InChain(PostProcessVolume, "SetActorHiddenInGame")
            struct {
                bool bNewHidden;
            } params { false };
            MCallFunction_Call
            MCallFunction_End
        }

        auto find_asset() -> void {
            // TODO 如果扫不到资产考虑使用/Script/Engine.KismetSystemLibrary:LoadAsset/LoadAsset_Blocking
            StaticMeshAssetData.clear();
            // 获取资产仓库
            const auto Registry = Cast<UAssetRegistry>(UAssetRegistryHelpers::GetAssetRegistry().ObjectPointer);
            if (!Registry) {
                Output::send<LogLevel::Error>(STR("[find_asset] Can not get Asset Registry!\n"));
                return;
            }
            for (auto ret = AssetUtils::GetAssetsByClass(Registry, FPath(/Script/Engine,StaticMesh)); auto &item : ret) {
                // wchar_t 转 char
                std::string str;
                auto o_str = item.PackageName().ToString();
                const auto w_str = o_str.c_str();
                Output::send<LogLevel::Normal>(STR("[find_asset] Asset Name: {}\n"), o_str); // Debug
                for (const wchar_t* cp = w_str; *cp; ++cp) str.push_back(static_cast<char>(*cp));
                StaticMeshAssetData.emplace_back(std::move(str), std::move(item));
            }
            Output::send<LogLevel::Normal>(STR("[find_asset] Asset Num: {}\n"), StaticMeshAssetData.size());
        }

        auto init_scene() -> void {
            // 获得Engine (目前没用)
            Engine = UObjectGlobals::FindFirstOf(L"Engine");
            if (!Engine) {
                Output::send<LogLevel::Error>(STR("[init_scene] Can not get Engine!\n"));
                return;
            }

            // 获得活动的PlayerController
            std::vector<UObject*> PCStorage;
            UObjectGlobals::FindAllOf(L"PlayerController", PCStorage);
            Output::send<LogLevel::Normal>(STR("[init_scene] PlayerController Num: {}\n"), PCStorage.size());
            for (const auto pc : PCStorage) {
                if (!pc) continue;
                MCallFunction_Begin_InChain(pc, "IsLocalController")
                struct {
                    bool ReturnValue;
                } params{};
                MCallFunction_Call
                if (params.ReturnValue) {
                    CurrentPlayerController = pc;
                    break;
                }
                MCallFunction_End
            }
            if (!CurrentPlayerController) {
                Output::send<LogLevel::Error>(STR("[init_scene] Can not get active PlayerController!\n"));
                return;
            }
            World = CurrentPlayerController->GetWorld();
            if (!World) return;

            // 添加物体
            // 默认Pawn
            const FVector PawnPos(0, 0, 0);
            const FRotator PawnRot(90, 0, 0);
            PawnActor = World->SpawnActor(UClassHelper::Get().GetClass("DefaultPawn"), &PawnPos, &PawnRot);
            MCallFunction_Begin_InChain(CurrentPlayerController, "Possess")
            struct {
                UObject* InPawn;
            } params { PawnActor };
            MCallFunction_Call
            MCallFunction_End
            set_pawn_params();

            // 创建相机
            const FVector CamPos(0, 0, 0);
            const FRotator CamRot(90, 0, 0);
            CameraActor = World->SpawnActor(UClassHelper::Get().GetClass("CameraActor"), &CamPos, &CamRot);
            if (!CameraActor) Output::send<LogLevel::Error>(STR("[init_scene] Can not add CameraActor!\n"));
            // 设置纵横比
            if (const auto comp = MGetComponentsByClass(CameraActor, "CameraComponent")) {
                MCallFunction_Begin(comp, "SetAspectRatio")
                struct {
                    float InAspectRatio;
                } params { 1.0f };
                MCallFunction_Call
                MCallFunction_End
                MCallFunction_Begin(comp, "SetConstraintAspectRatio") // 强制
                struct {
                    bool bInConstrainAspectRatio;
                } params { true };
                MCallFunction_Call
                MCallFunction_End
            }
            set_camera_params();
            set_resolution();
            set_game_mode();
            // 创建灯光
            const FVector LightPos(0, 0, 0);
            const FRotator LightRot(90, 0, 0);
            DirectionalLight = World->SpawnActor(UClassHelper::Get().GetClass("DirectionalLight"), &LightPos, &LightRot);
            set_light_params();

            // 创建展示 StaticMeshActor
            const FVector MeshPos(0, 0, 2000);
            const FRotator MeshRot(0, 0, 0);
            OnDisplayStaticMeshActor = World->SpawnActor(UClassHelper::Get().GetClass("StaticMeshActor"), &MeshPos, &MeshRot);
            if (!OnDisplayStaticMeshActor) Output::send<LogLevel::Error>(STR("[init_scene] Can not add StaticMeshActor!\n"));
            MCallFunction_Begin_InChain(OnDisplayStaticMeshActor, "SetMobility")
            struct {
                int NewMobility;
            } params { 2 };
            MCallFunction_Call
            MCallFunction_End
            set_mesh_params();
            //
            set_hidden_params();
        }

        auto set_game_mode() const -> void {
            auto NewViewTarget = CameraActor;
            if (GameMode == 0) {
                NewViewTarget = PawnActor;
            }
            // 切换相机
            MCallFunction_Begin_InChain(CurrentPlayerController, "SetViewTargetWithBlend")
            struct {
                UObject* NewViewTarget;
            } params { NewViewTarget };
            MCallFunction_Call
            MCallFunction_End
        }

        auto set_resolution() const -> void {
            Output::send<LogLevel::Normal>(STR("[set_resolution]\n"));
            FIntPoint resolution{};
            switch (ResolutionOptionsSelectedIndex) {
                case 0:
                    resolution = FIntPoint(512, 512);
                    break;
                case 1:
                    resolution = FIntPoint(768, 768);
                    break;
                case 2:
                    resolution = FIntPoint(1024, 1024);
                    break;
                case 3:
                    resolution = FIntPoint(2048, 2048);
                    break;
                case 4:
                    resolution = FIntPoint(4096, 4096);
                    break;
                default:
                    resolution = FIntPoint(1024, 1024);
            }
            if (UObject* GameUserSettings = UObjectGlobals::FindFirstOf(L"GameUserSettings")) {
                // 分辨率
                MCallFunction_Begin(GameUserSettings, "SetScreenResolution")
                struct {
                    FIntPoint Resolution;
                } params { resolution };
                MCallFunction_Call
                MCallFunction_End
                // 窗口模式
                MCallFunction_Begin(GameUserSettings, "SetFullscreenMode") // 0: Fullscreen, 1: WindowedFullscreen, 2: Windowed
                struct {
                    int InFullscreenMode;
                } params { WindowModeOptionsSelectedIndex };
                MCallFunction_Call
                MCallFunction_End
                // 应用
                MCallFunction_Begin(GameUserSettings, "ApplySettings")
                struct {
                    bool bCheckForCommandLineOverrides;
                } params { true };
                MCallFunction_Call
                MCallFunction_End
            } else Output::send<LogLevel::Error>(STR("[set_resolution] Can not get GameUserSettings!\n"));

        }

        auto set_hidden_params() const -> void {
            Output::send<LogLevel::Normal>(STR("[set_hidden_params]\n"));
            if (HideActor) {
                // 查找全部Actor
                std::vector<UObject*> ActorStorage;
                UObjectGlobals::FindAllOf(L"Actor", ActorStorage);
                Output::send<LogLevel::Normal>(STR("[remove_all_actors] Actor Num: {}\n"), ActorStorage.size());
                // 删除全部Actor
                for (const auto actor : ActorStorage) {
                    if (actor == OnDisplayStaticMeshActor || actor == CameraActor || actor == DirectionalLight) continue;
                    MCallFunction_Begin_InChain(actor, "SetActorHiddenInGame")
                    struct {
                        bool bNewHidden;
                    } params { true };
                    MCallFunction_Call
                    MCallFunction_End
                }
            }
            if (HideUI) {
                // 删除UI
                std::vector<UObject*> WidgetStorage;
                UObjectGlobals::FindAllOf(L"Widget", WidgetStorage);
                for (const auto widget : WidgetStorage) {
                    MCallFunction_Begin_InChain(widget, "SetVisibility")
                    struct {
                        uint8 InVisibility;
                    } params { 2 };
                    MCallFunction_Call
                    MCallFunction_End
                }
            }
        }

        static auto get_static_mesh_materials(FAssetData *asset) -> std::vector<UObject*> {
            Output::send<LogLevel::Normal>(STR("[get_static_mesh_materials]\n"));
            if (!asset) return {};
            std::vector<UObject*> materials;
            if (const auto obj = UAssetRegistryHelpers::GetAsset(*asset)) {
                int32 MaterialNum = 0;
                if (Version::IsAtLeast(4, 27)) {
                    MCallFunction_Begin(obj, "GetStaticMaterials")
                    struct {
                        TArray<UObject> ReturnValue;
                    } params;
                    MCallFunction_Call
                    MaterialNum = params.ReturnValue.Num();
                    MCallFunction_End
                } else {
                    if (const FProperty* StaticMaterials_FP = obj->GetPropertyByNameInChain(FromCharTypePtr<TCHAR>(L"StaticMaterials"))) {
                        void* StaticMaterials_Ptr = StaticMaterials_FP->ContainerPtrToValuePtr<void>(obj);
                        if (const auto StaticMaterials_Array = static_cast<TArray<void*>*>(StaticMaterials_Ptr)) {
                            MaterialNum = StaticMaterials_Array->Num();
                        } else Output::send<LogLevel::Error>(STR("[get_static_mesh_materials] Can not cast to StaticMaterials_Array!\n"));
                    } else Output::send<LogLevel::Error>(STR("[get_static_mesh_materials] Can not get StaticMaterials_FP!\n"));
                }
                if (MaterialNum == 0) return materials;
                Output::send<LogLevel::Normal>(STR("[get_static_mesh_materials] GetStaticMaterials {}!\n"), MaterialNum);
                MCallFunction_Begin(obj, "GetMaterial")
                for (int32 i = 0; i < MaterialNum; ++i) {
                    struct {
                        int32 MaterialIndex;
                        UObject* ReturnValue;
                    } params { i };
                    MCallFunction_Call
                    if (params.ReturnValue) {
                        Output::send<LogLevel::Normal>(STR("[get_static_mesh_materials] GetMaterial: {}!\n"), params.ReturnValue->GetFullName());
                        materials.push_back(params.ReturnValue);
                    }
                }
                MCallFunction_End
            } else Output::send<LogLevel::Error>(STR("[get_static_mesh_materials] Can not get asset!\n"));
            return materials;
        }

        auto load_material() -> void {
            Output::send<LogLevel::Normal>(STR("[load_material]\n"));
            StaticMeshMaterialsData.clear();
            for (const auto ret = get_static_mesh_materials(GuiCAL_StaticMeshAsset.GetSelectedItem()); auto obj : ret) {
                // wchar_t 转 char
                std::string str;
                auto o_str = obj->GetFullName();
                const auto w_str = o_str.c_str();
                Output::send<LogLevel::Normal>(STR("[load_material] Material Name: {}\n"), w_str); // Debug
                for (const wchar_t* cp = w_str; *cp; ++cp) str.push_back(static_cast<char>(*cp));
                StaticMeshMaterialsData.emplace_back(std::move(str), obj);
            }
        }

        auto display_material(UObject *material) const -> void {
            if (!OnDisplayStaticMeshActor) return;
            Output::send<LogLevel::Normal>(STR("[display_material]\n"));
            if (const auto comp = MGetComponentsByClass(OnDisplayStaticMeshActor, "StaticMeshComponent")) {
                MCallFunction_Begin_InChain(comp, "SetMaterial")
                struct {
                    int32 MaterialIndex;
                    UObject* NewMaterial;
                } params { 0, material };
                MCallFunction_Call
                MCallFunction_End
            }
        }

        auto display_static_mesh(FAssetData *asset) const -> void {
            if (!OnDisplayStaticMeshActor) return;
            Output::send<LogLevel::Normal>(STR("[display_static_mesh]\n"));
            if (const auto comp = MGetComponentsByClass(OnDisplayStaticMeshActor, "StaticMeshComponent")) {
                if (const auto obj = UAssetRegistryHelpers::GetAsset(*asset)) {
                    MCallFunction_Begin(comp, "SetStaticMesh")
                    struct {
                        UObject* NewMesh;
                    } params { obj };
                    MCallFunction_Call
                    MCallFunction_End
                } else Output::send<LogLevel::Error>(STR("[display_static_mesh] Can not get asset!\n"));
            }
        }

        auto set_mesh_params() const -> void {
            if (!OnDisplayStaticMeshActor) return;
            Output::send<LogLevel::Normal>(STR("[set_mesh_params]\n"));
            FHitResult _tmp;
            OnDisplayStaticMeshActor->K2_SetActorLocation(FVector(0, 0, ZeroHeight), false, _tmp, true);
        }

        auto set_camera_params() const -> void {
            if (!CameraActor) return;
            Output::send<LogLevel::Normal>(STR("[set_camera_params]\n"));
            FHitResult _tmp;
            CameraActor->K2_SetActorLocation(FVector(0, 0, ZeroHeight + CameraHeight), false, _tmp, true);
            if (const auto comp = MGetComponentsByClass(CameraActor, "CameraComponent")) {
                MCallFunction_Begin(comp, "SetFieldOfView")
                struct {
                    float InFieldOfView;
                } params { CameraFOV };
                MCallFunction_Call
                Output::send<LogLevel::Normal>(STR("[set_camera_params] SetFieldOfView {}!\n"), CameraFOV);
                MCallFunction_End
            }
        }

        auto set_pawn_params() const -> void {
            if (!PawnActor) return;
            Output::send<LogLevel::Normal>(STR("[set_pawn_params]\n"));
            FHitResult _tmp;
            PawnActor->K2_SetActorLocation(FVector(0, 0, ZeroHeight + CameraHeight), false, _tmp, true);
        }

        auto set_light_params() const -> void {
            if (!DirectionalLight) return;
            Output::send<LogLevel::Normal>(STR("[set_light_params]\n"));
            if (const auto comp = MGetComponentsByClass(DirectionalLight, "LightComponent")) {
                MCallFunction_Begin_InChain(comp, "SetIntensity")
                struct {
                    float NewIntensity;
                } params { LightIntensity };
                MCallFunction_Call
                Output::send<LogLevel::Normal>(STR("[set_light_params] SetIntensity {}!\n"), LightIntensity);
                MCallFunction_End
                MCallFunction_Begin_InChain(comp, "SetLightColor")
                struct {
                    FLinearColor NewLightColor;
                    bool bSRGB;
                } params { FLinearColor(LightColor[0], LightColor[1], LightColor[2], LightColor[3]), true };
                MCallFunction_Call
                Output::send<LogLevel::Normal>(STR("[set_light_params] SetLightColor ({}, {}, {}, {})!\n"), LightColor[0], LightColor[1], LightColor[2], LightColor[3]);
                MCallFunction_End
            }
        }

    private:
        //
        int GameMode = 0;
        bool HideActor = false;
        bool HideUI = true;
        const char* WindowModeOptions[3] = { "Fullscreen", "WindowedFullscreen", "Windowed" };
        int WindowModeOptionsSelectedIndex = 2;
        const char* ResolutionOptions[5] = { "512×512", "768×768", "1024×1024", "2048×2048", "4096×4096" };
        int ResolutionOptionsSelectedIndex = 2;
        //
        float ZeroHeight = 1000;
        // 相机 (重置按钮那还有值)
        float CameraHeight = 117.91f;
        float CameraFOV = 46;
        // 灯光
        float LightIntensity = 2;
        float* LightColor = new float[4]{ 1, 1, 1, 1 };
        //
        UObject* Engine = nullptr;
        std::vector<std::pair<std::string, FAssetData>> StaticMeshAssetData;
        GuiComponent_List<FAssetData> GuiCAL_StaticMeshAsset { "模型资产", &StaticMeshAssetData };
        std::vector<std::pair<std::string, UObject*>> StaticMeshMaterialsData;
        GuiComponent_List<UObject*> GuiCAL_StaticMeshMaterials { "模型材质", &StaticMeshMaterialsData };
        UObject* CurrentPlayerController = nullptr;
        UWorld* World = nullptr;
        AActor* PawnActor = nullptr;
        AActor* CameraActor = nullptr;
        AActor* OnDisplayStaticMeshActor = nullptr;
        AActor* DirectionalLight = nullptr;
        AActor* PostProcessVolume = nullptr;
    }; //class
}

/**
* export the start_mod() and uninstall_mod() functions to
* be used by the core ue4ss system to load in our dll mod
*/
#define MOD_EXPORT __declspec(dllexport)

extern "C" {
MOD_EXPORT RC::CppUserModBase *start_mod() { return new MyMods::MyExampleMod(); }
MOD_EXPORT void uninstall_mod(RC::CppUserModBase *mod) { delete mod; }
}
    




