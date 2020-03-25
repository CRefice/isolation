#ifndef MATERIAL_H
#define MATERIAL_H

#include "texture.h"
#include "vertex.h"

struct Material {
  Material(Texture diffuse, float ka, float kd, float ks, float exp,
           Texture wave_mask)
      : diffuse(std::move(diffuse)), ka(ka), kd(kd), ks(ks), exp(exp),
        wave_mask(std::move(wave_mask)) {}

  Texture diffuse;
  float ka, kd, ks, exp;
  Texture wave_mask;
  bool is_water = false;
};

#endif // MATERIAL_H
