
cmake_minimum_required(VERSION 3.8)
project(LavenderUpdater LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_FILES
  src/dllmain.cpp
  src/updater_core.cpp
  src/version_utils.cpp
  src/http_client.cpp
  src/logger.cpp
)

add_library(LavenderUpdater SHARED ${SRC_FILES})

if (WIN32)
  target_link_libraries(LavenderUpdater PRIVATE winhttp)

  # Try to link Version.lib if available (contains FileVersion APIs on Windows)
  find_library(VERSION_LIB NAMES Version)
  if (VERSION_LIB)
    target_link_libraries(LavenderUpdater PRIVATE ${VERSION_LIB})
  else()
    message(WARNING "Version.lib not found; continuing without explicit link to Version.lib")
  endif()
endif()


