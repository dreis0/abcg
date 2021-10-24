#ifndef ASTEROIDS_HPP_
#define ASTEROIDS_HPP_

#include <list>
#include <random>

#include "abcg.hpp"
#include "gamedata.hpp"
#include "rocket.hpp"

class OpenGLWindow;

class Obstacles {
 public:
  void initializeGL(GLuint program);
  void paintGL();
  void terminateGL();

  void update(const Rocket &rocket, float deltaTime);

 private:
  friend OpenGLWindow;

  GLuint m_program{};
  GLint m_colorLoc{};
  GLint m_translationLoc{};
  GLint m_scaleLoc{};

  int newObstacleInterval = 850;  // miliseconds

  struct Obstacle {
    GLuint m_vao{};
    GLuint m_vbo{};

    glm::vec4 m_color{1};
    bool m_hit{false};
    int m_polygonSides{};
    float m_scale{};
    glm::vec2 m_translation{glm::vec2(0)};
    glm::vec2 m_velocity{glm::vec2(0)};
  };

  std::list<Obstacle> m_obstacles;

  std::default_random_engine m_randomEngine;
  std::uniform_real_distribution<float> m_randomDist{-1.0f, 1.0f};
  abcg::ElapsedTimer m_new_obstacle_timer;

  Obstacles::Obstacle createObstacle();
};

#endif