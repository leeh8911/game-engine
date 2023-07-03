#include "sandbox_2d.h"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"

static const uint32_t s_MapWidth = 24;
// clang-format off
static const char *s_MapTiles = 
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWDDDDDDWWWWWWWWWWW"
"WWWWWDDDDDDDDDDDWWWWWWWW"
"WWWWDDDDDDDDDDDDDDWWWWWW"
"WWWDDDDDDDDDDDDDDDDDWWWW"
"WWDDDDwwwDDDDDDDDDDDDWWW"
"WDDDDDwwwDDDDDDDDDDDDDWW"
"WWDDDDDDDDDDDDDDDDDDDWWW"
"WWWWDDDDDDDDDDDDDDDDWWWW"
"WWWWWWDDDDDDDDDDDDDWWWWW"
"WWWWWWWWDDDDDDDDDDWWWWWW"
"WWWWWWWWWWDDDDDDDWWWWWWW"
"WWWWWWWWWWWWWDDDWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW";
// clang-format on
Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_CameraController(1280.0f / 720.f)
{
}

void Sandbox2D::OnAttach()
{
    GR_PROFILE_FUNCTION();

    m_CheckerboardTexture = gauri::Texture2D::Create("assets/textures/Checkerboard.png");
    m_SpriteSheet = gauri::Texture2D::Create("assets/game/textures/RPGpack_sheet_2X.png");

    s_TextureMap['D'] = gauri::SubTexture2D::CreateFromCoords(m_SpriteSheet, {6, 11}, {128, 128});
    s_TextureMap['W'] = gauri::SubTexture2D::CreateFromCoords(m_SpriteSheet, {11, 11}, {128, 128});

    m_MapWidth = s_MapWidth;
    m_MapHeight = strlen(s_MapTiles) / s_MapWidth;

    m_TextureStairs = gauri::SubTexture2D::CreateFromCoords(m_SpriteSheet, {0, 11}, {128, 128});
    m_TextureTree = gauri::SubTexture2D::CreateFromCoords(m_SpriteSheet, {2, 1}, {128, 128}, {1, 2});

    m_Particle.ColorBegin = {254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f};
    m_Particle.ColorEnd = {254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f};
    m_Particle.SizeBegin = 0.5f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
    m_Particle.LifeTime = 5.0f;
    m_Particle.Velocity = {0.0f, 0.0f};
    m_Particle.VelocityVariation = {3.0f, 1.0f};
    m_Particle.Position = {0.0f, 0.0f};

    m_CameraController.SetZoomLevel(5.0f);
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

#if (0)
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
#endif

    if (gauri::Input::IsMouseButtonPressed(GR_MOUSE_BUTTON_LEFT))
    {
        auto [x, y] = gauri::Input::GetMousePosition();
        auto width = gauri::Application::Get().GetWindow().GetWidth();
        auto height = gauri::Application::Get().GetWindow().GetHeight();

        auto bounds = m_CameraController.GetBounds();
        auto pos = m_CameraController.GetCamera().GetPosition();
        x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
        y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
        m_Particle.Position = {x + pos.x, y + pos.y};
        for (int i = 0; i < 50; i++)
            m_ParticleSystem.Emit(m_Particle);
    }

    m_ParticleSystem.OnUpdate(ts);
    m_ParticleSystem.OnRender(m_CameraController.GetCamera());

    gauri::Renderer2D::BeginScene(m_CameraController.GetCamera());
    for (uint32_t y = 0; y < m_MapHeight; ++y)
    {
        for (uint32_t x = 0; x < m_MapWidth; ++x)
        {
            char tileType = s_MapTiles[x + y * m_MapWidth];
            gauri::Ref<gauri::SubTexture2D> texture;
            if (s_TextureMap.find(tileType) != s_TextureMap.end())
            {
                texture = s_TextureMap[tileType];
            }
            else
            {
                texture = m_TextureStairs;
            }
            gauri::Renderer2D::DrawQuad({x - m_MapWidth / 2.0f, y - m_MapHeight / 2.0f, 0.5f}, {1.0f, 1.0f}, texture);
        }
    }
    gauri::Renderer2D::EndScene();
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
