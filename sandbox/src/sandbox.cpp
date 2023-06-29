#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gauri.h"
#include "gauri/core/entry_point.h"
#include "imgui.h"

// TEMP
#include "platform/opengl/opengl_shader.h"

#include "sandbox_2d.h"

class ExampleLayer : public gauri::Layer
{
  public:
    ExampleLayer() : Layer("Example"), m_CameraController(1280.0f / 720.f)
    {
        m_VertexArray = gauri::VertexArray::Create();

        // clang-format off
        // triangle {x, y, z}
        float vertices[3 * 7] = {-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
                                  0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f, 
                                  0.0f, 0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f};
        // clang-format on

        gauri::Ref<gauri::VertexBuffer> vertexBuffer = gauri::VertexBuffer::Create(vertices, sizeof(vertices));

        gauri::BufferLayout layout = {{gauri::ShaderDataType::Float3, "a_Position"},
                                      {gauri::ShaderDataType::Float4, "a_Color"}};
        vertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(vertexBuffer);

        uint32_t indices[3] = {0, 1, 2};
        gauri::Ref<gauri::IndexBuffer> indexBuffer =
            gauri::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
        m_VertexArray->SetIndexBuffer(indexBuffer);

        m_SquareVA = gauri::VertexArray::Create();

        // clang-format off
        // triangle {x, y, z}
        float squareVertices[5 * 4] = {-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
                                        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
                                        0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
                                       -0.5f, 0.5f, 0.0f, 0.0f, 1.0f};
        // clang-format on
        gauri::Ref<gauri::VertexBuffer> squareVB = gauri::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
        squareVB->SetLayout(
            {{gauri::ShaderDataType::Float3, "a_Position"}, {gauri::ShaderDataType::Float2, "a_TexCoord"}});
        m_SquareVA->AddVertexBuffer(squareVB);

        uint32_t squareIndices[6] = {0, 1, 2, 2, 3, 0};
        gauri::Ref<gauri::IndexBuffer> suqareIB =
            gauri::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
        m_SquareVA->SetIndexBuffer(suqareIB);

        std::string vertexSrc = R"(
        #version 330 core
        
        layout(location = 0) in vec3 a_Position;        
        layout(location = 1) in vec4 a_Color;        
        
        uniform mat4 u_ViewProjection;
        uniform mat4 u_Transform;
        
        out vec3 v_Position;
        out vec4 v_Color;

        void main()
        {
            v_Position = a_Position;
            v_Color = a_Color;
            gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
        }
    )";
        std::string framgmentSrc = R"(
        #version 330 core
        
        layout(location = 0) out vec4 color;

        in vec3 v_Position;
        in vec4 v_Color;

        void main()
        {
            color = v_Color;
        }
    )";

        m_Shader = gauri::Shader::Create("VertexPosColor", vertexSrc, framgmentSrc);

        std::string flatColorShaderVertexSrc = R"(
        #version 330 core
        
        layout(location = 0) in vec3 a_Position;        
        
        uniform mat4 u_ViewProjection;
        uniform mat4 u_Transform;

        out vec3 v_Position;

        void main()
        {
            v_Position = a_Position;
            gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
        }
    )";
        std::string flatColorShaderFragmentSrc = R"(
        #version 330 core
        
        layout(location = 0) out vec4 color;

        in vec3 v_Position;

        uniform vec3 u_Color;

        void main()
        {
            color = vec4(u_Color, 1.0);
        }
    )";

        m_FlatColorShader = gauri::Shader::Create("FlatColor", flatColorShaderVertexSrc, flatColorShaderFragmentSrc);

        auto textureShader = m_ShaderLibrary.Load("assets/shaders/texture.glsl");

        m_Texture = gauri::Texture2D::Create("assets/textures/Checkerboard.png");
        m_ChernoLogoTexture = gauri::Texture2D::Create("assets/textures/ChernoLogo.png");

        std::dynamic_pointer_cast<gauri::OpenGLShader>(textureShader)->Bind();
        std::dynamic_pointer_cast<gauri::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);
    }

    void OnUpdate(gauri::Timestep ts) override
    {
        // Update
        m_CameraController.OnUpdate(ts);

        // Render
        gauri::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
        gauri::RenderCommand::Clear();

        gauri::Renderer::BeginScene(m_CameraController.GetCamera());

        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

        std::dynamic_pointer_cast<gauri::OpenGLShader>(m_FlatColorShader)->Bind();
        std::dynamic_pointer_cast<gauri::OpenGLShader>(m_FlatColorShader)
            ->UploadUniformFloat3("u_Color", m_SquareColor);
        for (int y = 0; y < 20; ++y)
        {
            for (int x = 0; x < 20; ++x)
            {
                glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
                glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
                gauri::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
            }
        }
        auto textureShader = m_ShaderLibrary.Get("texture");

        m_Texture->Bind();
        gauri::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
        m_ChernoLogoTexture->Bind();
        gauri::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

        // Triangle
        // gauri::Renderer::Submit(m_Shader, m_VertexArray);

        gauri::Renderer::EndScene();
    }

    void OnEvent(gauri::Event &event) override
    {
        m_CameraController.OnEvent(event);
    }

    void OnImGuiRender() override
    {
        ImGui::Begin("Settings");
        ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
        ImGui::End();
    }

  private:
    gauri::ShaderLibrary m_ShaderLibrary{};
    gauri::Ref<gauri::Shader> m_Shader = nullptr;
    gauri::Ref<gauri::VertexArray> m_VertexArray = nullptr;

    gauri::Ref<gauri::Shader> m_FlatColorShader = nullptr;
    gauri::Ref<gauri::VertexArray> m_SquareVA = nullptr;

    gauri::Ref<gauri::Texture2D> m_Texture = nullptr, m_ChernoLogoTexture = nullptr;

    gauri::OrthographicCameraController m_CameraController;

    float m_SquareSpeed = 1.0f;

    glm::vec3 m_SquareColor = {0.2f, 0.3f, 0.8f};
};

class Sandbox : public gauri::Application
{
  public:
    Sandbox()
    {
        // PushLayer(new ExampleLayer());
        PushLayer(new Sandbox2D());
    }
    ~Sandbox() override = default;

  private:
};

gauri::Application *gauri::CreateApplication()
{
    return new Sandbox();
}
