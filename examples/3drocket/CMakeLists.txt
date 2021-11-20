project(3drocket)
add_executable(${PROJECT_NAME} camera.cpp main.cpp openglwindow.cpp rocket.cpp vertex.cpp)
enable_abcg(${PROJECT_NAME})
