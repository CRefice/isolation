#ifndef ANIMATION_H
#define ANIMATION_H

#include "transform.h"

class Animation {
public:
  virtual ~Animation() = 0;

  virtual Transform apply(Transform transform) = 0;
};

class SpinningAnimation : public Animation {
public:
  SpinningAnimation(float period, QVector3D axis)
      : period(period), axis(axis) {}

  Transform apply(Transform transform) override;

private:
  float period;
  QVector3D axis;
};

class BounceAnimation : public Animation {
public:
  BounceAnimation(float period, QVector3D center, QVector3D offset)
      : period(period), center(center), offset(offset) {}

  Transform apply(Transform transform) override;

private:
  float time = 0.0f, period;
  QVector3D center, offset;
};

class SquashAnimation : public Animation {
public:
  SquashAnimation(float period, QVector3D direction)
      : period(period), direction(direction) {}

  Transform apply(Transform transform) override;

private:
  float time = 0.0f, period;
  QVector3D direction;
};

class ComboAnimation : public Animation {
public:
  Transform apply(Transform transform) override;

  void add(std::unique_ptr<Animation> anim);

private:
  std::vector<std::unique_ptr<Animation>> anims;
};

#endif // ANIMATION_H
