#ifndef ROCKET_HPP_
#define ROCKET_HPP_

#include "abcg.hpp"
#include "gamedata.hpp"

// class Asteroids;
// class Bullets;
// class OpenGLWindow;
// class StarLayers;

class Rocket {
 public:
  void initializeGL(GLuint program);
  void paintGL(const GameData &gameData);
  void terminateGL();

  void update(const GameData &gameData, float deltaTime);

  GLuint m_program{};
  GLint m_translationLoc{};
  GLint m_colorLoc{};
  GLint m_scaleLoc{};

  GLuint m_vao{};
  GLuint m_vbo{};
  GLuint m_ebo{};

  glm::vec4 m_color{1};
  float m_scale{0.125f};
  glm::vec2 m_translation{glm::vec2(0)};
  glm::vec2 m_velocity{glm::vec2(0)};
  float m_acceleration = 0.0004;
  float m_baseSpeed = 0.0012f;

  abcg::ElapsedTimer m_trailBlinkTimer;
  abcg::ElapsedTimer m_accelerationCoolDown;
  // abcg::ElapsedTimer m_bulletCoolDownTimer;
};

#endif