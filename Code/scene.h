#ifndef SCENE_H
#define SCENE_H

#include <vector>

#include "light.h"
#include "mesh.h"

struct Scene {
  std::vector<MeshInstance> meshes;
  Light light;
  float time = 0.0f;

  void update();
};

#endif // SCENE_H
