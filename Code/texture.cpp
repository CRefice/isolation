#include <QDebug>
#include <QImage>

#include <cstdint>
#include <vector>

#include "texture.h"

std::vector<std::uint8_t> image_to_bytes(const QImage& image) {
  // needed since (0,0) is bottom left in OpenGL
  QImage im = image.mirrored();
  std::vector<std::uint8_t> data;
  data.reserve(im.width() * im.height() * 4);

  for (int i = 0; i != im.height(); ++i) {
    for (int j = 0; j != im.width(); ++j) {
      QRgb pixel = im.pixel(j, i);

      // pixel is of format #AARRGGBB (in hexadecimal notation)
      // so with bitshifting and binary AND you can get
      // the values of the different components
      std::uint8_t r = (std::uint8_t)((pixel >> 16) & 0xFF); // Red component
      std::uint8_t g = (std::uint8_t)((pixel >> 8) & 0xFF);  // Green component
      std::uint8_t b = (std::uint8_t)(pixel & 0xFF);         // Blue component
      std::uint8_t a = (std::uint8_t)((pixel >> 24) & 0xFF); // Alpha component

      // Add them to the Vector
      data.push_back(r);
      data.push_back(g);
      data.push_back(b);
      data.push_back(a);
    }
  }
  return data;
}

Texture::Texture(unsigned width, unsigned height, GLuint format,
                 GLuint data_type, GLuint data_format, const uint8_t* data) {
  initializeOpenGLFunctions();

  // Create the OpenGL texture handle
  glGenTextures(1, &handle);
  bind();

  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, data_format,
               data_type, data);

  if (data) {
    glGenerateMipmap(GL_TEXTURE_2D);
    set_parameters();
  }
}

Texture::~Texture() { glDeleteTextures(1, &handle); }

void Texture::swap(Texture&& other) { std::swap(handle, other.handle); }

void Texture::bind() { glBindTexture(GL_TEXTURE_2D, handle); }

void Texture::set_parameters() {
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  GLfloat f;
  glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &f);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, f);
}

Texture Texture::from_file(const QString& path) {
  QImage img(path);
  if (img.isNull()) {
    qDebug() << "Error loading texture:" << path;
  }
  auto pixels = image_to_bytes(img);
  return Texture(img.width(), img.height(), GL_SRGB_ALPHA, GL_UNSIGNED_BYTE,
                 GL_RGBA, pixels.data());
}
