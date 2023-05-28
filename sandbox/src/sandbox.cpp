
#include <iostream>

#include "gauri.h"

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
};
class Sandbox : public gauri::Application
{
  public:
    Sandbox()
    {
        PushLayer(new ExampleLayer());
        PushOverlay(new gauri::ImGuiLayer());
    }
    ~Sandbox() override = default;

  private:
};

gauri::Application *gauri::CreateApplication()
{
    return new Sandbox();
}
