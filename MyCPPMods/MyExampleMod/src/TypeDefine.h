//
// Created by chaim on 2025/10/8.
//

#ifndef MYCPPMODS_TYPEDEFINE_H
#define MYCPPMODS_TYPEDEFINE_H

namespace MyMods {
    struct FLinearColor {
        float R;
        float G;
        float B;
        float A;
    };

    struct FIntPoint {
        int32 X;
        int32 Y;
    };

    struct FWeightedBlendable {
        float Weight;
        UObject* Object;
    };
}
#endif //MYCPPMODS_TYPEDEFINE_H