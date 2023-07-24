#pragma once

#include "gauri.h"

#include "gauri_editor/panels/scene_hierarchy_panel.h"

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
    bool OnKeyPressed(KeyPressedEvent &e);

    void NewScene();
    void OpenScene();
    void SaveSceneAs();

    OrthographicCameraController m_CameraController;

    // Temp
    Ref<VertexArray> m_SquareVA = nullptr;
    Ref<Shader> m_FlatColorShader = nullptr;
    Ref<FrameBuffer> m_FrameBuffer = nullptr;

    Ref<Scene> m_ActiveScene = nullptr;
    Entity m_SquareEntity;
    Entity m_CameraEntity;
    Entity m_SecondCamera;

    bool m_PrimaryCamera = true;

    Ref<Texture2D> m_CheckerboardTexture = nullptr;

    bool m_ViewportFocused = false;
    bool m_ViewportHovered = false;
    glm::vec2 m_ViewportSize = {0.0f, 0.0f};

    glm::vec4 m_SquareColor = {0.2f, 0.3f, 0.8f, 1.0f};

    // Panels
    SceneHierarchyPanel m_SceneHierarchyPanel;
};
} // namespace gauri
