set(CMAKE_RELEASE_DIR "../bin/release")
# debug binary folder
set(CMAKE_DEBUG_DIR "../bin/debug")


# find openCV
set(CMAKE_MODULE_PATH ${CMAKE_INSTALL_PREFIX}/lib/cmake/ )
find_package(OpenCV REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(GLFW3 REQUIRED)
find_package(aruco REQUIRED)

include_directories(${EIGEN3_INCLUDE_DIR} ${GLFW_INCLUDE_DIRS})
add_executable (ARPuppet
  myGL.h
  main.cpp
  MarkerTracker.cpp
  color.h
  link.h
  myBiped.h
  linkedstructure.cpp
  combineWindowHandler.h)

target_link_libraries (ARPuppet ${OpenCV_LIBS} ${GLUT_LIBRARY} ${OPENGL_LIBRARY} ${GLFW_LIBRARIES} ${aruco_LIBS})

if(CMAKE_BUILD_TYPE STREQUAL Debug)
	set_target_properties(ARPuppet PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_DEBUG_DIR})
	set_target_properties(ARPuppet PROPERTIES OUTPUT_NAME ARPuppet${BUILD_PREFIX})
else()
	set_target_properties(ARPuppet PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RELEASE_DIR})
endif()
