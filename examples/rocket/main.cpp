#include <fmt/core.h>

#include "abcg.hpp"
#include "openglwindow.hpp"

int main(int argc, char **argv) {
  try {
    abcg::Application app(argc, argv);

    auto window{std::make_unique<OpenGLWindow>()};
    window->setOpenGLSettings({.samples = 4, .vsync = true});
    window->setWindowSettings({.width = 1000,
                               .height = 1000,
                               .showFPS = false,
                               .showFullscreenButton = false,
                               .title = "Rocket Trip"});
    app.run(std::move(window));
  } catch (const abcg::Exception &exception) {
    fmt::print(stderr, "{}\n", exception.what());
    return -1;
  }
  return 0;
}