#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <QOpenGLFunctions_3_3_Core>

#include "texture.h"

class Renderbuffer : protected QOpenGLFunctions_3_3_Core {
public:
  Renderbuffer(unsigned width, unsigned height,
               GLuint format = GL_DEPTH_COMPONENT24);
  ~Renderbuffer();

  void swap(Renderbuffer&& other);

  // Make class move-only to properly delete opengl texture on destruction
  Renderbuffer(Renderbuffer&& other) {
    initializeOpenGLFunctions();
    swap(std::move(other));
  }
  Renderbuffer& operator=(Renderbuffer&& other) {
    swap(std::move(other));
    return *this;
  }
  Renderbuffer(const Renderbuffer&) = delete;
  Renderbuffer& operator=(const Renderbuffer&) = delete;

  void bind();
  void unbind();

  GLuint gl_handle() { return rbo; }

private:
  GLuint rbo = 0;
};

class Framebuffer : protected QOpenGLFunctions_3_3_Core {
public:
  Framebuffer();
  ~Framebuffer();

  void attach_color(Texture& texture);
  void attach_depth(Texture& buf);
  void attach_depth(Renderbuffer& buf);
  void finalize();

  void swap(Framebuffer&& other);

  // Make class move-only to properly delete opengl texture on destruction
  Framebuffer(Framebuffer&& other) {
    initializeOpenGLFunctions();
    swap(std::move(other));
  }
  Framebuffer& operator=(Framebuffer&& other) {
    swap(std::move(other));
    return *this;
  }
  Framebuffer(const Framebuffer&) = delete;
  Framebuffer& operator=(const Framebuffer&) = delete;

  void bind();
  void unbind();

private:
  std::vector<GLenum> color_attachments;
  GLuint fbo = 0;
};

#endif // FRAMEBUFFER_H
