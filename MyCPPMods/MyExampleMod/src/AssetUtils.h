//
// Created by chaim on 2025/9/30.
//

#ifndef MYCPPMODS_ASSETUTILS_H
#define MYCPPMODS_ASSETUTILS_H

#include "Unreal/FAssetData.hpp"
#include "Unreal/UAssetRegistry.hpp"
#include "Containers/Array.hpp"

#define FPath(A, B) \
    FName(L## #A), \
    FName(L## #B), \
    FName(L## #A L"." L## #B)

using namespace RC;
using namespace Unreal;

class AssetUtils {
public:
    struct GetAssetsByClass_Params_5_1
    {
        FTopLevelAssetPath ClassName;
        TArray<FAssetData> OutAssetData;
        bool bSearchSubClasses;
    };
    //
    static auto GetAssetsByClass(UAssetRegistry* registry, const FName ClassNameLeft, const FName ClassNameRight, const FName ClassNameFull, bool bSearchSubClasses = false) -> TArray<FAssetData> ;
};



#endif //MYCPPMODS_ASSETUTILS_H