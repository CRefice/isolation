#include "scene.h"

void Scene::update() {
  time += 1.0f / 60.0f;
  for (auto& mesh : meshes) {
    if (mesh.anim) {
      mesh.transform = mesh.anim->apply(mesh.transform);
    }
  }
}
