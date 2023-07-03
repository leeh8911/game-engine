#pragma once

#include "gauri.h"

#include "particle_system.h"

class Sandbox2D : public gauri::Layer
{
  public:
    Sandbox2D();
    ~Sandbox2D() override = default;
    void OnAttach() override;
    void OnDetach() override;

    void OnUpdate(gauri::Timestep ts) override;
    void OnImGuiRender() override;
    void OnEvent(gauri::Event &event) override;

  private:
    gauri::OrthographicCameraController m_CameraController;

    // Temp
    gauri::Ref<gauri::VertexArray> m_SquareVA = nullptr;
    gauri::Ref<gauri::Shader> m_FlatColorShader = nullptr;

    gauri::Ref<gauri::Texture2D> m_CheckerboardTexture = nullptr;
    gauri::Ref<gauri::Texture2D> m_SpriteSheet = nullptr;
    gauri::Ref<gauri::SubTexture2D> m_TextureStairs = nullptr;
    gauri::Ref<gauri::SubTexture2D> m_TextureBarrel = nullptr;
    gauri::Ref<gauri::SubTexture2D> m_TextureTree = nullptr;

    struct ProfileResult
    {
        const char *Name;
        float Time;
    };

    std::vector<ProfileResult> m_ProfileResults;

    glm::vec4 m_SquareColor = {0.2f, 0.3f, 0.8f, 1.0f};

    ParticleSystem m_ParticleSystem;
    ParticleProps m_Particle;
};
