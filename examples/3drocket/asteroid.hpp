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

  [[nodiscard]] glm::vec4 getKa() const { return m_Ka; }
  [[nodiscard]] glm::vec4 getKd() const { return m_Kd; }
  [[nodiscard]] glm::vec4 getKs() const { return m_Ks; }
  [[nodiscard]] float getShininess() const { return m_shininess; }

  int m_instances;

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

  std::vector<glm::vec3> m_spinDirections;
  std::vector<glm::vec3> m_initialPositions;
  std::vector<glm::vec3> m_directions;

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