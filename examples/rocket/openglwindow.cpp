#include "openglwindow.hpp"

#include <imgui.h>
#include <stdlib.h>

#include "abcg.hpp"

void OpenGLWindow::handleEvent(SDL_Event& event) {
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_SPACE)
      m_gameData.m_input.set(static_cast<size_t>(Input::Sprint));
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_input.set(static_cast<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_input.set(static_cast<size_t>(Input::Right));
  }

  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_SPACE)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Sprint));
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Right));
  }
}

void OpenGLWindow::initializeGL() {
  // Load a new font
  ImGuiIO& io{ImGui::GetIO()};
  const auto filename{getAssetsPath() + "Inconsolata-Medium.ttf"};
  m_font = io.Fonts->AddFontFromFileTTF(filename.c_str(), 60.0f);
  if (m_font == nullptr) {
    throw abcg::Exception{abcg::Exception::Runtime("Cannot load font file")};
  }

  m_program = createProgramFromFile(getAssetsPath() + "objects.vert",
                                    getAssetsPath() + "objects.frag");

  abcg::glClearColor(0, 0, 0, 1);

#if !defined(__EMSCRIPTEN__)
  abcg::glEnable(GL_PROGRAM_POINT_SIZE);
#endif

  // Start pseudo-random number generator
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  restart();
}

void OpenGLWindow::restart() {
  m_gameTime.restart();

  m_gameData.m_state = State::Playing;
  m_gameData.m_score = 0;
  m_rocket.m_baseSpeed = 0.0050f;

  m_gameData.m_input.reset(static_cast<size_t>(Input::Sprint));
  m_gameData.m_input.reset(static_cast<size_t>(Input::Left));
  m_gameData.m_input.reset(static_cast<size_t>(Input::Right));

  m_rocket.initializeGL(m_program);
  m_obstacles.initializeGL(m_program);
  m_scoresStars.initializeGL(m_program);
}

void OpenGLWindow::update() {
  const float deltaTime{static_cast<float>(getDeltaTime())};

  m_rocket.update(m_gameData, deltaTime);
  m_obstacles.update(m_rocket);
  m_scoresStars.update(m_rocket);

  if (m_gameData.m_state == State::Playing) {
    checkCollisions();
    updateScore();
  }
}

void OpenGLWindow::paintGL() {
  update();

  abcg::glClear(GL_COLOR_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  m_rocket.paintGL(m_gameData);
  m_obstacles.paintGL();
  m_scoresStars.paintGL();
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();

  // Game has ended
  if (m_gameData.m_state == State::GameOver ||
      m_gameData.m_state == State::Win) {
    const auto size{ImVec2(300, 450)};
    const auto position{ImVec2((m_viewportWidth - size.x) / 2.0f,
                               (m_viewportHeight - size.y) / 2.0f)};

    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags flags{ImGuiWindowFlags_NoBackground |
                           ImGuiWindowFlags_NoTitleBar};
    ImGui::Begin(" ", nullptr, flags);
    ImGui::PushFont(m_font);

    if (m_gameData.m_state == State::GameOver) {
      ImGui::Text(
          fmt::format("Game Over!\n Score {}", m_gameData.m_score).c_str());
      ImGui::Button("Restart", ImVec2(300, 80));

      bool click = ImGui::IsItemClicked();
      if (click) restart();

    } else if (m_gameData.m_state == State::Win) {
      ImGui::Text("*You Win!*");
    }

    ImGui::PopFont();
    ImGui::End();
  } else {  // Game is going

    const auto size{ImVec2(300, 85)};
    ImGui::SetNextWindowPos(
        ImVec2(m_viewportWidth - size.x + 200, m_viewportHeight - size.y));

    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags flags{ImGuiWindowFlags_NoBackground |
                           ImGuiWindowFlags_NoTitleBar |
                           ImGuiWindowFlags_NoInputs};

    ImGui::Begin(" ", nullptr, flags);
    ImGui::PushFont(m_font);

    ImGui::Text(fmt::format("{}", m_gameData.m_score).c_str());
    // ImGui::Text(fmt::format("{}", m_gameTime.elapsed()).c_str());

    ImGui::PopFont();
    ImGui::End();
  }
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::terminateGL() {
  abcg::glDeleteProgram(m_program);

  m_rocket.terminateGL();
  m_obstacles.terminateGL();
  m_scoresStars.terminateGL();
}

void OpenGLWindow::checkCollisions() {
  // Check collision between rocket and obstacles
  for (const auto& obstacle : m_obstacles.m_obstacles) {
    const auto obstacleTranslation{obstacle.m_translation};
    const auto distance{
        glm::distance(m_rocket.m_translation, obstacleTranslation)};

    if (distance < m_rocket.m_scale * 0.45f + obstacle.m_scale * 0.85f) {
      m_gameData.m_state = State::GameOver;
    }
  }

  // Check colisions between rocket and score stars
  for (auto& star : m_scoresStars.m_stars) {
    const auto translation{star.m_translation};
    const auto distance{glm::distance(m_rocket.m_translation, translation)};

    if (distance < m_rocket.m_scale * 0.45f + star.m_scale * 0.85f) {
      m_gameData.m_score += 20;
      star.m_hit = true;
    }
  }

  // Remove start that where collected from screen
  m_scoresStars.m_stars.remove_if(
      [](const ScoreStars::ScoreStar& x) { return x.m_hit; });
}

void OpenGLWindow::updateScore() {
  double gameTime = m_gameTime.elapsed();
  if (gameTime > 1) {
    m_gameTime.restart();
    m_gameData.m_score += 1;
    m_rocket.m_baseSpeed += 0.0001f;

    // Give bonus score for accelerating
    int bonus = (int)abs(m_rocket.m_velocity.y / m_rocket.m_acceleration);
    m_gameData.m_score += bonus;
  }
}
