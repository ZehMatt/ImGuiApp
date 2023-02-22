#include "ImGuiApp/window.hpp"

#include "window.internal.hpp"

#include <imgui.h>

namespace ImGuiApp
{
    Window::Window(const std::string& windowClass, Handle handle, const Settings& settings)
        : _handle(handle)
        , _windowClass(windowClass)
        , _settings{ settings }
    {
        const auto wndId = static_cast<std::uint32_t>(handle);
        _windowName = settings.title + "###" + windowClass + "_" + std::to_string(wndId);
    }

    void Window::setFocus()
    {
        _flags |= WindowFlag_TakeFocus;
    }

} // namespace ImGuiApp