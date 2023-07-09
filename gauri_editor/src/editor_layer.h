#pragma once

#include "gauri.h"

namespace gauri
{
class EditorLayer : public Layer
{
  public:
    EditorLayer();
    ~EditorLayer() override = default;
    void OnAttach() override;
    void OnDetach() override;

    void OnUpdate(Timestep ts) override;
    void OnImGuiRender() override;
    void OnEvent(Event &event) override;

  private:
    OrthographicCameraController m_CameraController;

    // Temp
    Ref<VertexArray> m_SquareVA = nullptr;
    Ref<Shader> m_FlatColorShader = nullptr;
    Ref<FrameBuffer> m_FrameBuffer = nullptr;

    Ref<Scene> m_ActiveScene = nullptr;
    entt::entity m_SquareEntity;

    Ref<Texture2D> m_CheckerboardTexture = nullptr;

    bool m_ViewportFocused = false;
    bool m_ViewportHovered = false;
    glm::vec2 m_ViewportSize = {0.0f, 0.0f};

    glm::vec4 m_SquareColor = {0.2f, 0.3f, 0.8f, 1.0f};
};
} // namespace gauri
