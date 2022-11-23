//
// Created by asimo on 11/20/2022.
//

#ifndef QBOT_IMGUIEXT_H
#define QBOT_IMGUIEXT_H

#include <includes.hpp>
#include <imgui_internal.h>

namespace ImGuiExt
{
    IMGUI_API bool ComboWithFilter(const char* label, int* current_item, const std::vector<std::string>& items);
}

#endif //QBOT_IMGUIEXT_H
