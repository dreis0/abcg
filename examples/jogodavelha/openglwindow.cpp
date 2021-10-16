#include "openglwindow.hpp"

#include <fmt/core.h>
#include <imgui.h>

void OpenGLWindow::initializeGL() {
  auto windowSettings{getWindowSettings()};
  fmt::print("Initial window size: {}x{}\n", windowSettings.width,
             windowSettings.height);
}

void OpenGLWindow::paintGL() {
  // Set the clear color
  abcg::glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2],
                     m_clearColor[3]);
  // Clear the color buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::paintUI() {
  // Window begin
  ImGui::Begin("Jogo da velha");
  char winner = CheckIfGameIsOver();

  if (winner != '\0') {
    std::string msg = "";
    msg += winner;
    msg += " is the winner!";

    ImGui::Text(msg.c_str());
  } else if (!jogoEncerrado && movesCount % 2 == 0) {
    ImGui::Text("X turn");
  } else if (!jogoEncerrado) {
    ImGui::Text("O turn");
  } else {
    ImGui::Text("Draw!");
  }

  for (int i = 0; i < 9; i++) {
    char c = gameGrid[i / 3][i % 3];
    ImGui::Button(&c, ImVec2(200, 200));

    bool click = ImGui::IsItemClicked();

    if (!jogoEncerrado && c == '\0' && click) {
      gameGrid[i / 3][i % 3] = movesCount % 2 == 0 ? 'X' : 'O';
      movesCount++;
    }

    if ((i + 1) % 3 != 0) {
      ImGui::SameLine();
    }
  }

  bool reset = ImGui::Button("Restart game", ImVec2(-1, 50));
  if (reset) {
    for (int i = 0; i < 9; i++) {
      gameGrid[i / 3][i % 3] = '\0';
    }
    movesCount = 0;
  }

  // Window end
  ImGui::End();
}

char OpenGLWindow::CheckIfGameIsOver() {
  jogoEncerrado = true;

  // verificando linhas
  for (size_t i = 0; i < 3; i++) {
    if (gameGrid[i][0] != '\0' && gameGrid[i][0] == gameGrid[i][1] &&
        gameGrid[i][0] == gameGrid[i][2])
      return gameGrid[i][0];
  }

  // verificando linhas
  for (size_t i = 0; i < 3; i++) {
    if (gameGrid[0][i] != '\0' && gameGrid[0][i] == gameGrid[1][i] &&
        gameGrid[0][i] == gameGrid[2][i])
      return gameGrid[0][i];
  }

  // verificando diagonais
  if (gameGrid[0][0] != '\0' && gameGrid[0][0] == gameGrid[1][1] &&
      gameGrid[0][0] == gameGrid[2][2]) {
    return gameGrid[0][0];
  }

  if (gameGrid[0][2] != '\0' && gameGrid[0][2] == gameGrid[1][1] &&
      gameGrid[0][2] == gameGrid[2][0]) {
    return gameGrid[0][2];
  }

  jogoEncerrado = movesCount == 9;
  return '\0';
}