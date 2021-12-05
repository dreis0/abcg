#ifndef ROCKET_HPP_
#define ROCKET_HPP_

#include <vector>

#include "abcg.hpp"
#include "vertex.hpp"

class Rocket {
 public:
  void loadDiffuseTexture(std::string_view path);
  void loadObj(std::string_view path, bool standardize = true);
  void render(GLint program) const;
  void init(GLuint program);
  void terminateGL();

  [[nodiscard]] int getNumTriangles() const {
    return static_cast<int>(m_indices.size()) / 3;
  }

 private:
  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};

  glm::vec4 m_Ka;
  glm::vec4 m_Kd;
  glm::vec4 m_Ks;
  float m_shininess;
  GLuint m_diffuseTexture{};

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;
  glm::vec4 m_color{0.8f, 0.8f, 0.8f, 1.0f};

  bool m_hasNormals{false};
  bool m_hasTexCoords{false};

  void computeNormals();
  void createBuffers();
  void standardize();
};

#endif