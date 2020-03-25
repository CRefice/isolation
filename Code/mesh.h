#ifndef MESH_HPP
#define MESH_HPP

#include <QMatrix4x4>
#include <QOpenGLFunctions_3_3_Core>

#include <memory>
#include <vector>

#include "animation.h"
#include "material.h"
#include "transform.h"
#include "vertex.h"

class Mesh : protected QOpenGLFunctions_3_3_Core {
public:
  Mesh(const std::vector<Vertex>& vertices,
       const std::vector<unsigned int>& indices);
  ~Mesh();

  void swap(Mesh&& other);

  // Make class move-only to properly delete opengl buffers on destruction
  Mesh(Mesh&& other) {
    initializeOpenGLFunctions();
    swap(std::move(other));
  }
  Mesh& operator=(Mesh&& other) {
    swap(std::move(other));
    return *this;
  }
  Mesh(const Mesh&) = delete;
  Mesh& operator=(const Mesh&) = delete;

  void draw();

  static Mesh from_file(const QString& filename);
  static Mesh screen_quad();

private:
  void create_buffers();
  void fill_buffers(const std::vector<Vertex>& vertices,
                    const std::vector<unsigned int>& indices);
  void define_data_layout();

  GLuint vao = 0, vbo = 0, ebo = 0;
  std::size_t index_count = 0;
};

struct MeshInstance {
  MeshInstance(Mesh&& mesh, std::shared_ptr<Material> material,
               std::unique_ptr<Animation> anim = nullptr,
               Transform transform = {})
      : mesh(std::move(mesh)), material(std::move(material)),
        anim(std::move(anim)), transform(std::move(transform)) {}

  Mesh mesh;
  std::shared_ptr<Material> material;
  std::unique_ptr<Animation> anim;
  Transform transform;
};

#endif // MESH_HPP
