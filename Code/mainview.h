#ifndef MAINVIEW_H
#define MAINVIEW_H

#include "framebuffer.h"
#include "scene.h"
#include "shader.h"

#include <QColor>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QTimer>
#include <QVector3D>
#include <memory>

class MainView : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
  Q_OBJECT

public:
  enum SceneType : GLuint { ANIM = 0, WAVE };

  MainView(QWidget* parent = 0);
  ~MainView();

protected:
  void initializeGL();
  void resizeGL(int newWidth, int newHeight);
  void paintGL();

  // Functions for keyboard input events
  void keyPressEvent(QKeyEvent* ev);
  void keyReleaseEvent(QKeyEvent* ev);

  // Function for mouse input events
  void mouseDoubleClickEvent(QMouseEvent* ev);
  void mouseMoveEvent(QMouseEvent* ev);
  void mousePressEvent(QMouseEvent* ev);
  void mouseReleaseEvent(QMouseEvent* ev);
  void wheelEvent(QWheelEvent* ev);

private slots:
  void onMessageLogged(QOpenGLDebugMessage Message);

private:
  void createShaderPrograms();
  void createGeometry();

  void create_framebuffers(unsigned width, unsigned height);

  void draw_scene();
  void draw_screen_quad(Texture& source, Framebuffer& destination,
                        ShaderInstance& shader);

  QMatrix4x4 view_transform() const;

  std::unique_ptr<ShaderInstance> phong_shader, shadow_pass_shader,
      high_pass_shader, screen_shader, vert_blur_shader, horiz_blur_shader;
  Scene scene;

  std::unique_ptr<Mesh> screen_quad;
  std::unique_ptr<Framebuffer> framebuf;
  std::unique_ptr<Renderbuffer> depth_renderbuf;
  std::unique_ptr<Texture> screen_texture;

  std::unique_ptr<Framebuffer> shadow_framebuf;
  std::unique_ptr<Texture> shadow_texture;

  std::vector<Framebuffer> bloom_pingpong_framebufs;
  std::vector<Texture> bloom_pingpong_textures;

  QMatrix4x4 proj_transform;
  QMatrix4x4 rotation, scaling;

  // Camera orbiting-related members
  QPoint drag_start;
  float pitch = 0.0f, yaw = 0.0f;
  float camera_distance = 2.0f;

  QOpenGLDebugLogger debugLogger;
  QTimer timer; // timer used for animation

  GLuint screen_width, screen_height;
};

#endif // MAINVIEW_H
