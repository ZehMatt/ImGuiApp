#pragma once

#include <ImGuiApp/window.hpp>
#include <imgui.h>

namespace ImGuiApp
{
    enum WindowFlags : std::uint32_t
    {
        WindowFlag_None = 0U,
        WindowFlag_TakeFocus = (1U << 0),
    };
    
    class WindowInternal : public Window
    {
    public:
        void internalUpdate()
        {
            ImGui::SetNextWindowSize(
                ImVec2{ static_cast<float>(_settings.width), static_cast<float>(_settings.height) }, ImGuiCond_FirstUseEver);
                        
            if (_flags & WindowFlags::WindowFlag_TakeFocus)
            {
                _flags &= ~WindowFlags::WindowFlag_TakeFocus;
                ImGui::SetNextWindowFocus();
            }
                            
            ImGui::Begin(_windowName.c_str(), &_isOpen, 0);
            {                
                update();
            }
            ImGui::End();
        }
    };

} // namespace ImGuiApp