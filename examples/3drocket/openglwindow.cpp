#include "openglwindow.hpp"

#include <fmt/core.h>
#include <imgui.h>
#include <tiny_obj_loader.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/hash.hpp>
#include <string>
#include <unordered_map>

void OpenGLWindow::handleEvent(SDL_Event& ev) {
  if (ev.type == SDL_KEYDOWN) {
    if (ev.key.keysym.sym == SDLK_UP || ev.key.keysym.sym == SDLK_w)
      m_dollySpeed = 1.0f;
    if (ev.key.keysym.sym == SDLK_DOWN || ev.key.keysym.sym == SDLK_s)
      m_dollySpeed = -1.0f;
    if (ev.key.keysym.sym == SDLK_LEFT || ev.key.keysym.sym == SDLK_a)
      m_panSpeed = -1.0f;
    if (ev.key.keysym.sym == SDLK_RIGHT || ev.key.keysym.sym == SDLK_d)
      m_panSpeed = 1.0f;
    if (ev.key.keysym.sym == SDLK_q) m_truckSpeed = -1.0f;
    if (ev.key.keysym.sym == SDLK_e) m_truckSpeed = 1.0f;
  }

  if (ev.type == SDL_KEYUP) {
    if ((ev.key.keysym.sym == SDLK_UP || ev.key.keysym.sym == SDLK_w) &&
        m_dollySpeed > 0)
      m_dollySpeed = 0.0f;
    if ((ev.key.keysym.sym == SDLK_DOWN || ev.key.keysym.sym == SDLK_s) &&
        m_dollySpeed < 0)
      m_dollySpeed = 0.0f;
    if ((ev.key.keysym.sym == SDLK_LEFT || ev.key.keysym.sym == SDLK_a) &&
        m_panSpeed < 0)
      m_panSpeed = 0.0f;
    if ((ev.key.keysym.sym == SDLK_RIGHT || ev.key.keysym.sym == SDLK_d) &&
        m_panSpeed > 0)
      m_panSpeed = 0.0f;
    if (ev.key.keysym.sym == SDLK_q && m_truckSpeed < 0) m_truckSpeed = 0.0f;
    if (ev.key.keysym.sym == SDLK_e && m_truckSpeed > 0) m_truckSpeed = 0.0f;
  }

  if (ev.type == SDL_MOUSEWHEEL) {
    m_yMovement += (ev.wheel.y > 0 ? 1.0f : -1.0f) * 0.01;
    m_yMovement = glm::clamp(m_yMovement, -2.5f, 2.5f);
  } else {
    m_yMovement = 0;
  }
}

void OpenGLWindow::initializeGL() {
  abcg::glClearColor(0, 0, 0, 1);

  // Enable depth buffering
  abcg::glEnable(GL_DEPTH_TEST);

  // Create program
  m_program = createProgramFromFile(getAssetsPath() + "lookat.vert",
                                    getAssetsPath() + "lookat.frag");

  // Load rocket
  m_rocket.loadObj(getAssetsPath() + "rocket.obj");

  // Load asteroids
  m_asteroids.resize(m_qtd_asteroids);

  // Load star
  m_model.loadObj(getAssetsPath() + "asteroid1.obj");
  m_model.setupVAO(m_program);

  int idx = 0;
  for (auto& asteroid : m_asteroids) {
    auto filename{getAssetsPath() + "asteroid" + std::to_string((idx % 3) + 1) +
                  ".obj"};

    asteroid.loadObj(filename);
    idx++;
  }

  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  m_rocket.init(m_program);
  for (auto& asteroid : m_asteroids) {
    asteroid.init(m_program);
  }

  // Setup stars
  for (const auto index : iter::range(m_numStars)) {
    auto& position{m_starPositions.at(index)};
    auto& rotation{m_starRotations.at(index)};

    randomizeStar(position, rotation);
  }

  resizeGL(getWindowSettings().width, getWindowSettings().height);
}

void OpenGLWindow::randomizeStar(glm::vec3& position, glm::vec3& rotation) {
  // Get random position
  // x and y coordinates in the range [-8, 8]
  // z coordinates in the range [-100, 50]
  std::uniform_real_distribution<float> distPosXY(-8.0f, 8.0f);
  std::uniform_real_distribution<float> distPosZ(-50.0f, 50.0f);

  position = glm::vec3(distPosXY(m_randomEngine), distPosXY(m_randomEngine),
                       distPosZ(m_randomEngine));

  //  Get random rotation axis
  std::uniform_real_distribution<float> distRotAxis(-1.0f, 1.0f);

  rotation = glm::normalize(glm::vec3(distRotAxis(m_randomEngine),
                                      distRotAxis(m_randomEngine),
                                      distRotAxis(m_randomEngine)));
}

void OpenGLWindow::paintGL() {
  update();

  // Clear color buffer and depth buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  abcg::glUseProgram(m_program);

  // Get location of uniform variables (could be precomputed)
  const GLint viewMatrixLoc{
      abcg::glGetUniformLocation(m_program, "viewMatrix")};
  const GLint projMatrixLoc{
      abcg::glGetUniformLocation(m_program, "projMatrix")};
  const GLint modelMatrixLoc{
      abcg::glGetUniformLocation(m_program, "modelMatrix")};
  const GLint colorLoc{abcg::glGetUniformLocation(m_program, "color")};

  // Set uniform variables for viewMatrix and projMatrix
  // These matrices are used for every scene object
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE,
                           &m_camera.m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE,
                           &m_camera.m_projMatrix[0][0]);
  abcg::glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);  // White

  m_rocket.render(m_program);
  for (auto& asteroid : m_asteroids) {
    asteroid.render(m_program);
  }

  // Render each star
  for (const auto index : iter::range(m_numStars)) {
    const auto& position{m_starPositions.at(index)};
    const auto& rotation{m_starRotations.at(index)};

    // Compute model matrix of the current star
    glm::mat4 modelMatrix{1.0f};
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
    modelMatrix = glm::rotate(modelMatrix, m_angle, rotation);

    // Set uniform variable
    abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);

    m_model.render();
  }

  abcg::glUseProgram(0);
}

void OpenGLWindow::paintUI() { abcg::OpenGLWindow::paintUI(); }

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  m_camera.computeProjectionMatrix(width, height);
}

void OpenGLWindow::terminateGL() {
  m_model.terminateGL();
  abcg::glDeleteProgram(m_program);

  m_rocket.terminateGL();
  for (auto& asteroid : m_asteroids) {
    asteroid.terminateGL();
  }
  m_asteroids.clear();
}

void OpenGLWindow::update() {
  const float deltaTime{static_cast<float>(getDeltaTime())};
  m_angle = glm::wrapAngle(m_angle + glm::radians(90.0f) * deltaTime);

  // Update LookAt camera
  m_camera.dolly(m_dollySpeed * deltaTime);
  m_camera.truck(m_truckSpeed * deltaTime);
  m_camera.pan(m_panSpeed * deltaTime);
  m_camera.moveY(m_yMovement * deltaTime);

  // Update stars
  for (const auto index : iter::range(m_numStars)) {
    auto& position{m_starPositions.at(index)};
    auto& rotation{m_starRotations.at(index)};

    // Z coordinate increases by 4 units per second
    position.z += deltaTime * 4.0f;

    // If this star is behind the camera, select a new random position and
    // orientation, and move it back to -50
    if (position.z > 50.0f) {
      randomizeStar(position, rotation);
      position.z = -50.0f;  // Back to -50
    }
  }
}