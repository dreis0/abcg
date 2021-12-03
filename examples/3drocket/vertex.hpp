#ifndef VERTEX_HPP_
#define VERTEX_HPP_

#include <glm/gtx/hash.hpp>

#include "abcg.hpp"


struct Vertex {
  glm::vec3 position{};
  glm::vec3 normal{};
  glm::vec2 texCoord{};
  glm::vec4 tangent{};

  bool operator==(const Vertex& other) const noexcept {
    static const auto epsilon{std::numeric_limits<float>::epsilon()};
    return glm::all(glm::epsilonEqual(position, other.position, epsilon)) &&
           glm::all(glm::epsilonEqual(normal, other.normal, epsilon)) &&
           glm::all(glm::epsilonEqual(texCoord, other.texCoord, epsilon));
  }
};

// Explicit specialization of std::hash for Vertex
namespace std {
template <>
struct hash<Vertex> {
  size_t operator()(Vertex const& vertex) const noexcept {
    const std::size_t h1{std::hash<glm::vec3>()(vertex.position)};
    const std::size_t h2{std::hash<glm::vec3>()(vertex.normal)};
    const std::size_t h3{std::hash<glm::vec2>()(vertex.texCoord)};
    
    return h1 ^ h2 ^ h3;
  }
};
}  // namespace std

#endif