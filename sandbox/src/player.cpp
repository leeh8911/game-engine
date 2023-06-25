#include "particle_system.h"

#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

#include "gauri.h"
#include "player.h"

Player::Player()
{
    m_SmokeParticle.Position = {0.0f, 0.0f};
    m_SmokeParticle.Velocity = {-2.0f, 0.0f};
    m_SmokeParticle.VelocityVariation = {4.0f, 2.0f};
    m_SmokeParticle.SizeBegin = 0.35f;
    m_SmokeParticle.SizeEnd = 0.0f;
    m_SmokeParticle.SizeVariation = 0.15f;
    m_SmokeParticle.ColorBegin = {0.8f, 0.8f, 0.8f, 1.0f};
    m_SmokeParticle.ColorEnd = {0.6f, 0.6f, 0.6f, 1.0f};
    m_SmokeParticle.LifeTime = 4.0f;

    m_EngineParticle.Position = {0.0f, 0.0f};
    m_EngineParticle.Velocity = {-2.0f, 0.0f};
    m_EngineParticle.VelocityVariation = {3.0f, 1.0f};
    m_EngineParticle.SizeBegin = 0.5f;
    m_EngineParticle.SizeEnd = 0.0f;
    m_EngineParticle.SizeVariation = 0.3f;
    m_EngineParticle.ColorBegin = {254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f};
    m_EngineParticle.ColorEnd = {254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f};
    m_EngineParticle.LifeTime = 1.0f;

    ;
}

void Player::LoadAssets()
{
    m_ShipTexture = gauri::Texture2D::Create("assets/texture/Ship.png");
}

void Player::OnUpdate(gauri::Timestep ts)
{
    m_Time += ts;

    if (gauri::Input::IsKeyPressed(GR_KEY_SPACE))
    {
        m_Velocity.y += m_EnginePower;
        if (m_Velocity.y < 0.0f)
        {
            m_Velocity.y += m_EnginePower * 2.0f;
        }

        // Flames
        glm::vec2 emissionPoint = {0.0f, -0.6f};
        float rotation = glm::radians(GetRotation());
        glm::vec4 rotated =
            glm::rotate(glm::mat4(1.0f), rotation, {0.0f, 0.0f, 1.0f}) * glm::vec4(emissionPoint, 0.0f, 1.0f);
        m_EngineParticle.Position = m_Position + glm::vec2(rotated.x, rotated.y);
        m_EngineParticle.Velocity.y = -m_Velocity.y * 0.2f, -0.2f;
        m_ParticleSystem.Emit(m_EngineParticle);
    }
    else
    {
        m_Velocity.y -= m_Gravity;
    }

    m_Velocity.y = glm::clamp(m_Velocity.y, -20.f, 20.f);
    m_Position += m_Velocity * static_cast<float>(ts);

    if (m_Time > m_SmokeNextEmitTime)
    {
        m_SmokeParticle.Position = m_Position;
        m_ParticleSystem.Emit(m_SmokeParticle);
        m_SmokeNextEmitTime += m_SmokeEmitInterval;
    }

    m_ParticleSystem.OnUpdate(ts);
}

void Player::OnRender()
{
    m_ParticleSystem.OnRender();
    gauri::Renderer2D::DrawQuad({m_Position.x, m_Position.y, 0.5f}, {1.0f, 1.3f}, glm::radians(GetRotation()),
                                m_ShipTexture);
}

void Player::OnImGuiRender()
{
    ImGui::DragFloat("Engine Power", &m_EnginePower, 0.1f);
    ImGui::DragFloat("Gravity", &m_Gravity, 0.1f);
}

void Player::Reset()
{
    m_Position = {-10.0f, 0.0f};
    m_Velocity = {5.0f, 0.0f};
}
