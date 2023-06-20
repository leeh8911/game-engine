#include "sandbox_2d.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"

Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_CameraController(1280.0f / 720.f)
{
}

void Sandbox2D::OnAttach()
{
}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(gauri::Timestep ts)
{
    // Update
    m_CameraController.OnUpdate(ts);

    // Render
    gauri::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
    gauri::RenderCommand::Clear();

    gauri::Renderer2D::BeginScene(m_CameraController.GetCamera());
    gauri::Renderer2D::DrawQuad({-1.0f, 0.0f}, {0.8f, 0.8f}, {0.8f, 0.2f, 0.3f, 1.0f});
    gauri::Renderer2D::DrawQuad({0.5f, -0.5f}, {0.5f, 0.75f}, {0.2f, 0.3f, 0.8f, 1.0f});
    gauri::Renderer2D::EndScene();
}

void Sandbox2D::OnImGuiRender()
{
    ImGui::Begin("Settings");
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
    ImGui::End();
}

void Sandbox2D::OnEvent(gauri::Event &event)
{
    m_CameraController.OnEvent(event);
}
