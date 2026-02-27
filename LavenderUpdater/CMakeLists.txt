
cmake_minimum_required(VERSION 3.8)
project(LavenderUpdater LANGUAGES CXX RC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/resources.rc.in"
    "${CMAKE_CURRENT_BINARY_DIR}/resources.rc"
    @ONLY
)

set(SRC_FILES
  src/dllmain.cpp
  "src/core/updater_core.cpp"
  "src/misc/version_utils.cpp"
  "src/misc/http_client.cpp"
  "src/logger/logger.cpp"
  "${CMAKE_CURRENT_BINARY_DIR}/resources.rc"
)

set_source_files_properties(
    "${CMAKE_CURRENT_BINARY_DIR}/resources.rc"
    PROPERTIES COMPILE_FLAGS "/I\"${CMAKE_CURRENT_SOURCE_DIR}/src\""
)

add_library(LavenderUpdater SHARED ${SRC_FILES})

if (WIN32)
  target_link_libraries(LavenderUpdater PRIVATE winhttp dwmapi shell32 gdiplus)

  find_library(VERSION_LIB NAMES Version)
  if (VERSION_LIB)
    target_link_libraries(LavenderUpdater PRIVATE ${VERSION_LIB})
  else()
    message(WARNING "Version.lib not found; continuing without explicit link to Version.lib")
  endif()
endif()


