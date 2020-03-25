#include "shader.h"

ShaderInstance::ShaderInstance(const QString& vertpath,
                               const QString& fragpath) {
  initializeOpenGLFunctions();

  compile_shaders(vertpath, fragpath);

  find_uniforms();
}

void ShaderInstance::draw(Scene& scene, const QMatrix4x4& view_matrix,
                          const QMatrix4x4& proj_matrix) {
  program.bind();
  bind_global_uniforms(scene.time, view_matrix, proj_matrix);

  for (auto& mesh : scene.meshes) {
    draw_mesh(mesh, scene.light, view_matrix);
  }
}

void ShaderInstance::draw(Mesh& mesh) {
  program.bind();
  mesh.draw();
}

void ShaderInstance::uniform(const char* name, int value) {
  program.bind();
  glUniform1i(program.uniformLocation(name), value);
}

void ShaderInstance::uniform(const char* name, const QMatrix4x4& value) {
  program.bind();
  glUniformMatrix4fv(program.uniformLocation(name), 1, GL_FALSE, value.data());
}

void ShaderInstance::draw_mesh(MeshInstance& instance, const Light& light,
                               const QMatrix4x4& view_matrix) {
  bind_mesh_uniforms(instance, view_matrix, light);
  instance.mesh.draw();
}

void ShaderInstance::compile_shaders(const QString& vertpath,
                                     const QString& fragpath) {
  qDebug() << "Loading vertex shader:" << vertpath;
  program.addShaderFromSourceFile(QOpenGLShader::Vertex, vertpath);
  qDebug() << "Loading fragment shader:" << fragpath;
  program.addShaderFromSourceFile(QOpenGLShader::Fragment, fragpath);
  program.link();
}

void ShaderInstance::find_uniforms() {
  model_uniform = program.uniformLocation("model");
  view_uniform = program.uniformLocation("view");
  projection_uniform = program.uniformLocation("projection");
  normal_mat_uniform = program.uniformLocation("normal_matrix");
  material_diffuse_uniform = program.uniformLocation("material_diffuse");
  material_properties_uniform = program.uniformLocation("material_properties");
  light_position_uniform = program.uniformLocation("light_position");
  light_color_uniform = program.uniformLocation("light_color");
  amplitude_uniform = program.uniformLocation("amplitude");
  freq_uniform = program.uniformLocation("frequency");
  phase_uniform = program.uniformLocation("phase");
  time_uniform = program.uniformLocation("time");
  wave_mask_uniform = program.uniformLocation("wave_mask");

  // Only warn about required uniforms missing, as normal shader and others
  // could lack uniforms related to materials and lights
  if (model_uniform == -1) {
    qDebug()
        << "Failed to get uniform location for model transformation matrix";
  }
  if (view_uniform == -1) {
    qDebug() << "Failed to get uniform location for view matrix";
  }
  if (projection_uniform == -1) {
    qDebug() << "Failed to get uniform location for projection matrix";
  }
  if (normal_mat_uniform == -1) {
    qDebug() << "Failed to get uniform location for model normal matrix";
  }
}

void ShaderInstance::bind_global_uniforms(float time, const QMatrix4x4& view,
                                          const QMatrix4x4& projection) {
  glUniformMatrix4fv(view_uniform, 1, GL_FALSE, view.data());
  glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, projection.data());

  static float amplitude[] = {0.01f, 0.02f, 0.05f};
  static float frequency[] = {500.0f, 0.2f, 0.1f};
  static float phase[] = {0.0f, 1.0f, 2.0f};
  if (amplitude_uniform != -1) {
    glUniform1fv(amplitude_uniform, 3, amplitude);
  }
  if (freq_uniform != -1) {
    glUniform1fv(freq_uniform, 3, frequency);
  }
  if (phase_uniform != -1) {
    glUniform1fv(phase_uniform, 3, phase);
  }
  if (time_uniform != -1) {
    glUniform1f(time_uniform, time);
  }
}

void ShaderInstance::bind_mesh_uniforms(MeshInstance& instance,
                                        const QMatrix4x4& view,
                                        const Light& light) {
  auto model = to_matrix(instance.transform);
  glUniformMatrix4fv(model_uniform, 1, GL_FALSE, model.data());

  // Normal matrix is relative to view space
  auto view_model = view * model;
  glUniformMatrix3fv(normal_mat_uniform, 1, GL_FALSE,
                     view_model.normalMatrix().data());

  uniform("is_water", instance.material->is_water);

  if (instance.material->is_water) {
    static float amplitude[] = {0.1f, 0.08f, 0.3f, 0.2f, 0.04f, 0.12f};
    static float frequency[] = {17.0f, 20.0f, 5.0f, 6.0f, 16.0f, 4.9f};
    static float phase[] = {0.0f, 3.0f, 7.0f, 0.5f, 2.5f, 1.3f};
    if (amplitude_uniform != -1) {
      glUniform1fv(amplitude_uniform, 6, amplitude);
    }
    if (freq_uniform != -1) {
      glUniform1fv(freq_uniform, 6, frequency);
    }
    if (phase_uniform != -1) {
      glUniform1fv(phase_uniform, 6, phase);
    }
  }

  if (material_diffuse_uniform != -1) {
    glActiveTexture(GL_TEXTURE0);
    instance.material->diffuse.bind();
  }

  if (wave_mask_uniform != -1) {
    glActiveTexture(GL_TEXTURE2);
    instance.material->wave_mask.bind();
  }
  glActiveTexture(GL_TEXTURE0);

  if (material_properties_uniform != -1) {
    glUniform4fv(material_properties_uniform, 1,
                 reinterpret_cast<const GLfloat*>(&instance.material->ka));
  }

  if (light_position_uniform != -1 && light_color_uniform != -1) {
    glUniform3fv(light_position_uniform, 1,
                 reinterpret_cast<const GLfloat*>(&light.pos));
    glUniform3fv(light_color_uniform, 1,
                 reinterpret_cast<const GLfloat*>(&light.color));
  }
}
