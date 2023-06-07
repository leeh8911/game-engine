
#include <iostream>

#include "gauri.h"
#include "imgui.h"

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

        std::shared_ptr<gauri::VertexBuffer> vertexBuffer;
        vertexBuffer.reset(gauri::VertexBuffer::Create(vertices, sizeof(vertices)));

        gauri::BufferLayout layout = {{gauri::ShaderDataType::Float3, "a_Position"},
                                      {gauri::ShaderDataType::Float4, "a_Color"}};
        vertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(vertexBuffer);

        uint32_t indices[3] = {0, 1, 2};
        std::shared_ptr<gauri::IndexBuffer> indexBuffer;
        indexBuffer.reset(gauri::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
        m_VertexArray->SetIndexBuffer(indexBuffer);

        m_SquareVA.reset(gauri::VertexArray::Create());

        // clang-format off
        // triangle {x, y, z}
        float squareVertices[3 * 4] = {-0.75f, -0.75f, 0.0f,
                                        0.75f, -0.75f, 0.0f, 
                                        0.75f, 0.75f, 0.0f, 
                                       -0.75f, 0.75f, 0.0f};
        // clang-format on
        std::shared_ptr<gauri::VertexBuffer> squareVB;
        squareVB.reset(gauri::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
        squareVB->SetLayout({{gauri::ShaderDataType::Float3, "a_Position"}});
        m_SquareVA->AddVertexBuffer(squareVB);

        uint32_t squareIndices[6] = {0, 1, 2, 2, 3, 0};
        std::shared_ptr<gauri::IndexBuffer> suqareIB;
        suqareIB.reset(gauri::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
        m_SquareVA->SetIndexBuffer(suqareIB);

        std::string vertexSrc = R"(
        #version 330 core
        
        layout(location = 0) in vec3 a_Position;        
        layout(location = 1) in vec4 a_Color;        
        
        uniform mat4 u_ViewProjection;
        
        out vec3 v_Position;
        out vec4 v_Color;

        void main()
        {
            v_Position = a_Position;
            v_Color = a_Color;
            gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
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

        m_Shader.reset(new gauri::Shader(vertexSrc, framgmentSrc));

        std::string blueShaderVertexSrc = R"(
        #version 330 core
        
        layout(location = 0) in vec3 a_Position;        
        
        uniform mat4 u_ViewProjection;

        out vec3 v_Position;

        void main()
        {
            v_Position = a_Position;
            gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
        }
    )";
        std::string blueShaderFragmentSrc = R"(
        #version 330 core
        
        layout(location = 0) out vec4 color;

        in vec3 v_Position;

        void main()
        {
            color = vec4(0.2, 0.3, 0.8, 1.0);
        }
    )";

        m_BlueShader.reset(new gauri::Shader(blueShaderVertexSrc, blueShaderFragmentSrc));
    }

    void OnUpdate(gauri::Timestep ts) override
    {
        if (gauri::Input::IsKeyPressed(GR_KEY_LEFT))
        {
            m_CameraPosition.x -= m_CameraSpeed * ts;
        }
        else if (gauri::Input::IsKeyPressed(GR_KEY_RIGHT))
        {
            m_CameraPosition.x += m_CameraSpeed * ts;
        }

        if (gauri::Input::IsKeyPressed(GR_KEY_UP))
        {
            m_CameraPosition.y += m_CameraSpeed * ts;
        }
        else if (gauri::Input::IsKeyPressed(GR_KEY_DOWN))
        {
            m_CameraPosition.y -= m_CameraSpeed * ts;
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

        gauri::Renderer::Submit(m_BlueShader, m_SquareVA);
        gauri::Renderer::Submit(m_Shader, m_VertexArray);

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
    }

  private:
    std::shared_ptr<gauri::Shader> m_Shader = nullptr;
    std::shared_ptr<gauri::VertexArray> m_VertexArray = nullptr;
    std::shared_ptr<gauri::VertexBuffer> m_VertexBuffer = nullptr;
    std::shared_ptr<gauri::IndexBuffer> m_IndexBuffer = nullptr;

    std::shared_ptr<gauri::Shader> m_BlueShader = nullptr;
    std::shared_ptr<gauri::VertexArray> m_SquareVA = nullptr;
    gauri::OrthographicCamera m_Camera;
    glm::vec3 m_CameraPosition{};
    float m_CameraSpeed = 0.1f;

    float m_CameraRotation = 0.f;
    float m_CameraRotationSpeed = 10.f;
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
