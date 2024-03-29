#include "gauri_editor/editor_layer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include "ImGuizmo.h"

#include "gauri/math/math.h"
#include "gauri/scene/scene_serializer.h"
#include "gauri/utils/platform_utils.h"

namespace gauri
{
EditorLayer::EditorLayer() : Layer("EditorLayer"), m_CameraController(1280.0f / 720.f)
{
}

void EditorLayer::OnAttach()
{
    GR_PROFILE_FUNCTION();

    m_CheckerboardTexture = Texture2D::Create("assets/textures/Checkerboard.png");

    FrameBufferSpecification fbSpec;
    fbSpec.Attachments = {FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER,
                          FrameBufferTextureFormat::Depth};
    fbSpec.Width = 1280;
    fbSpec.Height = 720;
    m_FrameBuffer = FrameBuffer::Create(fbSpec);

    m_ActiveScene = CreateRef<Scene>();

    m_SceneHierarchyPanel.SetContext(m_ActiveScene);

    m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
}

void EditorLayer::OnDetach()
{
    GR_PROFILE_FUNCTION();
}

void EditorLayer::OnUpdate(Timestep ts)
{
    GR_PROFILE_FUNCTION();

    // Resize
    if (FrameBufferSpecification spec = m_FrameBuffer->GetSpecification();
        m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
        (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
    {
        m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
        m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
        m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
    }

    // Update
    if (m_ViewportFocused)
    {
        m_CameraController.OnUpdate(ts);
    }
    m_EditorCamera.OnUpdate(ts);

    // Render
    Renderer2D::ResetStats();
    m_FrameBuffer->Bind();
    RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
    RenderCommand::Clear();

    // Update scene
    m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);

    auto [mx, my] = ImGui::GetMousePos();
    mx -= m_ViewportBounds[0].x;
    my -= m_ViewportBounds[0].y;
    glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
    my = viewportSize.y - my;
    int mouseX = (int)mx;
    int mouseY = (int)my;

    if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
    {
        int pixelData = m_FrameBuffer->ReadPixel(1, mouseX, mouseY);
        GR_CORE_WARN("pixel Data = {0}", pixelData);
    }

    m_FrameBuffer->Unbind();
}

void EditorLayer::OnImGuiRender()
{
    GR_PROFILE_FUNCTION();

    static bool dockspaceOpen = true;
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    else
    {
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
    if (!opt_padding)
        ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO &io = ImGui::GetIO();
    ImGuiStyle &style = ImGui::GetStyle();
    float minWinSizeX = style.WindowMinSize.x;
    style.WindowMinSize.x = 370.0f;
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    style.WindowMinSize.x = minWinSizeX;

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            // Disabling fullscreen would allow the window to be moved to the front of other windows,
            // which we can't undo at the moment without finer window depth/z control.
            if (ImGui::MenuItem("New", "Ctrl+N"))
            {
                NewScene();
            }

            if (ImGui::MenuItem("Open...", "Ctrl+O"))
            {
                OpenScene();
            }
            if (ImGui::MenuItem("Save As...", "Ctrl+S"))
            {
                SaveSceneAs();
            }

            if (ImGui::MenuItem("Exit"))
            {
                Application::Get().Close();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    m_SceneHierarchyPanel.OnImGuiRender();

    ImGui::Begin("Stats");

    auto stats = Renderer2D::GetStats();
    ImGui::Text("Renderer2D Stats:");
    ImGui::Text("Draw Calls: %d", stats.DrawCalls);
    ImGui::Text("Quads: %d", stats.QuadCount);
    ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ImGui::Begin("Viewport");
    auto viewportOffset = ImGui::GetCursorPos(); // Includes tab bar

    m_ViewportFocused = ImGui::IsWindowFocused();
    m_ViewportHovered = ImGui::IsWindowHovered();
    Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    m_ViewportSize = {viewportPanelSize.x, viewportPanelSize.y};

    uint32_t textureID = m_FrameBuffer->GetColorAttachmentRendererID();
    ImGui::Image(reinterpret_cast<void *>(textureID), ImVec2{m_ViewportSize.x, m_ViewportSize.y}, ImVec2{0, 1},
                 ImVec2{1, 0});

    auto windowSize = ImGui::GetWindowSize();
    ImVec2 minBound = ImGui::GetWindowPos();
    minBound.x += viewportOffset.x;
    minBound.y += viewportOffset.y;

    ImVec2 maxBound = {minBound.x + windowSize.x, minBound.y + windowSize.y};
    m_ViewportBounds[0] = {minBound.x, minBound.y};
    m_ViewportBounds[1] = {maxBound.x, maxBound.y};

    // Gizmos
    Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
    if (selectedEntity && m_GuizmoType != -1)
    {
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();

        float windowWidth = (float)ImGui::GetWindowWidth();
        float windowHeight = (float)ImGui::GetWindowHeight();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

        // Camera
        // Runtime camera from entity
        // auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
        // const auto &camera = cameraEntity.GetComponent<CameraComponent>().Camera;
        // const glm::mat4 &cameraProjection = camera.GetProjection();
        // glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());

        // Editor camera
        const glm::mat4 &cameraProjection = m_EditorCamera.GetProjection();
        glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

        // Entity transform
        auto &tc = selectedEntity.GetComponent<TransformComponent>();
        glm::mat4 transform = tc.GetTransform();

        // Snapping
        bool snap = Input::IsKeyPressed(GR_KEY_LEFT_CONTROL);
        float snapValue = 0.5f; // Snap to 0.5m for translation and scale
        // Snap to 45 degrees for rotation
        if (m_GuizmoType == ImGuizmo::OPERATION::ROTATE)
        {
            snapValue = 45.0f;
        }
        float snapValues[3] = {snapValue, snapValue, snapValue};

        ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
                             static_cast<ImGuizmo::OPERATION>(m_GuizmoType), ImGuizmo::LOCAL, glm::value_ptr(transform),
                             nullptr, snap ? snapValues : nullptr);

        if (ImGuizmo::IsUsing())
        {
            glm::vec3 translation, rotation, scale;
            math::DecomposeTransform(transform, translation, rotation, scale);

            glm::vec3 deltaRotation = rotation - tc.Rotation;

            tc.Translation = translation;
            tc.Rotation += deltaRotation;
            tc.Scale = scale;
        }
    }

    ImGui::End();
    ImGui::PopStyleVar();

    ImGui::End();
}

void EditorLayer::OnEvent(Event &event)
{
    m_CameraController.OnEvent(event);
    m_EditorCamera.OnEvent(event);

    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>(GR_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
}

bool EditorLayer::OnKeyPressed(KeyPressedEvent &e)
{
    // Shortcuts
    if (e.GetRepeatCount() > 0)
    {
        return false;
    }

    bool control = Input::IsKeyPressed(GR_KEY_LEFT_CONTROL) || Input::IsKeyPressed(GR_KEY_RIGHT_CONTROL);

    bool shift = Input::IsKeyPressed(GR_KEY_LEFT_SHIFT) || Input::IsKeyPressed(GR_KEY_RIGHT_SHIFT);

    switch (e.GetKeyCode())
    {
    case GR_KEY_S: {
        if (control)
        {
            SaveSceneAs();
            return true;
        }
        break;
    }
    case GR_KEY_O: {
        if (control)
        {
            OpenScene();
            return true;
        }
        break;
    }
    case GR_KEY_N: {
        if (control)
        {
            NewScene();
            return true;
        }
        break;
    }

        // Gizmos
    case GR_KEY_Q:
        m_GuizmoType = -1;
        break;
    case GR_KEY_W:
        m_GuizmoType = static_cast<int>(ImGuizmo::OPERATION::TRANSLATE);
        break;
    case GR_KEY_E:
        m_GuizmoType = static_cast<int>(ImGuizmo::OPERATION::ROTATE);
        break;
    case GR_KEY_R:
        m_GuizmoType = static_cast<int>(ImGuizmo::OPERATION::SCALE);
        break;
    }
    return false;
}

void EditorLayer::NewScene()
{
    m_ActiveScene = CreateRef<Scene>();
    m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
    m_SceneHierarchyPanel.SetContext(m_ActiveScene);
}

void EditorLayer::OpenScene()
{
    std::string filepath = FileDialogs::OpenFile("Gauri Scene (*.gauri)\0*.gauri\0");
    if (!filepath.empty())
    {
        m_ActiveScene = CreateRef<Scene>();
        m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);

        SceneSerializer serializer(m_ActiveScene);
        serializer.Deserialize(filepath);
    }
}

void EditorLayer::SaveSceneAs()
{
    std::string filepath = FileDialogs::SaveFile("Gauri Scene (*.gauri)\0*.gauri\0");
    if (!filepath.empty())
    {
        SceneSerializer serializer(m_ActiveScene);
        serializer.Serialize(filepath);
    }
}

} // namespace gauri
