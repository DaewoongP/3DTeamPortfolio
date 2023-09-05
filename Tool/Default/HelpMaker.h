#pragma once

#include "Base.h"
#include "Tool_Defines.h"

BEGIN(Tool)

// ImGui���� ������ �Լ� ������
class CImGui_Function final
{
public:
    // (?)�� Ŀ���� �������� ������ ����ִ� ���
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
