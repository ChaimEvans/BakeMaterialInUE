# BakeMaterialInUE
一个基于 UE4SS 的 C++ 程序，能够向UE游戏注入程序，将游戏内的材质烘焙成图像。

## 使用
1. 前往[Releases](https://github.com/ChaimEvans/BakeMaterialInUE/releases)下载 或 自行编译
2. 安装MOD，路径如下：
```
ue4ss/
├── Mods/
│   ├── MyExampleMod/
│   │   └── dlls/
│   │       └── main.dll
│   └── mods.txt
```
3. 修改`mods.txt`，加入：
```
MyExampleMod : 1
```

> 我这里没改MOD名，若冲突请自行编译🫡

## 开发
> - 要求Visual Studio 2022以上，且配置Visual Studio的CMake环境变量
> - 生成项目依赖需要有在@EpicGames Github组织的Github账号
> - 更多信息请查看[https://docs.ue4ss.com/guides/creating-a-c++-mod.html](https://docs.ue4ss.com/guides/creating-a-c++-mod.html)

1. **生成项目依赖**：`generate_project_files.bat`
2. **编译**：`build_mods_shipping.bat` 得 `build\MyCPPMods\MyExampleMod\Game__Shipping__Win64\MyExampleMod.dll`
