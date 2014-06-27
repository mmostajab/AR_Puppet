CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(CMAKE_BUILD_TYPE "Debug")

PROJECT(inversekinematics)

FIND_PACKAGE(GLUT)
FIND_PACKAGE(OpenGL)

SET(SRC
src/main.cpp
src/linkedstructure.cpp
)

ADD_EXECUTABLE(inversekinematics ${SRC})
TARGET_LINK_LIBRARIES(inversekinematics ${GLUT_LIBRARY} ${OPENGL_LIBRARY})
