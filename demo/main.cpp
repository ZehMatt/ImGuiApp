#include <ImGuiApp/app.hpp>
#include <cstdlib>
#include <iostream>

class TestWindow : public ImGuiApp::WindowBase<TestWindow>
{
    float f = 0.0f;
    int counter = 0;
    bool show_demo_window = false;
    bool show_another_window = false;
    bool temp = true;
    float clear_color = 0.0f;

public:
    using WindowBase::WindowBase;

    virtual void update() override
    {

        ImGui::Text("This is some useful text.");          // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window); // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text(
            "Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);
    }
};

class TestWindow2 : public ImGuiApp::WindowBase<TestWindow2>
{
    float f = 0.0f;
    int counter = 0;
    bool show_demo_window = false;
    bool show_another_window = false;
    bool temp = true;
    float clear_color = 0.0f;

public:
    using WindowBase::WindowBase;

    virtual void update() override
    {
        ImGui::Text("This is some useful text.");          // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window); // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        ImGui::SetNextWindowDockID(ImGui::GetWindowDockID(), ImGuiCond_Always);
        ImGui::Begin("Next2");
        ImGui::Button("Yup");
        ImGui::End();

        if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text(
            "Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);
    }
};

int main()
{
    ImGuiApp::App app;

    auto wnd1 = app.createWindow<TestWindow>(ImGuiApp::Window::Settings{ "hello world" });
    
    app.createWindow<TestWindow2>(ImGuiApp::Window::Settings{ "hello world 2" });

    wnd1->setFocus();
        
    return app.run();
}
