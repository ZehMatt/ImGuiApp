#pragma once

#include "window.hpp"

#include <memory>

namespace ImGuiApp
{
    class App
    {
        struct AppState* _appState{};

    public:
        App();
        ~App();

        void exit();

        template<typename T> std::shared_ptr<T> createWindow(const Window::Settings& settings)
        {
            auto nextHandle = getNextWndHandle();
            auto newWnd = std::make_shared<T>(nextHandle, settings);
            createWindow_(newWnd);
            return newWnd;
        }

        std::shared_ptr<Window> getWindow(Window::Handle handle) const;

        void setFocus(Window::Handle handle);

        int run();

    private:
        Window::Handle getNextWndHandle();

        void createWindow_(std::shared_ptr<Window> wnd);
    };

} // namespace ImGuiApp