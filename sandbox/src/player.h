#pragma once

#include "gauri.h"

#include "particle_system.h"

class Player
{
  public:
    Player();

    void LoadAssets();

    void OnUpdate(gauri::Timestep ts);
    void OnRender();

    void OnImGuiRender();

    void Reset();

    float GetRotation() const
    {
        return m_Rotation;
    }
    const glm::vec2 &GetPosition() const
    {
        return m_Position;
    }

    uint32_t GetScore() const
    {
        return (uint32_t)(m_Position.x + 10.0f) / 10.0f;
    }

  private:
    glm::vec2 m_Position = {-10.0f, 0.0f};
    glm::vec2 m_Velocity = {5.0f, 0.0f};

    float m_EnginePower = 0.5f;
    float m_Gravity = 0.5f;

    float m_Time = 0.0f;
    float m_SmokeEmitInterval = 0.4f;
    float m_SmokeNextEmitTime = m_SmokeEmitInterval;

    ParticleProps m_SmokeParticle, m_EngineParticle;
    ParticleSystem m_ParticleSystem;

    gauri::Ref<gauri::Texture2D> m_ShipTexture;
    float m_Rotation = 0.0f;
};
