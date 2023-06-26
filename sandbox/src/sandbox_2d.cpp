#include "sandbox_2d.h"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"

Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_CameraController(1280.0f / 720.f)
{
}

void Sandbox2D::OnAttach()
{
    GR_PROFILE_FUNCTION();

    m_CheckerboardTexture = gauri::Texture2D::Create("assets/textures/Checkerboard.png");
}

void Sandbox2D::OnDetach()
{
    GR_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(gauri::Timestep ts)
{
    GR_PROFILE_FUNCTION();

    // Update
    m_CameraController.OnUpdate(ts);

    // Render
    {
        GR_PROFILE_SCOPE("Renderer Prep");
        gauri::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
        gauri::RenderCommand::Clear();
    }

    {
        GR_PROFILE_SCOPE("Renderer Draw");
        gauri::Renderer2D::BeginScene(m_CameraController.GetCamera());
        gauri::Renderer2D::DrawQuad({-1.0f, 0.0f}, {0.8f, 0.8f}, glm::radians(-45.0f), {0.8f, 0.2f, 0.3f, 1.0f});
        gauri::Renderer2D::DrawQuad({0.5f, -0.5f}, {0.5f, 0.75f}, {0.2f, 0.3f, 0.8f, 1.0f});
        gauri::Renderer2D::DrawQuad({0.0f, 0.0f, -0.1f}, {10.0f, 10.0f}, m_CheckerboardTexture, 10.0f);
        gauri::Renderer2D::EndScene();
    }
}

void Sandbox2D::OnImGuiRender()
{
    GR_PROFILE_FUNCTION();

    ImGui::Begin("Settings");
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
    m_ProfileResults.clear();

    ImGui::End();
}

void Sandbox2D::OnEvent(gauri::Event &event)
{
    m_CameraController.OnEvent(event);
}
