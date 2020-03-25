#ifndef VERTEX_H
#define VERTEX_H

#include <QVector2D>
#include <QVector3D>

struct Vector {
  Vector() = default;
  Vector(float x, float y, float z) : x(x), y(y), z(z) {}
  Vector(const QVector3D& vec) : x(vec.x()), y(vec.y()), z(vec.z()) {}

  float x, y, z;
};

struct TexCoord {
  TexCoord() = default;
  TexCoord(float u, float v) : u(u), v(v) {}
  TexCoord(const QVector2D& vec) : u(vec.x()), v(vec.y()) {}

  float u, v;
};

struct Vertex {
  Vector pos;
  Vector normal;
  TexCoord coords;
};

#endif // VERTEX_H
