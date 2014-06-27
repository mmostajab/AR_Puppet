set(CMAKE_RELEASE_DIR "../bin/release")
# debug binary folder
set(CMAKE_DEBUG_DIR "../bin/debug")


# find openCV
set(CMAKE_MODULE_PATH ${CMAKE_INSTALL_PREFIX}/lib/cmake/ )
find_package(OpenCV REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Eigen3 REQUIRED)


include_directories(${EIGEN3_INCLUDE_DIR})
add_executable (inversekinematics main.cpp linkedstructure)
target_link_libraries (inversekinematics ${OpenCV_LIBS} ${GLUT_LIBRARY} ${OPENGL_LIBRARY})
# ${PCL_LIBRARIES} ${OpenCV_LIBS} ${cvsba_LIBS})

if(CMAKE_BUILD_TYPE STREQUAL Debug)
	set_target_properties(inversekinematics PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_DEBUG_DIR})
	set_target_properties(inversekinematics PROPERTIES OUTPUT_NAME inversekinematics${BUILD_PREFIX})
else()
	set_target_properties(inversekinematics PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RELEASE_DIR})
endif()