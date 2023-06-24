#pragma once

#include "gauri.h"

class Player
{
  public:
    Player() = default;

    void OnUpdate(gauri::Timestep ts);

    float GetRotation() const
    {
        return m_Rotation;
    }
    glm::vec3 GetPosition() const
    {
        return m_Position;
    }

    void LoadAssets();

  private:
    glm::vec3 m_Position = {0.0f, 0.0f, 0.0f};

    float m_Rotation = 0.0f;
};
