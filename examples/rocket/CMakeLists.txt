project(rocket)

add_executable(${PROJECT_NAME} main.cpp openglwindow.cpp rocket.cpp obstacles.cpp)

enable_abcg(${PROJECT_NAME})
