#include "game_layer.h"

#include <string>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include "gauri.h"

#include "level.h"
#include "random.h"

GameLayer::GameLayer() : Layer("GameLayer")
{
    auto &window = gauri::Application::Get().GetWindow();
    CreateCamera(window.GetWidth(), window.GetHeight());

    Random::Init();
}

void GameLayer::OnAttach()
{
    m_Level.Init();

    ImGuiIO io = ImGui::GetIO();
    m_Font = io.Fonts->AddFontFromFileTTF("assets/OpenSans-Regular.ttf", 120.0f);
}

void GameLayer::OnDetach()
{
}

void GameLayer::OnUpdate(gauri::Timestep ts)
{
    m_Time += ts;
    if ((int)(m_Time * 10.0f) % 8 > 4)
    {
        m_Blink = !m_Blink;
    }

    if (m_Level.IsGameOver())
    {
        m_State = GameState::GameOver;
    }

    const auto &playerPos = m_Level.GetPlayer().GetPosition();
    m_Camera->SetPosition({playerPos.x, playerPos.y, 0.0f});

    switch (m_State)
    {
    case GameState::Play:
        m_Level.OnUpdate(ts);
        break;
    }

    gauri::RenderCommand::SetClearColor({0.0f, 0.0f, 0.0f, 1.0f});
    gauri::RenderCommand::Clear();

    gauri::Renderer2D::BeginScene(*m_Camera);
    m_Level.OnRender();
    gauri::Renderer2D::EndScene();
}

void GameLayer::OnImGuiRender()
{
    // ImGui::Begin("Settings");
    // m_Level.OnImGuiRender();
    // ImGui::End();

    // UI?

    switch (m_State)
    {
    case GameState::Play:
        uint32_t playerScore = m_Level.GetPlayer().GetScore();
        std::string scoreStr = std::string("Score: ") + std::to_string(playerScore);
        ImGui::GetForegroundDrawList()->AddText(m_Font, 48.0f, ImGui::GetWindowPos(), 0xffffffff, scoreStr.c_str());
        break;
    case GameState::MainMenu:
        break;
    case GameState::GameOver:
        break;
    }
}

void GameLayer::OnEvent(gauri::Event &)
{
}

bool GameLayer::OnMouseButtonPressed(gauri::MouseButtonPressedEvent &e)
{
    if (m_State == GameState::GameOver)
    {
        m_Level.Reset();
    }

    m_State = GameState::Play;
    return false;
}
bool GameLayer::OnWindowResize(gauri::WindowResizeEvent &e)
{
    CreateCamera(e.GetWidth(), e.GetHeight());
    return false;
}

void GameLayer::CreateCamera(uint32_t width, uint32_t height)
{
    float aspectRatio = (float)width / (float)height;

    float camWidth = 8.0f;
    float bottom = -camWidth;
    float top = camWidth;
    float left = bottom * aspectRatio;
    float right = top * aspectRatio;
    m_Camera = gauri::CreateScope<gauri::OrthographicCamera>(left, right, bottom, top);
}
