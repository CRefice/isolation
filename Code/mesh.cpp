#include <algorithm>
#include <cassert>

#include "material.h"
#include "mesh.h"
#include "model.h"

Mesh::Mesh(const std::vector<Vertex>& vertices,
           const std::vector<unsigned int>& indices) {
  // Create a mesh from raw arrays of vertices and indices
  initializeOpenGLFunctions();

  create_buffers();
  fill_buffers(vertices, indices);
  define_data_layout();

  // Unbind the vertex array when done
  glBindVertexArray(0);
}

Mesh::~Mesh() {
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);
}

void Mesh::swap(Mesh&& other) {
  std::swap(vao, other.vao);
  std::swap(vbo, other.vbo);
  std::swap(ebo, other.ebo);
  std::swap(index_count, other.index_count);
}

void Mesh::draw() {
  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
}

void Mesh::create_buffers() {
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);
}

void Mesh::fill_buffers(const std::vector<Vertex>& vertices,
                        const std::vector<unsigned int>& indices) {
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
               vertices.data(), GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  index_count = indices.size();
}

void Mesh::define_data_layout() {
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<GLvoid*>(offsetof(Vertex, pos)));
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<GLvoid*>(offsetof(Vertex, normal)));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<GLvoid*>(offsetof(Vertex, coords)));
}

Mesh Mesh::from_file(const QString& filename) {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;

  auto model = Model(filename);
  model.unitize();

  auto qVerts = model.getVertices_indexed();
  auto qNormals = model.getNormals_indexed();
  auto qCoords = model.getTextureCoords_indexed();
  assert((qVerts.size() == qNormals.size()) &&
         (qVerts.size() == qCoords.size()));
  for (int i = 0; i < qVerts.size(); ++i) {
    // Read off position, normal and tex coords from model
    Vector position(qVerts[i]);
    Vector normal(qNormals[i]);
    TexCoord coord(qCoords[i]);
    vertices.push_back(Vertex{position, normal, coord});
  }
  auto qIndices = model.getIndices();
  indices = std::vector<unsigned int>(qIndices.begin(), qIndices.end());
  return Mesh(vertices, indices);
}

Mesh Mesh::screen_quad() {
  static const std::vector<Vertex> vertices = {
      Vertex{{-1.0, +1.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 1.0}}, // Top left
      Vertex{{+1.0, +1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 1.0}}, // Top right
      Vertex{{-1.0, -1.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0}}, // Bottom left
      Vertex{{+1.0, -1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 0.0}}, // Bottom right
  };
  static const std::vector<unsigned int> indices = {0, 2, 3, 0, 3, 1};
  return Mesh(vertices, indices);
}
