//
// Created by chaim on 2025/9/30.
//

#include "AssetUtils.h"

auto AssetUtils::GetAssetsByClass(UAssetRegistry *registry, const FName ClassNameLeft, const FName ClassNameRight, const FName ClassNameFull, const bool bSearchSubClasses) -> TArray<FAssetData> {
    TArray<FAssetData> result;
    if (!registry) {
        Output::send<LogLevel::Error>(STR("[AssetUtils::GetAssetsByClass] Can not get Asset Registry!\n"));
        return result;
    }
    if (Version::IsAtLeast(5, 1)) {
        auto params = GetAssetsByClass_Params_5_1 {
            { ClassNameLeft, ClassNameRight },
            {},
            bSearchSubClasses
        };
        // 检查GetAssetsByClass反射
        if (!UAssetRegistry::Functions::GetAssetsByClass.IsValid()) {
            Output::send<LogLevel::Error>(STR("[AssetUtils::GetAssetsByClass] Can not get Function 'GetAssetsByClass'!\n"));
            return result;
        }
        UAssetRegistry::Functions::GetAssetsByClass(registry, params);
        result.CopyFast(params.OutAssetData);
    } else {
        registry->GetAssetsByClass(ClassNameFull, result, bSearchSubClasses);
    }
    return result;
}
