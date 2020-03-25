#include <QDateTime>
#include <cmath>

#include "mainview.h"
#include "mesh.h"

constexpr float frame_time = 1000.0f / 60.0f;
constexpr float shadow_map_size = 2048;
static auto sky_color = QVector3D(0.2f, 0.8f, 1.0f) * 10.0f;

/**
 * @brief MainView::MainView
 *
 * Constructor of MainView
 *
 * @param parent
 */
MainView::MainView(QWidget* parent) : QOpenGLWidget(parent) {
  connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
}

/**
 * @brief MainView::~MainView
 *
 * Destructor of MainView
 * This is the last function called, before exit of the program
 * Use this to clean up your variables, buffers etc.
 *
 */
MainView::~MainView() {
  qDebug() << "MainView destructor";

  makeCurrent();
}

// --- OpenGL initialization

/**
 * @brief MainView::initializeGL
 *
 * Called upon OpenGL initialization
 * Attaches a debugger and calls other init functions
 */
void MainView::initializeGL() {
  qDebug() << ":: Initializing OpenGL";
  initializeOpenGLFunctions();

  connect(&debugLogger, SIGNAL(messageLogged(QOpenGLDebugMessage)), this,
          SLOT(onMessageLogged(QOpenGLDebugMessage)), Qt::DirectConnection);

  if (debugLogger.initialize()) {
    qDebug() << ":: Logging initialized";
    debugLogger.startLogging(QOpenGLDebugLogger::SynchronousLogging);
  }

  QString glVersion;
  glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
  qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

  // Enable depth buffer
  glEnable(GL_DEPTH_TEST);

  // Default is GL_LESS
  glDepthFunc(GL_LEQUAL);

  glDisable(GL_CULL_FACE);

  glClearColor(sky_color.x(), sky_color.y(), sky_color.z(), 0.0f);

  createShaderPrograms();
  createGeometry();

  create_framebuffers(800, 600);

  proj_transform.perspective(60, 1, 0.001, 100.0);

  timer.start(frame_time);
}

void MainView::createShaderPrograms() {
  phong_shader = std::make_unique<ShaderInstance>(
      ":/shaders/vertshader_phong.glsl", ":/shaders/fragshader_phong.glsl");
  phong_shader->uniform("material_diffuse", 0);
  phong_shader->uniform("shadow_map", 1);
  phong_shader->uniform("wave_mask", 2);

  shadow_pass_shader = std::make_unique<ShaderInstance>(
      ":/shaders/vertshader_shadow.glsl", ":/shaders/fragshader_shadow.glsl");
  shadow_pass_shader->uniform("wave_mask", 2);

  screen_shader = std::make_unique<ShaderInstance>(
      ":/shaders/vertshader_screen.glsl", ":/shaders/fragshader_screen.glsl");
  screen_shader->uniform("screen_texture", 0);
  screen_shader->uniform("bloom_texture", 1);

  high_pass_shader =
      std::make_unique<ShaderInstance>(":/shaders/vertshader_screen.glsl",
                                       ":/shaders/fragshader_high_pass.glsl");
  vert_blur_shader = std::make_unique<ShaderInstance>(
      ":/shaders/vertshader_screen.glsl",
      ":/shaders/fragshader_blur_vertical.glsl");
  horiz_blur_shader = std::make_unique<ShaderInstance>(
      ":/shaders/vertshader_screen.glsl",
      ":/shaders/fragshader_blur_horizontal.glsl");
}

void MainView::createGeometry() {
  screen_quad = std::make_unique<Mesh>(Mesh::screen_quad());

  scene.light.pos = {0.0f, 3.0f, 1.5f};
  scene.light.color = QVector3D(0.99f, 0.72f, 0.60f) * 30.0f;

  auto rugmat = std::make_shared<Material>(
      Texture::from_file(":/textures/rug_logo.png"), 0.2f, 0.6f, 0.2f, 4.0f,
      Texture::from_file(":/textures/blank.png"));

  auto bark_mat = std::make_shared<Material>(
      Texture::from_file(":/textures/bark.png"), 0.2f, 0.6f, 0.2f, 2.0f,
      Texture::from_file(":/textures/blank.png"));
  auto leaf_mat = std::make_shared<Material>(
      Texture::from_file(":/textures/leaves.png"), 0.2f, 0.6f, 0.3f, 16.0f,
      Texture::from_file(":/textures/leaves_mask.png"));
  auto sand_mat = std::make_shared<Material>(
      Texture::from_file(":/textures/sand.png"), 0.2f, 0.6f, 0.3f, 16.0f,
      Texture::from_file(":/textures/blank.png"));
  auto ocean_mat = std::make_shared<Material>(
      Texture::from_file(":/textures/white.png"), 0.2f, 0.4f, 0.5f, 20.0f,
      Texture::from_file(":/textures/gradient.png"));
  ocean_mat->is_water = true;

  Transform transf;
  transf.position.setZ(1.0f);
  scene.meshes.emplace_back(Mesh::from_file(":/models/bark.obj"), bark_mat,
                            nullptr, transf);

  transf = Transform();
  transf.position.setZ(+0.5f);
  transf.position.setY(0.7f);
  scene.meshes.emplace_back(Mesh::from_file(":/models/leaves.obj"), leaf_mat,
                            nullptr, transf);

  transf = Transform();
  transf.scale = QVector3D(2.0f, 2.0f, 2.0f);
  transf.position.setY(-1.0f);
  transf.position.setZ(0.00f);
  scene.meshes.emplace_back(Mesh::from_file(":/models/island.obj"), sand_mat,
                            nullptr, transf);

  transf = Transform();
  transf.position.setY(-1.0f);
  transf.position.setZ(1.0f);
  transf.scale = QVector3D(50.0f, 1.0f, 50.0f);
  scene.meshes.emplace_back(Mesh::from_file(":/models/ocean.obj"), ocean_mat,
                            nullptr, transf);
}

void MainView::create_framebuffers(unsigned int width, unsigned int height) {
  {
    framebuf = std::make_unique<Framebuffer>();
    screen_texture =
        std::make_unique<Texture>(width, height, GL_RGB16F, GL_FLOAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    framebuf->attach_color(*screen_texture);
    depth_renderbuf = std::make_unique<Renderbuffer>(width, height);
    framebuf->attach_depth(*depth_renderbuf);
    framebuf->finalize();
  }

  {
    bloom_pingpong_framebufs.clear();
    bloom_pingpong_textures.clear();

    for (int i = 0; i < 2; ++i) {
      bloom_pingpong_textures.emplace_back(width, height, GL_RGB16F, GL_FLOAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      Framebuffer buf;
      buf.attach_color(bloom_pingpong_textures.back());
      buf.finalize();
      bloom_pingpong_framebufs.push_back(std::move(buf));
    }
  }

  {
    shadow_framebuf = std::make_unique<Framebuffer>();
    shadow_texture = std::make_unique<Texture>(shadow_map_size, shadow_map_size,
                                               GL_DEPTH_COMPONENT24, GL_FLOAT,
                                               GL_DEPTH_COMPONENT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
                    GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    shadow_framebuf->attach_depth(*shadow_texture);
    shadow_framebuf->finalize();
  }
}

// --- OpenGL drawing

void MainView::draw_scene() {
  scene.update();

  glViewport(0, 0, shadow_map_size, shadow_map_size);
  shadow_framebuf->bind();
  glClear(GL_DEPTH_BUFFER_BIT);

  QVector3D light_pos(scene.light.pos.x, scene.light.pos.y, scene.light.pos.z);
  QMatrix4x4 light_proj, light_view;
  light_proj.ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);

  light_view.lookAt(light_pos, QVector3D(0.0f, 0.0f, 0.0f),
                    QVector3D(0.0f, 1.0f, 0.0f));

  shadow_pass_shader->draw(scene, light_view, light_proj);

  glViewport(0, 0, screen_width, screen_height);
  framebuf->bind();

  glEnable(GL_DEPTH_TEST);
  glClearColor(sky_color.x(), sky_color.y(), sky_color.z(), 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  QMatrix4x4 view = view_transform();

  glActiveTexture(GL_TEXTURE1);
  shadow_texture->bind();

  phong_shader->uniform("light_view", light_view);
  phong_shader->uniform("light_projection", light_proj);
  phong_shader->draw(scene, view, proj_transform);
}

/**
 * @brief MainView::paintGL
 *
 * Actual function used for drawing to the screen
 *
 */

void MainView::draw_screen_quad(Texture& source, Framebuffer& destination,
                                ShaderInstance& shader) {
  destination.bind();
  source.bind();

  glClear(GL_COLOR_BUFFER_BIT);
  shader.draw(*screen_quad);
}

void MainView::paintGL() {
  int oldFbo;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFbo);

  draw_scene();

  glDisable(GL_DEPTH_TEST);

  // Extract bright parts from image into first bloom buffer
  draw_screen_quad(*screen_texture, bloom_pingpong_framebufs.front(),
                   *high_pass_shader);

  for (int i = 0; i < 5; i++) {
    draw_screen_quad(bloom_pingpong_textures.front(),
                     bloom_pingpong_framebufs.back(), *vert_blur_shader);

    draw_screen_quad(bloom_pingpong_textures.back(),
                     bloom_pingpong_framebufs.front(), *horiz_blur_shader);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, oldFbo);
  glEnable(GL_DEPTH_TEST);

  // Combine bloom with scene
  glActiveTexture(GL_TEXTURE0);
  screen_texture->bind();
  glActiveTexture(GL_TEXTURE1);
  bloom_pingpong_textures.front().bind();
  glClear(GL_COLOR_BUFFER_BIT);
  screen_shader->draw(*screen_quad);
}

/**
 * @brief MainView::resizeGL
 *
 * Called upon resizing of the screen
 *
 * @param newWidth
 * @param newHeight
 */
void MainView::resizeGL(int newWidth, int newHeight) {
  screen_width = newWidth;
  screen_height = newHeight;

  create_framebuffers(newWidth, newHeight);

  // Update projection to fit the new aspect ratio
  float ratio = (float)newWidth / newHeight;
  proj_transform = QMatrix4x4();
  proj_transform.perspective(60, ratio, 0.001, 100.0);
}

// --- Private helpers

/**
 * @brief MainView::onMessageLogged
 *
 * OpenGL logging function, do not change
 *
 * @param Message
 */
void MainView::onMessageLogged(QOpenGLDebugMessage Message) {
  qDebug() << " → Log:" << Message;
}

QMatrix4x4 MainView::view_transform() const {
  QVector3D center(0, 0, 0);
  // For some reason, in QT transformations are applied
  // in the opposite order as calling order.
  QMatrix4x4 transform;
  transform.translate(QVector3D(0, 0, -camera_distance));
  transform.translate(center);
  transform.rotate(yaw, QVector3D(1, 0, 0));
  transform.rotate(pitch, QVector3D(0, 1, 0));
  transform.translate(-center);
  return transform;
}
