
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
        GR_INFO("ExampleLayer::OnUpdate");
    }

    void OnEvent(gauri::Event &event) override
    {
        GR_TRACE("ExampleLayer::OnEvent: {0}", event.ToString());
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
