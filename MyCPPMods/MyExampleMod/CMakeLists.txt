cmake_minimum_required(VERSION 3.22)

set(TARGET MyExampleMod)
project(${TARGET})

add_library(${TARGET} SHARED
    "src/dllmain.cpp"
    "src/GuiComponent_List.h"
    "src/AssetUtils.h"
    "src/AssetUtils.cpp"
    "src/UClassHelper.hpp"
    "src/TypeDefine.h"
)
target_include_directories(${TARGET} PRIVATE .)

target_link_libraries(${TARGET} PRIVATE UE4SS)
