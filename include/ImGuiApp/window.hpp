#pragma once

#include <imgui.h>
#include <memory>
#include <string>
#include <vector>

namespace ImGuiApp
{
    class Window
    {
    public:
        enum class Handle : uint32_t
        {
            Invalid = 0xFFFFFFFF,
        };

        struct Settings
        {
            std::string title;
            const int width{ 480 };
            const int height{ 320 };
        };

        explicit Window(const std::string& windowClass, Handle handle, const Settings& settings);

        virtual ~Window() = default;

        Window(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(Window other) = delete;
        Window& operator=(Window&& other) = delete;

        virtual void update() = 0;

        Handle getHandle() const noexcept
        {
            return _handle;
        }

        bool isClosed() const noexcept
        {
            return !_isOpen;
        }

        void setFocus();

    protected:
        Handle _handle{ Handle::Invalid };
        std::string _windowClass;
        std::string _windowName;
        std::uint32_t _flags{};
        Settings _settings;
        bool _isOpen{ true };
    };

    namespace Detail
    {
        template<typename T> inline const char* getWindowClass()
        {
            const auto& typeInfo = typeid(T);
            return typeInfo.name();
        }
    } // namespace Detail

    template<typename T> class WindowBase : public Window
    {
    public:
        template<typename... TArgs>
        WindowBase(Window::Handle handle, const Settings& settings)
            : Window(Detail::getWindowClass<T>(), handle, settings)
        {
        }
    };

} // namespace ImGuiApp
