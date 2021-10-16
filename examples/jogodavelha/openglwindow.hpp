#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include "abcg.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;

 private:
  std::array<float, 4> m_clearColor{0.906f, 0.910f, 0.918f, 1.0f};

 private:
  std::array<std::array<char, 3>, 3> gameGrid{
      {{'\0', '\0', '\0'}, {'\0', '\0', '\0'}, {'\0', '\0', '\0'}}};

  int movesCount;
  int idxClicked = -1;
  bool jogoEncerrado{false};

  char CheckIfGameIsOver();
};

#endif