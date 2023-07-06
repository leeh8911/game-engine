#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gauri.h"
#include "gauri/core/entry_point.h"
#include "imgui.h"

// TEMP
#include "platform/opengl/opengl_shader.h"

#include "editor_layer.h"

namespace gauri
{
class GauriEditor : public Application
{
  public:
    GauriEditor() : Application("Gauri Editor")
    {
        // PushLayer(new ExampleLayer());
        PushLayer(new EditorLayer());
    }
    ~GauriEditor() override = default;

  private:
};

Application *CreateApplication()
{
    return new GauriEditor();
}
} // namespace gauri
