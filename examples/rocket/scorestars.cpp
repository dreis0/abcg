#include "scorestars.hpp"

#include <cppitertools/itertools.hpp>

void ScoreStars::initializeGL(GLuint program) {
  terminateGL();

  // Start pseudo-random number generator
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  m_program = program;
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  // Create obstacles
  m_stars.clear();
  m_stars.resize(1);

  for (auto &star : m_stars) {
    star = createScoreStar();
  }
}

void ScoreStars::paintGL() {
  abcg::glUseProgram(m_program);

  for (const auto &star : m_stars) {
    abcg::glBindVertexArray(star.m_vao);

    abcg::glUniform4fv(m_colorLoc, 1, &star.m_innerColor.r);
    abcg::glUniform4fv(m_colorLoc, 1, &star.m_outterColor.r);
    abcg::glUniform1f(m_scaleLoc, star.m_scale);

    abcg::glUniform2f(m_translationLoc, star.m_translation.x,
                      star.m_translation.y);

    abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, star.m_polygonSides + 2);

    abcg::glBindVertexArray(0);
  }

  abcg::glUseProgram(0);
}

void ScoreStars::terminateGL() {
  for (auto star : m_stars) {
    abcg::glDeleteBuffers(1, &star.m_vbo);
    abcg::glDeleteVertexArrays(1, &star.m_vao);
  }
}

void ScoreStars::update(const Rocket &rocket) {
  // add new obstacles after intervale
  if (m_new_star_timer.elapsed() >
      newStarInterval / (rocket.m_baseSpeed * 1000)) {
    m_new_star_timer.restart();

    std::uniform_real_distribution<float> m_random{0.0f, 1.0f};
    auto chance{m_random(m_randomEngine)};

    if (chance > (1.0f - newBonusChance)) {
      auto star{createScoreStar()};
      m_stars.push_back(star);
    }
  }

  // move obstacles down
  for (auto &star : m_stars) {
    star.m_translation.y -= rocket.m_baseSpeed + rocket.m_velocity.y;
  }

  // discard objects that are out of the screen
  m_stars.remove_if(
      [](const ScoreStars::ScoreStar &x) { return x.m_translation.y < -1.3f; });
}

ScoreStars::ScoreStar ScoreStars::createScoreStar() {
  ScoreStar star;
  auto &re{m_randomEngine};  // Shortcut

  std::uniform_real_distribution<float> randomXDist(-0.90f, 0.90f);
  auto randomX{randomXDist(re)};
  glm::vec2 translation{randomX, 0.90f};
  star.m_translation = translation;

  // Create geometry and color
  std::vector<glm::vec2> positions(0);
  std::vector<glm::vec3> colors(0);

  positions.emplace_back(0, 0);
  colors.push_back(star.m_innerColor);

  const auto step{M_PI * 2 / star.m_polygonSides};
  for (const auto angle : iter::range(0.0, M_PI * 2, step)) {
    positions.emplace_back(std::cos(angle), std::sin(angle));
    colors.push_back(star.m_outterColor);
  }

  positions.push_back(positions.at(1));
  colors.push_back(star.m_outterColor);

  // Generate VBO of colors
  abcg::glGenBuffers(1, &m_vboColors);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
  abcg::glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3),
                     colors.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate VBO of positions
  abcg::glGenBuffers(1, &star.m_vbo);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, star.m_vbo);
  abcg::glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
                     positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &star.m_vao);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(star.m_vao);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, star.m_vbo);
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);

  return star;
}
