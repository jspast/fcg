# Este arquivo CMakeLists.txt foi adaptado a partir do projeto castor
# do PET INF/UFRGS (https://github.com/petcomputacaoufrgs/castor-fcg).

# Todos os arquivos que devem ser compilados.
set(SOURCES
  src/main.cpp
  src/matrices.cpp
  src/camera.cpp
  src/textrendering.cpp
  src/hud.cpp
)

cmake_minimum_required(VERSION 3.11.0)

project(LAB_FCG VERSION 1.0.0)

set(CMAKE_CXX_STANDARD          11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

if(WIN32)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${PROJECT_SOURCE_DIR}/bin/Debug")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/bin/Release")
elseif(UNIX)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/Linux")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
message(STATUS
  "Build type: ${CMAKE_BUILD_TYPE}

               Change the build type on the command line with

                   -DCMAKE_BUILD_TYPE=type

               for type in {Release, Debug, RelWithDebInfo}.
")

set(EXECUTABLE_NAME main)

# Verifica se todos os arquivos fonte estão presentes no diretório
# atual. Se não estão, avisa sobre CMakeLists mal configurado.
foreach(source_file IN LISTS SOURCES)
  if(NOT EXISTS ${PROJECT_SOURCE_DIR}/${source_file})
    message(FATAL_ERROR "
O arquivo ${PROJECT_SOURCE_DIR}/${source_file} não existe.
Por favor, atualize a lista de arquivos fonte no arquivo CMakeLists.txt.")
    break()
  endif()
endforeach()

add_executable(${EXECUTABLE_NAME} ${SOURCES})

target_include_directories(${EXECUTABLE_NAME} BEFORE PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${EXECUTABLE_NAME} BEFORE PRIVATE ${PROJECT_SOURCE_DIR}/external/glm)
target_include_directories(${EXECUTABLE_NAME} BEFORE PRIVATE ${PROJECT_SOURCE_DIR}/external)

if(WIN32)

  target_link_libraries(${EXECUTABLE_NAME} glfw gdi32 opengl32)

elseif(UNIX)

  target_compile_options(${EXECUTABLE_NAME} PRIVATE -Wall -Wno-unused-function)

  # Add custom target for 'run'
  add_custom_target(run
      COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} ./main
      DEPENDS main
      USES_TERMINAL
  )

  find_package(OpenGL REQUIRED)
  find_package(X11 REQUIRED)
  find_library(MATH_LIBRARY m)
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)
  target_link_libraries(${EXECUTABLE_NAME}
    ${CMAKE_DL_LIBS}
    ${MATH_LIBRARY}
    glfw
    ${CMAKE_THREAD_LIBS_INIT}
    ${OPENGL_LIBRARIES}
    ${X11_LIBRARIES}
    ${X11_Xrandr_LIB}
    ${X11_Xcursor_LIB}
    ${X11_Xinerama_LIB}
    ${X11_Xxf86vm_LIB}
  )

endif()
