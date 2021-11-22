#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <list>

#include "abcg.hpp"
#include "asteroid.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "rocket.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void handleEvent(SDL_Event &ev) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
  static const int m_numStars{1000};
  std::default_random_engine m_randomEngine;
  Model m_model;
  GLuint m_program{};
  std::array<glm::vec3, m_numStars> m_starPositions;
  std::array<glm::vec3, m_numStars> m_starRotations;
  float m_angle{};

  void randomizeStar(glm::vec3 &position, glm::vec3 &rotation);

  Rocket m_rocket{};
  int m_qtd_asteroids{32};
  std::list<Asteroid> m_asteroids;

  int m_viewportWidth{};
  int m_viewportHeight{};

  Camera m_camera;
  float m_dollySpeed{0.0f};
  float m_truckSpeed{0.0f};
  float m_panSpeed{0.0f};
  float m_yMovement{0.0f};

  void update();
};

#endif