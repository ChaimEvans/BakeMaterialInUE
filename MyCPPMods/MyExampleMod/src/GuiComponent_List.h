//
// Created by chaim on 2025/9/29.
//

#ifndef MYCPPMODS_GUICOMPONENT_H
#define MYCPPMODS_GUICOMPONENT_H

#include "imgui.h"

template<typename T>
class GuiComponent_List {
public:
    explicit GuiComponent_List(const char* label, std::vector<std::pair<std::string, T> > *ld) : ListData(ld), SearchedListData({}), MenuItems({}) {
        Label = label;
        MenuLabel = std::string("菜单: ") + Label;
        SearchStr[0] = '\0';
    };

    ~GuiComponent_List() {
        delete[] SearchStr;
    }

    void Draw() {
        if (!this->ListData) return;
        SearchedListData.clear();
        if (!SearchStr || !*SearchStr || strlen(SearchStr) < 2) {
            for (auto& item : *this->ListData)
                SearchedListData.push_back(&item);
        } else {
            // 转为小写字符串视图
            std::string search_lower(SearchStr);
            std::ranges::transform(search_lower, search_lower.begin(), [](unsigned char c){ return std::tolower(c); });

            for (auto& item : *this->ListData)
            {
                std::string key_lower = item.first;
                std::ranges::transform(key_lower, key_lower.begin(), [](const unsigned char c){ return std::tolower(c); });

                if (key_lower.contains(search_lower))
                {
                    SearchedListData.push_back(&item);
                }
            }
        }
        // 创建列表框
        if (ImGui::ListBox(
            Label,
            &g_selectedIndex,
            [](void *data, const int idx) {
                if (const auto vec = static_cast<std::vector<std::pair<std::string, T>* > *>(data); idx >= 0 && idx < vec->size()) {
                    return vec->at(idx)->first.c_str();
                }
                return "";
            },
            &SearchedListData, SearchedListData.size(), 10)) // 5行高度
        {
            // 选中项改变时的处理
        }
        // 为当前选中的项创建右键菜单
        if (g_selectedIndex != -1 && ImGui::BeginPopupContextItem("菜单 ")) {
            // 菜单项
            for (auto [name, func]: MenuItems) {
                if (ImGui::MenuItem(name.c_str())) {
                    if (g_selectedIndex >= 0 && g_selectedIndex < SearchedListData.size())
                        func(&SearchedListData.at(g_selectedIndex)->second, g_selectedIndex);
                }
            }
            ImGui::EndPopup();
        }
    }

    std::vector<std::pair<std::string, std::function<void(T *, int)> > > &GetMenuItems() { return MenuItems; }

    T* GetSelectedItem() {
        if (g_selectedIndex >= 0 && g_selectedIndex < SearchedListData.size())
            return &this->SearchedListData.at(g_selectedIndex)->second;
        return nullptr;
    }

    char* SearchStr = new char[256];

private:
    const char* Label;
    std::string MenuLabel;
    int g_selectedIndex = -1; // 当前选中的索引
    std::vector<std::pair<std::string, T> > *ListData;
    std::vector<std::pair<std::string, T>* > SearchedListData;
    std::vector<std::pair<std::string, std::function<void(T *, int)> > > MenuItems;
};


#endif //MYCPPMODS_GUICOMPONENT_H
