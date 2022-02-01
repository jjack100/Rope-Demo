cmake_minimum_required( VERSION 3.17 )

project( Rope-Demo )

#GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory( glfw )

#Glad
add_library(glad
  STATIC
    glad/src/glad.c
)

target_include_directories(glad
  PRIVATE
    glad/include
)

add_executable( ${PROJECT_NAME} )

target_sources(${PROJECT_NAME}
  PRIVATE
    src/main.c
    src/shader.h
    src/shader.c
)

target_include_directories(${PROJECT_NAME}
  PRIVATE
    glad/include
    include
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    glfw
    glad
)
