#include <QDebug>
#include <cassert>

#include "framebuffer.h"

Renderbuffer::Renderbuffer(unsigned width, unsigned height, GLuint format) {
  initializeOpenGLFunctions();

  glGenRenderbuffers(1, &rbo);
  bind();
  glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
  unbind();
}

Renderbuffer::~Renderbuffer() { glDeleteRenderbuffers(1, &rbo); }
void Renderbuffer::swap(Renderbuffer&& other) { std::swap(rbo, other.rbo); }
void Renderbuffer::bind() { glBindRenderbuffer(GL_RENDERBUFFER, rbo); }
void Renderbuffer::unbind() { glBindRenderbuffer(GL_RENDERBUFFER, 0); }

Framebuffer::Framebuffer() {
  initializeOpenGLFunctions();

  glGenFramebuffers(1, &fbo);
}

Framebuffer::~Framebuffer() { glDeleteFramebuffers(1, &fbo); }
void Framebuffer::swap(Framebuffer&& other) { std::swap(fbo, other.fbo); }
void Framebuffer::bind() { glBindFramebuffer(GL_FRAMEBUFFER, fbo); }
void Framebuffer::unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void Framebuffer::attach_color(Texture& texture) {
  bind();
  GLuint attachment = GL_COLOR_ATTACHMENT0 + color_attachments.size();
  glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D,
                         texture.gl_handle(), 0);
  color_attachments.push_back(attachment);
}

void Framebuffer::attach_depth(Texture& texture) {
  bind();
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         texture.gl_handle(), 0);
}

void Framebuffer::attach_depth(Renderbuffer& buf) {
  bind();
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, buf.gl_handle());
}

void Framebuffer::finalize() {
  bind();
  if (color_attachments.empty()) {
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
  } else {
    glDrawBuffers(color_attachments.size(), color_attachments.data());
  }
  assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
  unbind();
}
