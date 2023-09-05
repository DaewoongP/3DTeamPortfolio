#pragma once

#include "Base.h"
#include "Tool_Defines.h"

BEGIN(Tool)

// ImGui에서 쓸만한 함수 모음집
class CImGui_Function final
{
public:
    // (?)에 커서를 가져가면 문구를 띄워주는 기능
    inline static void HelpMarker(const char* desc)
    {
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort) && ImGui::BeginTooltip())
        {
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }
};

END
