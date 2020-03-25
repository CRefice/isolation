#include <cmath>

#include "animation.h"

Animation::~Animation() {}

Transform SpinningAnimation::apply(Transform transform) {
  transform.rot_axis = axis;
  transform.rot_angle += 360.0 / period / 60.0f;
  return transform;
}

Transform BounceAnimation::apply(Transform transform) {
  transform.position = center + std::sin(time / period) * offset;
  time += 1.0 / 60.0f;
  return transform;
}

Transform SquashAnimation::apply(Transform transform) {
  transform.scale = QVector3D(1.0f, 1.0f, 1.0f) +
                    (0.5f + std::sin(time / period) / 2.0f) * direction;
  time += 1.0 / 60.0f;
  return transform;
}

Transform ComboAnimation::apply(Transform transform) {
  for (auto& anim : anims) {
    transform = anim->apply(transform);
  }
  return transform;
}

void ComboAnimation::add(std::unique_ptr<Animation> anim) {
  anims.push_back(std::move(anim));
}
