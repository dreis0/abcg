#ifndef SCORESTARS_HPP_
#define SCORESTARS_HPP_

#include <list>
#include <random>

#include "abcg.hpp"
#include "gamedata.hpp"
#include "rocket.hpp"

class OpenGLWindow;

class ScoreStars {
 public:
  void initializeGL(GLuint program);
  void paintGL();
  void terminateGL();

  void update(const Rocket &rocket);

 private:
  friend OpenGLWindow;

  GLuint m_program{};
  GLint m_colorLoc{};
  GLint m_translationLoc{};
  GLint m_scaleLoc{};
  GLuint m_vboColors{};

  int newStarInterval = 1515;  // miliseconds
  float newBonusChance = 0.2;

  struct ScoreStar {
    GLuint m_vao{};
    GLuint m_vbo{};

    glm::vec4 m_innerColor{1.0f, 1.0f, 1.0f, 0.50f};
    glm::vec4 m_outterColor{0.58f, 0.78f, 1.0f, 1.0f};
    float m_scale{0.06};
    float m_radius{0.90f};
    bool m_hit = false;

    int m_polygonSides{15};

    glm::vec2 m_translation{glm::vec2(0)};
  };

  std::list<ScoreStar> m_stars;

  std::default_random_engine m_randomEngine;
  std::uniform_real_distribution<float> m_randomDist{-1.0f, 1.0f};
  abcg::ElapsedTimer m_new_star_timer;

  ScoreStars::ScoreStar createScoreStar();
};

#endif