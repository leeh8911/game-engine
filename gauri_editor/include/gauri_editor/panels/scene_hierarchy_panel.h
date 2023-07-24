#pragma once

#include "gauri/core/base.h"
#include "gauri/scene/entity.h"
#include "gauri/scene/scene.h"

namespace gauri
{
class SceneHierarchyPanel
{
  public:
    SceneHierarchyPanel() = default;
    SceneHierarchyPanel(Ref<Scene> &scene);

    void SetContext(const Ref<Scene> &scene);

    void OnImGuiRender();

    Entity GetSelectedEntity() const
    {
        return m_SelectionContext;
    }

  private:
    void DrawEntityNode(Entity entity);
    void DrawComponents(Entity entity);
    Ref<Scene> m_Context{nullptr};
    Entity m_SelectionContext{};
};
} // namespace gauri
