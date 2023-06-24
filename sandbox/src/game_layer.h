#pragma once

#include "gauri.h"

#include "imgui.h"

#include "level.h"

class Level;

class GameLayer : public gauri::Layer
{
  public:
    GameLayer();
    ~GameLayer() override = default;

    void OnAttach() override;
    void OnDetach() override;

    void OnUpdate(gauri::Timestep ts) override;
    void OnImGuiRender() override;
    void OnEvent(gauri::Event & /*event*/) override;

    bool OnMouseButtonPressed(gauri::MouseButtonPressedEvent &e);
    bool OnWindowResize(gauri::WindowResizeEvent &e);

  private:
    void CreateCamera(uint32_t width, uint32_t height);

    gauri::Scope<gauri::OrthographicCamera> m_Camera{nullptr};

    Level m_Level;
    ImFont *m_Font;
    float m_Time = 0.0f;
    bool m_Blink = false;

    enum class GameState
    {
        Play = 0,
        MainMenu,
        GameOver
    };
    GameState m_State = GameState::MainMenu;
};
