#include "ImGuiApp/app.hpp"

#include "window.internal.hpp"

#define SDL_MAIN_HANDLED 1
#include <SDL.h>
#include <SDL_opengl.h>
#include <backends/imgui_impl_opengl2.h>
#include <backends/imgui_impl_sdl.h>
#include <mutex>

namespace ImGuiApp
{

    struct AppState
    {
        ImGuiIO& io;
        SDL_Window* window;
        std::vector<std::shared_ptr<Window>> windows;
        std::mutex queueLock;
    };

    struct AppEvent
    {
        std::uint32_t code;
        std::uintptr_t arg0;
        std::uintptr_t arg1;
        std::uintptr_t arg2;
        std::uintptr_t arg3;
    };

    static AppState& getState(void* appState)
    {
        return *static_cast<AppState*>(appState);
    }

    static bool initializeImGui(AppState& state)
    {
        auto& io = state.io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
                                                            // io.ConfigViewportsNoAutoMerge = true;
        // io.ConfigViewportsNoTaskBarIcon = true;
        return true;
    }

    static bool createHostWindow(AppState& state)
    {
        // Setup window
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

        SDL_WindowFlags window_flags
            = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SKIP_TASKBAR | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS);

        ImGuiIO io = state.io;

        SDL_Window* window = SDL_CreateWindow("Host Window", 0, 0, 0, 0, window_flags);
        if (window == nullptr)
        {
            return false;
        }
        SDL_SetWindowOpacity(window, 0.0f);

        SDL_GLContext gl_context = SDL_GL_CreateContext(window);
        SDL_GL_MakeCurrent(window, gl_context);

        // Enable adaptive vsync
        bool useVsync = false;
        if (useVsync)
        {
            if (SDL_GL_SetSwapInterval(-1) == -1)
            {
                // Fallback to ordinary vsync.
                SDL_GL_SetSwapInterval(1);
            }
        }
        else
        {
            SDL_GL_SetSwapInterval(0);
        }

        // Setup Platform/Renderer backends
        ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
        ImGui_ImplOpenGL2_Init();

        state.window = window;
        return true;
    }

    App::App()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        auto& io = ImGui::GetIO();
        _appState = new AppState{ io, nullptr };
    }

    App::~App()
    {
        ImGui_ImplOpenGL2_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        SDL_Quit();

        delete static_cast<AppState*>(_appState);
    }

    void App::exit()
    {
        SDL_Event event{};
        event.type = SDL_QUIT;

        SDL_PushEvent(&event);
    }

    std::shared_ptr<Window> App::getWindow(Window::Handle handle) const
    {
        auto& state = getState(_appState);

        auto it = std::find_if(state.windows.begin(), state.windows.end(), [handle](auto&& wnd) {
            return wnd->getHandle() == handle;
        });

        if (it == state.windows.end())
            return nullptr;

        return *it;
    }

    void App::setFocus(Window::Handle handle)
    {
        auto wnd = getWindow(handle);
        if (wnd == nullptr)
            return;
    }

    int App::run()
    {
        auto& state = getState(_appState);

        // Setup ImGui
        initializeImGui(state);

        // Setup window
        createHostWindow(state);

        auto nextRenderTime = std::chrono::high_resolution_clock::now();

        while (true)
        {
            bool forceRender = false;
            auto updateRate = 60;

            SDL_Event event{};
            while (SDL_PollEvent(&event) == 1)
            {
                ImGui_ImplSDL2_ProcessEvent(&event);

                if (event.type == SDL_QUIT)
                {
                    break;
                }

                forceRender = true;
                updateRate = 500;
            }

            // Start the Dear ImGui frame
            ImGui_ImplOpenGL2_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();

            for (auto& window : state.windows)
            {
                std::static_pointer_cast<WindowInternal>(window)->internalUpdate();
            }

            // Erase windows that are no longer open.
            state.windows.erase(
                std::remove_if(
                    state.windows.begin(), state.windows.end(),
                    [](const auto& wnd) {
                        return wnd->isClosed();
                    }),
                state.windows.end());

            // Rendering
            ImGui::Render();
            glViewport(0, 0, (int)state.io.DisplaySize.x, (int)state.io.DisplaySize.y);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

            if (state.io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
                SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
            }

            SDL_GL_SwapWindow(state.window);

            if (state.windows.empty())
                break;

            std::this_thread::sleep_for(std::chrono::milliseconds(1000 / updateRate));
        }

        return 0;
    }

    Window::Handle App::getNextWndHandle()
    {
        auto& state = getState(_appState);
        if (state.windows.empty())
        {
            return Window::Handle{ 1U };
        }
        else
        {
            const auto& last = state.windows.back();
            return Window::Handle{ static_cast<std::uint32_t>(last->getHandle()) + 1 };
        }
    }

    void App::createWindow_(std::shared_ptr<Window> wnd)
    {
        auto& state = getState(_appState);
        state.windows.push_back(wnd);
    }

} // namespace ImGuiApp