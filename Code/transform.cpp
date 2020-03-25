#include "transform.h"

QMatrix4x4 to_matrix(const Transform& transform) {
  QMatrix4x4 mat;
  mat.translate(transform.position);
  mat.rotate(transform.rot_angle, transform.rot_axis);
  mat.scale(transform.scale);
  return mat;
}
