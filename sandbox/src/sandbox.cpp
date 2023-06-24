#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gauri.h"
#include "gauri/core/entry_point.h"
#include "imgui.h"

// TEMP
#include "platform/opengl/opengl_shader.h"

#include "game_layer.h"

class Sandbox : public gauri::Application
{
  public:
    Sandbox()
    {
        PushLayer(new GameLayer());
    }
    ~Sandbox() override = default;

  private:
};

gauri::Application *gauri::CreateApplication()
{
    return new Sandbox();
}
