#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <imgui.h>

#include <random>
#include <fmt/core.h>

#include "abcg.hpp"
#include "gamedata.hpp"
#include "rocket.hpp"
#include "obstacles.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void handleEvent(SDL_Event& event) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
  GLuint m_program{};

  int m_viewportWidth{};
  int m_viewportHeight{};

  GameData m_gameData;
  Rocket m_rocket;
  Obstacles m_obstacles;

  abcg::ElapsedTimer m_restartWaitTimer;
  abcg::ElapsedTimer m_gameTime;

  ImFont* m_font{};

  std::default_random_engine m_randomEngine;

  void checkCollisions();
  void checkWinCondition();

  void restart();
  void update();
  void updateScore();
};

#endif