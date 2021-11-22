project(3drocket)
add_executable(${PROJECT_NAME} vertex.hpp camera.cpp main.cpp rocket.cpp openglwindow.cpp asteroid.cpp model.cpp)
enable_abcg(${PROJECT_NAME})
