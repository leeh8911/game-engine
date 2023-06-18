#include "sandbox_2d.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "platform/opengl/opengl_shader.h"

Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_CameraController(1280.0f / 720.f)
{
}

void Sandbox2D::OnAttach()
{
    m_SquareVA = gauri::VertexArray::Create();

    // clang-format off
        // triangle {x, y, z}
        float squareVertices[5 * 4] = {-0.5f, -0.5f, 0.0f,
                                        0.5f, -0.5f, 0.0f,
                                        0.5f, 0.5f, 0.0f,
                                       -0.5f, 0.5f, 0.0f};
    // clang-format on
    gauri::Ref<gauri::VertexBuffer> squareVB;
    squareVB.reset(gauri::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
    squareVB->SetLayout({{gauri::ShaderDataType::Float3, "a_Position"}});
    m_SquareVA->AddVertexBuffer(squareVB);

    uint32_t squareIndices[6] = {0, 1, 2, 2, 3, 0};
    gauri::Ref<gauri::IndexBuffer> suqareIB;
    suqareIB.reset(gauri::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
    m_SquareVA->SetIndexBuffer(suqareIB);

    m_FlatColorShader = gauri::Shader::Create("assets/shaders/flat_color.glsl");
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

    gauri::Renderer::BeginScene(m_CameraController.GetCamera());

    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

    std::dynamic_pointer_cast<gauri::OpenGLShader>(m_FlatColorShader)->Bind();
    std::dynamic_pointer_cast<gauri::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("u_Color", m_SquareColor);

    gauri::Renderer::Submit(m_FlatColorShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

    gauri::Renderer::EndScene();
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
