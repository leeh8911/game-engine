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
    gauri::Renderer2D::ResetStats();

    {
        GR_PROFILE_SCOPE("Renderer Prep");
        gauri::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
        gauri::RenderCommand::Clear();
    }

    {
        static float rotation = 0.0f;
        rotation += ts * 20.0f;

        GR_PROFILE_SCOPE("Renderer Draw");
        gauri::Renderer2D::BeginScene(m_CameraController.GetCamera());
        gauri::Renderer2D::DrawQuad({-1.0f, 0.0f}, {0.8f, 0.8f}, glm::radians(rotation), {0.8f, 0.2f, 0.3f, 1.0f});
        gauri::Renderer2D::DrawQuad({-1.0f, 0.0f}, {0.8f, 0.8f}, {0.8f, 0.2f, 0.3f, 1.0f});
        gauri::Renderer2D::DrawQuad({0.5f, -0.5f}, {0.5f, 0.75f}, {0.2f, 0.3f, 0.8f, 1.0f});
        gauri::Renderer2D::DrawQuad({0.0f, 0.0f, -0.1f}, {20.0f, 20.0f}, m_CheckerboardTexture, 10.0f);
        gauri::Renderer2D::DrawQuad({-0.5f, -0.5f, 0.0f}, {1.0f, 1.0f}, glm::radians(rotation), m_CheckerboardTexture,
                                    20.0f);
        gauri::Renderer2D::EndScene();

        gauri::Renderer2D::BeginScene(m_CameraController.GetCamera());
        for (float y = -5.0f; y < 5.0f; y += 0.5f)
        {
            for (float x = -5.0f; x < 5.0f; x += 0.5f)
            {
                glm::vec4 color = {(x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.5f};
                gauri::Renderer2D::DrawQuad({x, y}, {0.45f, 0.45f}, color);
            }
        }
        gauri::Renderer2D::EndScene();
    }
}

void Sandbox2D::OnImGuiRender()
{
    GR_PROFILE_FUNCTION();

    ImGui::Begin("Settings");

    auto stats = gauri::Renderer2D::GetStats();
    ImGui::Text("Renderer2D Stats:");
    ImGui::Text("Draw Calls: %d", stats.DrawCalls);
    ImGui::Text("Quads: %d", stats.QuadCount);
    ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
    m_ProfileResults.clear();
    ImGui::End();
}

void Sandbox2D::OnEvent(gauri::Event &event)
{
    m_CameraController.OnEvent(event);
}
