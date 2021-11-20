#ifndef ASTEROID_HPP_
#define ASTEROID_HPP_

#include <vector>

#include "abcg.hpp"
#include "vertex.hpp"

class Asteroid {
 public:
  void loadObj(std::string_view path, bool standardize = true);
  void render(GLint colorLoc) const;
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

  int m_spinDirection;
  glm::vec3 m_direction{};

  std::default_random_engine m_randomEngine;

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;
  glm::vec4 m_color{0.8f, 0.8f, 0.8f, 1.0f};

  void createBuffers();
  void standardize();
};

#endif