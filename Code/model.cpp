#include "model.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <cmath>
#include <limits>

Model::Model(QString filename) {
  hNorms = false;
  hTexs = false;

  qDebug() << ":: Loading model:" << filename;
  QFile file(filename);
  if (file.open(QIODevice::ReadOnly)) {
    QTextStream in(&file);

    QString line;
    QStringList tokens;

    while (!in.atEnd()) {
      line = in.readLine();
      if (line.startsWith("#"))
        continue; // skip comments

      tokens = line.split(" ", QString::SkipEmptyParts);

      // Switch depending on first element
      if (tokens[0] == "v") {
        parseVertex(tokens);
      }

      if (tokens[0] == "vn") {
        parseNormal(tokens);
      }

      if (tokens[0] == "vt") {
        parseTexture(tokens);
      }

      if (tokens[0] == "f") {
        parseFace(tokens);
      }
    }

    file.close();

    // create an array version of the data
    unpackIndexes();

    // Allign all vertex indices with the right normal/texturecoord indices
    alignData();
  }
}

/**
 *
 * Unitize the model by scaling so that it fits a box with sides 1
 * and origin at 0,0,0
 * Usefull for models with different scales
 *
 */
void Model::unitize() {
  auto max_x = std::numeric_limits<float>::min();
  auto max_y = std::numeric_limits<float>::min();
  auto max_z = std::numeric_limits<float>::min();
  auto min_x = std::numeric_limits<float>::max();
  auto min_y = std::numeric_limits<float>::max();
  auto min_z = std::numeric_limits<float>::max();
  for (const auto& vertex : vertices_indexed) {
    max_x = std::max(max_x, vertex.x());
    max_y = std::max(max_y, vertex.y());
    max_z = std::max(max_z, vertex.z());
    min_x = std::min(min_x, vertex.x());
    min_y = std::min(min_y, vertex.y());
    min_z = std::min(min_z, vertex.z());
  }
  auto center = QVector3D(max_x + min_x, max_y + min_y, max_z + min_z) / 2.0f;
  auto max_extent = std::max({max_x - min_x, max_y - min_y, max_z - min_z});
  for (auto& vertex : vertices_indexed) {
    vertex = (vertex - center) / (max_extent / 2.0f);
  }
}

QVector<QVector3D> Model::getVertices() { return vertices; }

QVector<QVector3D> Model::getNormals() { return normals; }

QVector<QVector2D> Model::getTextureCoords() { return textureCoords; }

QVector<QVector3D> Model::getVertices_indexed() { return vertices_indexed; }

QVector<QVector3D> Model::getNormals_indexed() { return normals_indexed; }

QVector<QVector2D> Model::getTextureCoords_indexed() {
  return textureCoords_indexed;
}

QVector<unsigned> Model::getIndices() { return indices; }

QVector<float> Model::getVNInterleaved() {
  QVector<float> buffer;

  for (int i = 0; i != vertices.size(); ++i) {
    QVector3D vertex = vertices.at(i);
    QVector3D normal = normals.at(i);
    buffer.append(vertex.x());
    buffer.append(vertex.y());
    buffer.append(vertex.z());
    buffer.append(normal.x());
    buffer.append(normal.y());
    buffer.append(normal.z());
  }

  return buffer;
}

QVector<float> Model::getVNTInterleaved() {
  QVector<float> buffer;

  for (int i = 0; i != vertices.size(); ++i) {
    QVector3D vertex = vertices.at(i);
    QVector3D normal = normals.at(i);
    QVector2D uv = textureCoords.at(i);
    buffer.append(vertex.x());
    buffer.append(vertex.y());
    buffer.append(vertex.z());
    buffer.append(normal.x());
    buffer.append(normal.y());
    buffer.append(normal.z());
    buffer.append(uv.x());
    buffer.append(uv.y());
  }

  return buffer;
}

QVector<float> Model::getVNInterleaved_indexed() {
  QVector<float> buffer;

  for (int i = 0; i != vertices_indexed.size(); ++i) {
    QVector3D vertex = vertices_indexed.at(i);
    QVector3D normal = normals_indexed.at(i);
    buffer.append(vertex.x());
    buffer.append(vertex.y());
    buffer.append(vertex.z());
    buffer.append(normal.x());
    buffer.append(normal.y());
    buffer.append(normal.z());
  }

  return buffer;
}

QVector<float> Model::getVNTInterleaved_indexed() {
  QVector<float> buffer;

  for (int i = 0; i != vertices_indexed.size(); ++i) {
    QVector3D vertex = vertices_indexed.at(i);
    QVector3D normal = normals_indexed.at(i);
    QVector2D uv = textureCoords_indexed.at(i);
    buffer.append(vertex.x());
    buffer.append(vertex.y());
    buffer.append(vertex.z());
    buffer.append(normal.x());
    buffer.append(normal.y());
    buffer.append(normal.z());
    buffer.append(uv.x());
    buffer.append(uv.y());
  }

  return buffer;
}

/**
 * @brief Model::getNumTriangles
 *
 * Gets the number of triangles in the model
 *
 * @return number of triangles
 */
int Model::getNumTriangles() { return vertices.size() / 3; }

void Model::parseVertex(QStringList tokens) {
  float x, y, z;
  x = tokens[1].toFloat();
  y = tokens[2].toFloat();
  z = tokens[3].toFloat();
  vertices_indexed.append(QVector3D(x, y, z));
}

void Model::parseNormal(QStringList tokens) {
  hNorms = true;
  float x, y, z;
  x = tokens[1].toFloat();
  y = tokens[2].toFloat();
  z = tokens[3].toFloat();
  norm.append(QVector3D(x, y, z));
}

void Model::parseTexture(QStringList tokens) {
  hTexs = true;
  float u, v;
  u = tokens[1].toFloat();
  v = tokens[2].toFloat();
  tex.append(QVector2D(u, v));
}

void Model::parseFace(QStringList tokens) {
  QStringList elements;

  for (int i = 1; i != tokens.size(); ++i) {
    elements = tokens[i].split("/");
    // -1 since .obj count from 1
    indices.append(elements[0].toInt() - 1);

    if (elements.size() > 1 && !elements[1].isEmpty()) {
      texcoord_indices.append(elements[1].toInt() - 1);
    }

    if (elements.size() > 2 && !elements[2].isEmpty()) {
      normal_indices.append(elements[2].toInt() - 1);
    }
  }
}

/**
 * @brief Model::alignData
 *
 * Make sure that the indices from the vertices align with those
 * of the normals and the texture coordinates, create extra vertices
 * if vertex has multiple normals or texturecoords
 */
void Model::alignData() {
  QVector<QVector3D> verts = QVector<QVector3D>();
  verts.reserve(vertices_indexed.size());
  QVector<QVector3D> norms = QVector<QVector3D>();
  norms.reserve(vertices_indexed.size());
  QVector<QVector2D> texcs = QVector<QVector2D>();
  texcs.reserve(vertices_indexed.size());
  QVector<Vertex> vs = QVector<Vertex>();

  QVector<unsigned> ind = QVector<unsigned>();
  ind.reserve(indices.size());

  unsigned currentIndex = 0;

  for (int i = 0; i != indices.size(); ++i) {
    QVector3D v = vertices_indexed[indices[i]];

    QVector3D n = QVector2D(0, 0);
    if (hNorms) {
      n = norm[normal_indices[i]];
    }

    QVector2D t = QVector2D(0, 0);
    if (hTexs) {
      t = tex[texcoord_indices[i]];
    }

    Vertex k = Vertex(v, n, t);
    if (vs.contains(k)) {
      // Vertex already exists, use that index
      ind.append(vs.indexOf(k));
    } else {
      // Create a new vertex
      verts.append(v);
      norms.append(n);
      texcs.append(t);
      vs.append(k);
      ind.append(currentIndex);
      ++currentIndex;
    }
  }
  // Remove old data
  vertices_indexed.clear();
  normals_indexed.clear();
  textureCoords_indexed.clear();
  indices.clear();

  // Set the new data
  vertices_indexed = verts;
  normals_indexed = norms;
  textureCoords_indexed = texcs;
  indices = ind;
}

/**
 * @brief Model::unpackIndexes
 *
 * Unpack indices so that they are available for glDrawArrays()
 *
 */
void Model::unpackIndexes() {
  vertices.clear();
  normals.clear();
  textureCoords.clear();
  for (int i = 0; i != indices.size(); ++i) {
    vertices.append(vertices_indexed[indices[i]]);

    if (hNorms) {
      normals.append(norm[normal_indices[i]]);
    }

    if (hTexs) {
      textureCoords.append(tex[texcoord_indices[i]]);
    }
  }
}
