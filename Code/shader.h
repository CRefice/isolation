#ifndef SHADER_H
#define SHADER_H

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QString>

#include "scene.h"

// A loaded shader program, containing locations for all of the shader's
// uniforms, thus handling scene drawing from start to end.
class ShaderInstance : protected QOpenGLFunctions_3_3_Core {
public:
  ShaderInstance(const QString& vertpath, const QString& fragpath);

  void draw(Scene& scene, const QMatrix4x4& view_matrix,
            const QMatrix4x4& proj_matrix);

  void draw(Mesh& mesh);

  void uniform(const char* name, int value);
  void uniform(const char* name, const QMatrix4x4& value);

private:
  void draw_mesh(MeshInstance& instance, const Light& light,
                 const QMatrix4x4& view_matrix);

  void compile_shaders(const QString& vertpath, const QString& fragpath);
  void find_uniforms();

  void bind_global_uniforms(float time, const QMatrix4x4& view,
                            const QMatrix4x4& projection);
  void bind_mesh_uniforms(MeshInstance& instance, const QMatrix4x4& view,
                          const Light& light);

  QOpenGLShaderProgram program;
  GLint model_uniform, view_uniform, projection_uniform, normal_mat_uniform;
  GLint material_diffuse_uniform, material_properties_uniform;
  GLint light_position_uniform, light_color_uniform;
  // Wave properties
  GLint amplitude_uniform, freq_uniform, phase_uniform, time_uniform;
  GLint wave_mask_uniform;
};

#endif // SHADER_H
