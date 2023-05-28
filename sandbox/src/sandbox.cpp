
#include <iostream>

#include "gauri.h"
#include "imgui.h"

class ExampleLayer : public gauri::Layer
{
  public:
    ExampleLayer() : Layer("Example")
    {
    }

    void OnUpdate() override
    {
        if (gauri::Input::IsKeyPressed(GR_KEY_TAB))
        {
            GR_INFO("Tab key is pressed (poll)!");
        }
    }

    void OnEvent(gauri::Event &event) override
    {
        if (event.GetEventType() == gauri::EventType::KeyPressed)
        {
            gauri::KeyPressedEvent &e = static_cast<gauri::KeyPressedEvent &>(event);
            if (e.GetKeyCode() == GR_KEY_TAB)
            {
                GR_INFO("Tab key is pressed (event)!");
            }
            GR_TRACE("{0}", (char)e.GetKeyCode());
        }
    }

    void OnImGuiRender() override
    {
        // clang-format off
        // TheCherno의 Hazel의 경우 DLL이라 설정의 문제가 발생해서 해결해야 했지만, gauri의 경우 StaticLib이라 설정의 문제가 발생하지 않는다.
        // clang-format on

        ImGui::Begin("Test");
        ImGui::Text("Hello World!");
        ImGui::End();
    }
};
class Sandbox : public gauri::Application
{
  public:
    Sandbox()
    {
        PushLayer(new ExampleLayer());
    }
    ~Sandbox() override = default;

  private:
};

gauri::Application *gauri::CreateApplication()
{
    return new Sandbox();
}
