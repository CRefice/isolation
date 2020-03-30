#ifndef TEXTURE_H
#define TEXTURE_H

#include <QOpenGLFunctions_3_3_Core>
#include <QString>

#include <cstdint>
#include <vector>

#include "vertex.h"

class Texture : protected QOpenGLFunctions_3_3_Core {
public:
  Texture(unsigned width, unsigned height, GLuint format,
          GLuint data_type = GL_UNSIGNED_BYTE, GLuint data_format = GL_RGBA,
          const uint8_t* data = nullptr);
  ~Texture();

  void swap(Texture&& other);

  // Make class move-only to properly delete opengl texture on destruction
  Texture(Texture&& other) {
    initializeOpenGLFunctions();
    swap(std::move(other));
  }
  Texture& operator=(Texture&& other) {
    swap(std::move(other));
    return *this;
  }
  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;

  void bind();

  GLuint gl_handle() { return handle; }

  static Texture from_file(const QString& path);

private:
  void set_parameters();

  GLuint handle = 0;
};
#endif // TEXTURE_H
