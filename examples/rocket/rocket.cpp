#include "rocket.hpp"

void Rocket::initializeGL(GLuint program) {
  terminateGL();

  m_program = program;
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  // m_rotationLoc = abcg::glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  // m_rotation = 0.0f;
  m_translation = glm::vec2(0);
  m_velocity = glm::vec2(0);

  // Rocket starts centralized but lower on the screen
  m_translation.y -= 0.60f;

  // clang-format off
  std::array<glm::vec2, 16> positions{
      // Rocket body
      glm::vec2{0, +13.5f},     // 0
      glm::vec2{-3.5f, 9.5f},   // 1
      glm::vec2{-3.5f, -4.5f},  // 2
      glm::vec2{-6.5f, -8.0f},  // 3 
      glm::vec2{6.5f, -8.0f},   // 4
      glm::vec2{3.5f, -4.5f},   // 5
      glm::vec2{3.5f, 9.5f},    // 6

      // Left wing
      glm::vec2{-2.5f, 6.0f},  
      glm::vec2{-7.0f, 3.0f},
      glm::vec2{-2.5f, 3.0f},

      // Right wing
      glm::vec2{2.5f, 6.0f}, 
      glm::vec2{7.0f, 3.0f},
      glm::vec2{2.5f, 3.0f},
      
      // Rocket fire
       glm::vec2{-4.5f, -8.0f}, 
       glm::vec2{-0.0f, -13.0f},   
       glm::vec2{4.5f, -8.0f}
      };

  // Normalize
  for (auto &position : positions) {
    position /= glm::vec2{15.5f, 15.5f};
  }

  const std::array indices{0, 1, 6, //body
                           1, 5, 6,
                           1, 2, 5,
                           2, 3, 5,
                           3, 4, 5,
                           // Wings
                           7, 8, 9,
                           10, 11, 12,
                           // Tail
                            13, 14, 15
                          };
  // clang-format on

  // Generate VBO
  abcg::glGenBuffers(1, &m_vbo);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  abcg::glGenBuffers(1, &m_ebo);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_vao);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Rocket::paintGL(const GameData &gameData) {
  if (gameData.m_state != State::Playing) return;

  abcg::glUseProgram(m_program);

  abcg::glBindVertexArray(m_vao);

  abcg::glUniform1f(m_scaleLoc, m_scale);
  // abcg::glUniform1f(m_rotationLoc, m_rotation);
  abcg::glUniform2fv(m_translationLoc, 1, &m_translation.x);

  // Timer for the rocket tail
  if (m_trailBlinkTimer.elapsed() > 100.0 / 1000.0) {
    m_trailBlinkTimer.restart();
  }

  if (gameData.m_input[static_cast<size_t>(Input::Sprint)]) {
    // Show thruster trail during 50 ms
    if (m_trailBlinkTimer.elapsed() < 50.0 / 1000.0) {
      abcg::glEnable(GL_BLEND);
      abcg::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      // 50% transparent
      abcg::glUniform4f(m_colorLoc, 1, 1, 1, 0.5f);

      abcg::glDrawElements(GL_TRIANGLES, 8 * 3, GL_UNSIGNED_INT, nullptr);

      abcg::glDisable(GL_BLEND);
    }
  }

  abcg::glUniform4fv(m_colorLoc, 1, &m_color.r);
  abcg::glDrawElements(GL_TRIANGLES, 7 * 3, GL_UNSIGNED_INT, nullptr);

  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Rocket::terminateGL() {
  abcg::glDeleteBuffers(1, &m_vbo);
  abcg::glDeleteBuffers(1, &m_ebo);
  abcg::glDeleteVertexArrays(1, &m_vao);
}

void Rocket::update(const GameData &gameData, float deltaTime) {
  if (gameData.m_state != State::Playing) return;

  // Move sideways
  if (gameData.m_input[static_cast<size_t>(Input::Left)] &&
      m_translation.x > -0.92)
    m_translation.x -= 0.0025f;
  if (gameData.m_input[static_cast<size_t>(Input::Right)] &&
      m_translation.x < 0.92)
    m_translation.x += 0.0025f;

  // Apply acceleration
  if (gameData.m_input[static_cast<size_t>(Input::Sprint)] &&
      gameData.m_state == State::Playing) {
    m_velocity.y += m_acceleration * deltaTime;
    m_accelerationCoolDown.restart();

    // remove acceleration if rocket is above default speed
  } else if (m_accelerationCoolDown.elapsed() > 50.0 / 1000.0 &&
             m_velocity.y >= 0) {
    m_velocity.y -= m_acceleration * deltaTime;
    m_velocity.y = m_velocity.y < 0 ? 0 : m_velocity.y; //prevents speed from beign negative
  }
}
