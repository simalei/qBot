//
// Created by asimo on 11/20/2022.
//

#include "imguiExt.hpp"
namespace ImGuiExt
{

    // https://github.com/forrestthewoods/lib_fts

    // Forward declarations for "private" implementation
    namespace fuzzy_internal {
        static bool fuzzy_match_recursive(const char* pattern, const char* str, int& outScore, const char* strBegin,
                                          uint8_t const* srcMatches, uint8_t* newMatches, int maxMatches, int nextMatch,
                                          int& recursionCount, int recursionLimit);
    }
    // Private implementation
    static bool fuzzy_internal::fuzzy_match_recursive(const char* pattern, const char* str, int& outScore,
                                                      const char* strBegin, uint8_t const* srcMatches, uint8_t* matches, int maxMatches,
                                                      int nextMatch, int& recursionCount, int recursionLimit)
    {
        // Count recursions
        ++recursionCount;
        if (recursionCount >= recursionLimit)
            return false;

        // Detect end of strings
        if (*pattern == '\0' || *str == '\0')
            return false;

        // Recursion params
        bool recursiveMatch = false;
        uint8_t bestRecursiveMatches[256];
        int bestRecursiveScore = 0;

        // Loop through pattern and str looking for a match
        bool first_match = true;
        while (*pattern != '\0' && *str != '\0') {

            // Found match
            if (tolower(*pattern) == tolower(*str)) {

                // Supplied matches buffer was too short
                if (nextMatch >= maxMatches)
                    return false;

                // "Copy-on-Write" srcMatches into matches
                if (first_match && srcMatches) {
                    memcpy(matches, srcMatches, nextMatch);
                    first_match = false;
                }

                // Recursive call that "skips" this match
                uint8_t recursiveMatches[256];
                int recursiveScore;
                if (fuzzy_match_recursive(pattern, str + 1, recursiveScore, strBegin, matches, recursiveMatches, sizeof(recursiveMatches), nextMatch, recursionCount, recursionLimit)) {

                    // Pick best recursive score
                    if (!recursiveMatch || recursiveScore > bestRecursiveScore) {
                        memcpy(bestRecursiveMatches, recursiveMatches, 256);
                        bestRecursiveScore = recursiveScore;
                    }
                    recursiveMatch = true;
                }

                // Advance
                matches[nextMatch++] = (uint8_t)(str - strBegin);
                ++pattern;
            }
            ++str;
        }

        // Determine if full pattern was matched
        bool matched = *pattern == '\0' ? true : false;

        // Calculate score
        if (matched) {
            const int sequential_bonus = 15;            // bonus for adjacent matches
            const int separator_bonus = 30;             // bonus if match occurs after a separator
            const int camel_bonus = 30;                 // bonus if match is uppercase and prev is lower
            const int first_letter_bonus = 15;          // bonus if the first letter is matched

            const int leading_letter_penalty = -5;      // penalty applied for every letter in str before the first match
            const int max_leading_letter_penalty = -15; // maximum penalty for leading letters
            const int unmatched_letter_penalty = -1;    // penalty for every letter that doesn't matter

            // Iterate str to end
            while (*str != '\0')
                ++str;

            // Initialize score
            outScore = 100;

            // Apply leading letter penalty
            int penalty = leading_letter_penalty * matches[0];
            if (penalty < max_leading_letter_penalty)
                penalty = max_leading_letter_penalty;
            outScore += penalty;

            // Apply unmatched penalty
            int unmatched = (int)(str - strBegin) - nextMatch;
            outScore += unmatched_letter_penalty * unmatched;

            // Apply ordering bonuses
            for (int i = 0; i < nextMatch; ++i) {
                uint8_t currIdx = matches[i];

                if (i > 0) {
                    uint8_t prevIdx = matches[i - 1];

                    // Sequential
                    if (currIdx == (prevIdx + 1))
                        outScore += sequential_bonus;
                }

                // Check for bonuses based on neighbor character value
                if (currIdx > 0) {
                    // Camel case
                    char neighbor = strBegin[currIdx - 1];
                    char curr = strBegin[currIdx];
                    if (::islower(neighbor) && ::isupper(curr))
                        outScore += camel_bonus;

                    // Separator
                    bool neighborSeparator = neighbor == '_' || neighbor == ' ';
                    if (neighborSeparator)
                        outScore += separator_bonus;
                }
                else {
                    // First letter
                    outScore += first_letter_bonus;
                }
            }
        }

        // Return best result
        if (recursiveMatch && (!matched || bestRecursiveScore > outScore)) {
            // Recursive score is better than "this"
            memcpy(matches, bestRecursiveMatches, maxMatches);
            outScore = bestRecursiveScore;
            return true;
        }
        else if (matched) {
            // "this" score is better than recursive
            return true;
        }
        else {
            // no match
            return false;
        }
    }



    static bool fuzzy_match(char const* pattern, char const* str, int& outScore, uint8_t* matches, int maxMatches) {
        int recursionCount = 0;
        int recursionLimit = 10;

        return fuzzy_internal::fuzzy_match_recursive(pattern, str, outScore, str, nullptr, matches, maxMatches, 0, recursionCount, recursionLimit);
    }

    // Public interface
    bool fuzzy_match_simple(char const* pattern, char const* str) {
        while (*pattern != '\0' && *str != '\0') {
            if (tolower(*pattern) == tolower(*str))
                ++pattern;
            ++str;
        }

        return *pattern == '\0' ? true : false;
    }

    bool fuzzy_match(char const* pattern, char const* str, int& outScore) {

        uint8_t matches[256];
        return fuzzy_match(pattern, str, outScore, matches, sizeof(matches));
    }

    static bool sortbysec_desc(const std::pair<int, int>& a, const std::pair<int, int>& b)
    {
        return (b.second < a.second);
    }

    bool ComboWithFilter(const char* label, int* current_item, const std::vector<std::string>& items)
    {
        ImGuiContext& g = *GImGui;

        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return false;

        const ImGuiStyle& style = g.Style;
        int items_count = items.size();

        // Call the getter to obtain the preview string which is a parameter to BeginCombo()
        const char* preview_value = NULL;
        if (*current_item >= 0 && *current_item < items_count)
            preview_value = items[*current_item].c_str();

        static char pattern_buffer[256] = { 0 };
        bool isNeedFilter = false;

        char comboButtonName[512] = { 0 };
        ImFormatString(comboButtonName, IM_ARRAYSIZE(comboButtonName), "%s##name_ComboWithFilter_button_%s", preview_value? preview_value:"", label);

        char name_popup[256 + 10];
        ImFormatString(name_popup, IM_ARRAYSIZE(name_popup), "##name_popup_%s", label);

        // Display items
        // FIXME-OPT: Use clipper (but we need to disable it on the appearing frame to make sure our call to SetItemDefaultFocus() is processed)
        bool value_changed = false;

        const float expected_w = ImGui::CalcItemWidth();
        ImVec2 item_min = ImGui::GetItemRectMin();
        bool isNewOpen = false;
        float sz = ImGui::GetFrameHeight();
        ImVec2 size(sz, sz);
        ImVec2 CursorPos = window->DC.CursorPos;
        ImVec2 pos = CursorPos + ImVec2(expected_w-sz, 0);
        const ImRect bb(pos, pos + size);

        float ButtonTextAlignX = g.Style.ButtonTextAlign.x;
        g.Style.ButtonTextAlign.x = 0;
        if (ImGui::Button(comboButtonName, ImVec2(expected_w, 0)))
        {
            ImGui::OpenPopup(name_popup);
            isNewOpen = true;
        }
        g.Style.ButtonTextAlign.x = ButtonTextAlignX;
        bool hovered = ImGui::IsItemHovered();
        bool active = ImGui::IsItemActivated();
        bool pressed = ImGui::IsItemClicked();

        // Render
        //const ImU32 bg_col = GetColorU32((active && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
        //RenderFrame(bb.Min, bb.Max, bg_col, true, g.Style.FrameRounding);
        const ImU32 text_col = ImGui::GetColorU32(ImGuiCol_Text);
        ImGui::RenderArrow(window->DrawList, bb.Min + ImVec2(ImMax(0.0f, (size.x - g.FontSize) * 0.5f), ImMax(0.0f, (size.y - g.FontSize) * 0.5f)), text_col, ImGuiDir_Down);

        if (isNewOpen)
        {
            memset(pattern_buffer, 0, IM_ARRAYSIZE(pattern_buffer));
        }
        ImVec2 item_max = ImGui::GetItemRectMax();
        ImGui::SetNextWindowPos({ CursorPos.x, item_max.y });
        ImGui::SetNextWindowSize({ ImGui::GetItemRectSize().x, 0 });
        if (ImGui::BeginPopup(name_popup))
        {
            ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor(240, 240, 240, 255));
            ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(0, 0, 0, 255));
            ImGui::PushItemWidth(-FLT_MIN);
            // Filter input
            if (isNewOpen)
                ImGui::SetKeyboardFocusHere();
            ImGui::InputText("##ComboWithFilter_inputText", pattern_buffer, 256);

            // Search Icon, you can use it if you load IconsFontAwesome5 https://github.com/juliettef/IconFontCppHeaders
            //const ImVec2 label_size = CalcTextSize(ICON_FA_SEARCH, NULL, true);
            //const ImVec2 search_icon_pos(ImGui::GetItemRectMax().x - label_size.x - style.ItemInnerSpacing.x * 2, window->DC.CursorPos.y + style.FramePadding.y + g.FontSize * 0.1f);
            //RenderText(search_icon_pos, ICON_FA_SEARCH);

            ImGui::PopStyleColor(2);
            if (pattern_buffer[0] != '\0')
            {
                isNeedFilter = true;
            }

            std::vector<std::pair<int, int> > itemScoreVector;
            if (isNeedFilter)
            {
                for (int i = 0; i < items_count; i++)
                {
                    int score = 0;
                    bool matched = fuzzy_match(pattern_buffer, items[i].c_str(), score);
                    if (matched)
                        itemScoreVector.push_back(std::make_pair(i, score));
                }
                std::sort(itemScoreVector.begin(), itemScoreVector.end(), sortbysec_desc);
            }

            int show_count = isNeedFilter ? itemScoreVector.size() : items_count;
            if (ImGui::ListBoxHeader("##ComboWithFilter_itemList", show_count))
            {
                for (int i = 0; i < show_count; i++)
                {
                    int idx = isNeedFilter ? itemScoreVector[i].first : i;
                    ImGui::PushID((void*)(intptr_t)idx);
                    const bool item_selected = (idx == *current_item);
                    const char* item_text = items[idx].c_str();
                    if (ImGui::Selectable(item_text, item_selected))
                    {
                        value_changed = true;
                        *current_item = idx;
                        ImGui::CloseCurrentPopup();
                    }
                    if (item_selected)
                        ImGui::SetItemDefaultFocus();
                    ImGui::PopID();
                }
                ImGui::ListBoxFooter();
            }
            ImGui::PopItemWidth();
            ImGui::EndPopup();
        }


        if (value_changed)
            ImGui::MarkItemEdited(GImGui->LastItemData.ID);

        return value_changed;
    }
}