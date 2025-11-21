//
// Created by chaim on 2025/9/30.
//

#ifndef MYCPPMODS_UCLASSHELPER_H
#define MYCPPMODS_UCLASSHELPER_H

using namespace RC;
using namespace Unreal;

class UClassHelper
{
public:
    // 获取单例
    static UClassHelper& Get()
    {
        static UClassHelper Instance;
        return Instance;
    }

    // 初始化常用类
    void Init()
    {
        AddClass(L"/Script/Engine.DefaultPawn", "DefaultPawn");
        AddClass(L"/Script/Engine.CameraActor", "CameraActor");
        AddClass(L"/Script/Engine.DirectionalLight", "DirectionalLight");
        AddClass(L"/Script/Engine.PointLight", "PointLight");
        AddClass(L"/Script/Engine.StaticMeshActor", "StaticMeshActor");
        AddClass(L"/Script/Engine.StaticMeshComponent", "StaticMeshComponent");
        AddClass(L"/Script/Engine.LightComponent", "LightComponent");
        AddClass(L"/Script/Engine.CameraComponent", "CameraComponent");
        AddClass(L"/Script/Engine.PostProcessVolume", "PostProcessVolume");
    }

    // 根据名字获取 UClass*
    UClass* GetClass(const std::string& Name)
    {
        if (const auto it = Classes.find(Name); it != Classes.end())
        {
            return it->second;
        }
        return nullptr;
    }

private:
    std::unordered_map<std::string, UClass*> Classes;

    // 构造函数私有化，禁止外部 new
    UClassHelper() = default;

    void AddClass(const wchar_t* Path, const std::string& Name)
    {
        if (auto* Cls = UObjectGlobals::StaticFindObject<UClass*>(nullptr, nullptr, Path))
        {
            Classes[Name] = Cls;
        }
    }
};


#endif //MYCPPMODS_UCLASSHELPER_H