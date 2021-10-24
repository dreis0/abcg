#ifndef ASTEROIDS_HPP_
#define ASTEROIDS_HPP_

#include <list>
#include <random>

#include "abcg.hpp"
#include "gamedata.hpp"

class OpenGLWindow;

class Road {
 public:
  void initializeGL(GLuint program);
  void paintGL();
  void terminateGL();

  void update(float deltaTime);

 private:
  friend OpenGLWindow;

  GLuint m_program{};
  // GLint m_colorLoc{};
  // GLint m_translationLoc{};
  // GLint m_scaleLoc{};

  GLuint m_vao{};
  GLuint m_vbo{};

  std::default_random_engine m_randomEngine;
  std::uniform_real_distribution<float> m_randomDist{-1.0f, 1.0f};
};

#endif