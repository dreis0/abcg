#include "road.hpp"

// #include <glm/gtx/fast_trigonometry.hpp>
// #include <glm/gtx/rotate_vector.hpp>

// GL_LINE_STIPPLE_REPEAT
void Road::initializeGL(GLuint program) {
  terminateGL();

  m_program = program;

  abcg::glDrawArrays(GL_LINE_STIPPLE_REPEAT, 0, 1);

  // Generate VBO
  abcg::glGenBuffers(1, &m_vbo);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  abcg::glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
                     positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(asteroid.m_vao);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, asteroid.m_vbo);
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Road::paintGL() {}

void Road::terminateGL() {}

void Road::update(float deltaTime) {}
