#include "asteroid.hpp"

#include <fmt/core.h>
#include <stdlib.h>
#include <tiny_obj_loader.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtx/hash.hpp>
#include <unordered_map>

#include "abcg.hpp"
#include "vertex.hpp"

void Asteroid::createBuffers() {
  // Delete previous buffers
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);

  // VBO
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices[0]) * m_vertices.size(),
                     m_vertices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // EBO
  abcg::glGenBuffers(1, &m_EBO);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(m_indices[0]) * m_indices.size(), m_indices.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Asteroid::loadObj(std::string_view path, bool standardize) {
  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path.data())) {
    if (!reader.Error().empty()) {
      throw abcg::Exception{abcg::Exception::Runtime(
          fmt::format("Failed to load model {} ({})", path, reader.Error()))};
    }
    throw abcg::Exception{
        abcg::Exception::Runtime(fmt::format("Failed to load model {}", path))};
  }

  if (!reader.Warning().empty()) {
    fmt::print("Warning: {}\n", reader.Warning());
  }

  const auto& attrib{reader.GetAttrib()};
  const auto& shapes{reader.GetShapes()};

  m_vertices.clear();
  m_indices.clear();

  // A key:value map with key=Vertex and value=index
  std::unordered_map<Vertex, GLuint> hash{};

  // Loop over shapes
  for (const auto& shape : shapes) {
    // Loop over indices
    for (const auto offset : iter::range(shape.mesh.indices.size())) {
      // Access to vertex
      const tinyobj::index_t index{shape.mesh.indices.at(offset)};

      // Vertex position
      const int startIndex{3 * index.vertex_index};
      const float vx{attrib.vertices.at(startIndex + 0)};
      const float vy{attrib.vertices.at(startIndex + 1)};
      const float vz{attrib.vertices.at(startIndex + 2)};

      Vertex vertex{};
      vertex.position = {vx, vy, vz};

      // If hash doesn't contain this vertex
      if (hash.count(vertex) == 0) {
        // Add this index (size of m_vertices)
        hash[vertex] = m_vertices.size();
        // Add this vertex
        m_vertices.push_back(vertex);
      }

      m_indices.push_back(hash[vertex]);
    }
  }

  if (standardize) {
    this->standardize();
  }

  createBuffers();
}

void Asteroid::render(GLint m_program) const {
  abcg::glBindVertexArray(m_VAO);

  const GLint modelMatrixLoc{
      abcg::glGetUniformLocation(m_program, "modelMatrix")};
  const GLint colorLoc{abcg::glGetUniformLocation(m_program, "color")};

  // Determine entity's intial position
  glm::mat4 model{1.0f};
  model = glm::mat4(1.0);
  model = glm::translate(model, m_direction);  // send it forward
  model = glm::scale(model, glm::vec3(0.3f));

  // Move entity throughout the scene
  float elapsedTime = (float)m_timer.elapsed();
  // model = glm::translate(model, (m_direction + elapsedTime));
  model =
      glm::rotate(model, glm::radians(5.0f) * elapsedTime, glm::vec3(0, 1, 0));

  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(colorLoc, m_color[0], m_color[1], m_color[2], m_color[3]);
  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
                       nullptr);

  abcg::glBindVertexArray(0);
}

void Asteroid::init(GLuint program) {
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  std::uniform_real_distribution<float> range(-1.0f, 1.0f);
  m_spinDirection = range(m_randomEngine);

  m_direction = glm::vec3(range(m_randomEngine), range(m_randomEngine),
                          range(m_randomEngine));

  m_timer.restart();

  // Release previous VAO
  abcg::glDeleteVertexArrays(1, &m_VAO);

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO);
  abcg::glBindVertexArray(m_VAO);

  // Bind EBO and VBO
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

  // Bind vertex attributes
  const GLint positionAttribute{
      abcg::glGetAttribLocation(program, "inPosition")};
  if (positionAttribute >= 0) {
    abcg::glEnableVertexAttribArray(positionAttribute);
    abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                                sizeof(Vertex), nullptr);
  }

  // End of binding
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
  abcg::glBindVertexArray(0);
}

void Asteroid::standardize() {
  // Center to origin and normalize largest bound to [-1, 1]

  // Get bounds
  glm::vec3 max(std::numeric_limits<float>::lowest());
  glm::vec3 min(std::numeric_limits<float>::max());
  for (const auto& vertex : m_vertices) {
    max.x = std::max(max.x, vertex.position.x);
    max.y = std::max(max.y, vertex.position.y);
    max.z = std::max(max.z, vertex.position.z);
    min.x = std::min(min.x, vertex.position.x);
    min.y = std::min(min.y, vertex.position.y);
    min.z = std::min(min.z, vertex.position.z);
  }

  // Center and scale
  const auto center{(min + max) / 2.0f};
  const auto scaling{2.0f / glm::length(max - min)};
  for (auto& vertex : m_vertices) {
    vertex.position = (vertex.position - center) * scaling;
  }
}

void Asteroid::terminateGL() {
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}