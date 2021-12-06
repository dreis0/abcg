#ifndef ASTEROID_HPP_
#define ASTEROID_HPP_

#include <random>
#include <vector>

#include "abcg.hpp"
#include "vertex.hpp"

class Asteroid {
 public:
  void loadDiffuseTexture(std::string_view path);
  void loadNormalTexture(std::string_view path);
  void render(GLint program) const;
  void loadModel(std::string assetsPath, std::string objName);
  void init(GLuint program);
  void terminateGL();

  [[nodiscard]] int getNumTriangles() const {
    return static_cast<int>(m_indices.size()) / 3;
  }

 private:
  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};

  abcg::ElapsedTimer m_timer;

  glm::vec4 m_Ka;
  glm::vec4 m_Kd;
  glm::vec4 m_Ks;
  float m_shininess;
  GLuint m_diffuseTexture{};
  GLuint m_normalTexture{};

  glm::vec3 m_spinDirection{0, 0, 0};
  glm::vec3 m_initialPosition{0.0f, 0.0f, 0.0f};
  glm::vec3 m_direction{0.0f, 0.0f, 0.0f};

  std::default_random_engine m_randomEngine;

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;
  glm::vec4 m_color{0.8f, 0.8f, 0.8f, 1.0f};

  bool m_hasNormals{false};
  bool m_hasTexCoords{false};

  void computeNormals();
  void computeTangents();
  void createBuffers();
  void standardize();
  void loadObj(std::string_view path, bool standardize = true);
};

#endif