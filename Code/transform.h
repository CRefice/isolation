#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <QMatrix4x4>
#include <QVector3D>

struct Transform {
  QVector3D position;
  QVector3D rot_axis{0.0f, 1.0f, 0.0f};
  QVector3D scale{1.0f, 1.0f, 1.0f};
  float rot_angle = 0.0f;
};

QMatrix4x4 to_matrix(const Transform& transform);

#endif // TRANSFORM_H
