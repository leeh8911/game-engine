
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
        // TheCherno�� Hazel�� ��� DLL�̶� ������ ������ �߻��ؼ� �ذ��ؾ� ������, gauri�� ��� StaticLib�̶� ������ ������ �߻����� �ʴ´�.
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
