#include "obstacles.hpp"

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

void Obstacles::initializeGL(GLuint program) {
  terminateGL();

  // Start pseudo-random number generator
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  m_program = program;
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  // Create obstacles
  m_obstacles.clear();
  std::uniform_int_distribution<int> range(0, 3);
  m_obstacles.resize(range(m_randomEngine));

  for (auto &obstacle : m_obstacles) {
    obstacle = createObstacle();
  }
}

void Obstacles::paintGL() {
  abcg::glUseProgram(m_program);

  for (const auto &obstacle : m_obstacles) {
    abcg::glBindVertexArray(obstacle.m_vao);

    abcg::glUniform4fv(m_colorLoc, 1, &obstacle.m_color.r);
    abcg::glUniform1f(m_scaleLoc, obstacle.m_scale);

    abcg::glUniform2f(m_translationLoc, obstacle.m_translation.x,
                      obstacle.m_translation.y);

    abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, obstacle.m_polygonSides + 2);

    abcg::glBindVertexArray(0);
  }

  abcg::glUseProgram(0);
}

void Obstacles::terminateGL() {
  for (auto obstacle : m_obstacles) {
    abcg::glDeleteBuffers(1, &obstacle.m_vbo);
    abcg::glDeleteVertexArrays(1, &obstacle.m_vao);
  }
}

void Obstacles::update(const Rocket &rocket, float deltaTime) {
  // add new obstacles after interval
  if (m_new_obstacle_timer.elapsed() >
      newObstacleInterval / (rocket.m_baseSpeed * 1000)) {
    m_new_obstacle_timer.restart();
    std::uniform_int_distribution<int> range(0, 2);

    std::vector<int> els(range(m_randomEngine));

    for (int i : els) {
      auto obstacle{createObstacle()};
      m_obstacles.push_back(obstacle);
    }
  }

  // move obstacles down
  for (auto &obstacle : m_obstacles) {
    obstacle.m_translation.y -= rocket.m_baseSpeed + rocket.m_velocity.y;
    std::uniform_int_distribution<int> range(1, 10);
    auto randomValue{range(m_randomEngine)};
  }

  // discard objects that are out of the screen
  m_obstacles.remove_if(
      [](const Obstacles::Obstacle &x) { return x.m_translation.y < -1.3f; });
}

Obstacles::Obstacle Obstacles::createObstacle() {
  Obstacle obstacle;
  std::uniform_real_distribution<float> randomScaleRange(0.05f, 0.22f);
  float scale = randomScaleRange(m_randomEngine);

  std::uniform_real_distribution<float> randomXDist(-0.90f, 0.90f);
  auto randomX{randomXDist(m_randomEngine)};

  glm::vec2 translation{randomX, 0.80f};

  auto &re{m_randomEngine};  // Shortcut

  // Randomly choose the number of sides
  std::uniform_int_distribution<int> randomSides(6, 20);
  obstacle.m_polygonSides = randomSides(re);

  // Choose a random color
  std::uniform_real_distribution<float> randomIntensity_low(0.1f, 0.5f);
  std::uniform_real_distribution<float> randomIntensity_high(0.5f, 0.8f);
  const glm::vec4 color{randomIntensity_low(re), randomIntensity_high(re),
                        randomIntensity_high(re), randomIntensity_low(re)};
  obstacle.m_color = color;
  // obstacle.m_color = glm::vec4(1) * randomIntensity(re);

  obstacle.m_color.a = 1.0f;
  obstacle.m_scale = scale;
  obstacle.m_translation = translation;

  // Choose a random direction
  glm::vec2 direction{m_randomDist(re), m_randomDist(re)};
  obstacle.m_velocity = glm::normalize(direction) / 7.0f;

  // Create geometry
  std::vector<glm::vec2> positions(0);
  positions.emplace_back(0, 0);
  const auto step{M_PI * 2 / obstacle.m_polygonSides};
  std::uniform_real_distribution<float> randomRadius(0.8f, 1.0f);
  for (const auto angle : iter::range(0.0, M_PI * 2, step)) {
    const auto radius{randomRadius(re)};
    positions.emplace_back(radius * std::cos(angle), radius * std::sin(angle));
  }
  positions.push_back(positions.at(1));

  // Generate VBO
  abcg::glGenBuffers(1, &obstacle.m_vbo);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, obstacle.m_vbo);
  abcg::glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
                     positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &obstacle.m_vao);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(obstacle.m_vao);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, obstacle.m_vbo);
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);

  return obstacle;
}
