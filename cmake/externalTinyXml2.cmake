include(ExternalProject)

set(TINYXML2_CMAKE_ARGS
  -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
  -DCMAKE_BUILD_TYPE=Release
  -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
  -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
  -DBUILD_STATIC_LIBS:BOOL=ON
)

ExternalProject_Add(libTinyXml2
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/srcs/tinyxml2

  GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
  GIT_TAG 6.0.0

  CMAKE_ARGS ${TINYXML2_CMAKE_ARGS}
)
