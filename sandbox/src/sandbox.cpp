#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gauri.h"
#include "imgui.h"

// TEMP
#include "platform/opengl/opengl_shader.h"

class ExampleLayer : public gauri::Layer
{
  public:
    ExampleLayer() : Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f)
    {
        m_VertexArray.reset(gauri::VertexArray::Create());

        // clang-format off
        // triangle {x, y, z}
        float vertices[3 * 7] = {-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
                                  0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f, 
                                  0.0f, 0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f};
        // clang-format on

        gauri::Ref<gauri::VertexBuffer> vertexBuffer;
        vertexBuffer.reset(gauri::VertexBuffer::Create(vertices, sizeof(vertices)));

        gauri::BufferLayout layout = {{gauri::ShaderDataType::Float3, "a_Position"},
                                      {gauri::ShaderDataType::Float4, "a_Color"}};
        vertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(vertexBuffer);

        uint32_t indices[3] = {0, 1, 2};
        gauri::Ref<gauri::IndexBuffer> indexBuffer;
        indexBuffer.reset(gauri::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
        m_VertexArray->SetIndexBuffer(indexBuffer);

        m_SquareVA.reset(gauri::VertexArray::Create());

        // clang-format off
        // triangle {x, y, z}
        float squareVertices[5 * 4] = {-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
                                        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
                                        0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
                                       -0.5f, 0.5f, 0.0f, 0.0f, 1.0f};
        // clang-format on
        gauri::Ref<gauri::VertexBuffer> squareVB;
        squareVB.reset(gauri::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
        squareVB->SetLayout(
            {{gauri::ShaderDataType::Float3, "a_Position"}, {gauri::ShaderDataType::Float2, "a_TexCoord"}});
        m_SquareVA->AddVertexBuffer(squareVB);

        uint32_t squareIndices[6] = {0, 1, 2, 2, 3, 0};
        gauri::Ref<gauri::IndexBuffer> suqareIB;
        suqareIB.reset(gauri::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
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

        m_Shader.reset(gauri::Shader::Create(vertexSrc, framgmentSrc));

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

        m_FlatColorShader.reset(gauri::Shader::Create(flatColorShaderVertexSrc, flatColorShaderFragmentSrc));

        std::string textureShaderVertexSrc = R"(
        #version 330 core
        
        layout(location = 0) in vec3 a_Position;        
        layout(location = 1) in vec2 a_TexCoord;        
        
        uniform mat4 u_ViewProjection;
        uniform mat4 u_Transform;

        out vec2 v_TexCoord;

        void main()
        {
            v_TexCoord = a_TexCoord;
            gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
        }
    )";
        std::string textureShaderFragmentSrc = R"(
        #version 330 core
        
        layout(location = 0) out vec4 color;

        in vec2 v_TexCoord;

        uniform sampler2D u_Texture;

        void main()
        {
            color = texture(u_Texture, v_TexCoord);
        }
    )";

        m_TextureShader.reset(gauri::Shader::Create(textureShaderVertexSrc, textureShaderFragmentSrc));

        m_Texture = gauri::Texture2D::Create("assets/textures/Checkerboard.png");

        std::dynamic_pointer_cast<gauri::OpenGLShader>(m_TextureShader)->Bind();
        std::dynamic_pointer_cast<gauri::OpenGLShader>(m_TextureShader)->UploadUniformInt("u_Texture", 0);
    }

    void OnUpdate(gauri::Timestep ts) override
    {
        if (gauri::Input::IsKeyPressed(GR_KEY_LEFT))
        {
            m_CameraPosition.x -= m_CameraMoveSpeed * ts;
        }
        else if (gauri::Input::IsKeyPressed(GR_KEY_RIGHT))
        {
            m_CameraPosition.x += m_CameraMoveSpeed * ts;
        }

        if (gauri::Input::IsKeyPressed(GR_KEY_UP))
        {
            m_CameraPosition.y += m_CameraMoveSpeed * ts;
        }
        else if (gauri::Input::IsKeyPressed(GR_KEY_DOWN))
        {
            m_CameraPosition.y -= m_CameraMoveSpeed * ts;
        }

        if (gauri::Input::IsKeyPressed(GR_KEY_A))
        {
            m_CameraRotation += m_CameraRotationSpeed * ts;
        }
        else if (gauri::Input::IsKeyPressed(GR_KEY_D))
        {
            m_CameraRotation -= m_CameraRotationSpeed * ts;
        }

        gauri::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
        gauri::RenderCommand::Clear();

        m_Camera.SetPosition(m_CameraPosition);
        m_Camera.SetRotation(m_CameraRotation);

        gauri::Renderer::BeginScene(m_Camera);

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
        m_Texture->Bind();
        gauri::Renderer::Submit(m_TextureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

        // Triangle
        // gauri::Renderer::Submit(m_Shader, m_VertexArray);

        gauri::Renderer::EndScene();
    }

    void OnEvent(gauri::Event &event) override
    {
        gauri::EventDispatcher dispatcher(event);
        dispatcher.Dispatch<gauri::KeyPressedEvent>(GR_BIND_EVENT_FN(ExampleLayer::OnKeyPressedEvent));
    }

    bool OnKeyPressedEvent(gauri::KeyPressedEvent &event)
    {
        return false;
    }

    void OnImGuiRender() override
    {
        ImGui::Begin("Settings");
        ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
        ImGui::End();
    }

  private:
    gauri::Ref<gauri::Shader> m_Shader = nullptr;
    gauri::Ref<gauri::VertexArray> m_VertexArray = nullptr;

    gauri::Ref<gauri::Shader> m_FlatColorShader = nullptr, m_TextureShader = nullptr;
    gauri::Ref<gauri::VertexArray> m_SquareVA = nullptr;

    gauri::Ref<gauri::Texture2D> m_Texture = nullptr;

    gauri::OrthographicCamera m_Camera;
    glm::vec3 m_CameraPosition{};
    float m_CameraMoveSpeed = 5.0f;

    float m_CameraRotation = 0.f;
    float m_CameraRotationSpeed = 30.f;

    float m_SquareSpeed = 1.0f;

    glm::vec3 m_SquareColor = {0.2f, 0.3f, 0.8f};
};
class Sandbox : public gauri::Application
{
  public:
    Sandbox()
    {
        PushLayer(new ExampleLayer());
    }
    ~Sandbox() override = default;

  private:
};

gauri::Application *gauri::CreateApplication()
{
    return new Sandbox();
}
