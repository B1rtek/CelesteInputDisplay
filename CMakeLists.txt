cmake_minimum_required(VERSION 3.11)
project(CelesteInputDisplay)

set(CMAKE_CXX_STANDARD 14)

include(FetchContent)

set(SFML_VERSION "2.5.1")

FetchContent_Declare(
        sfml
        GIT_REPOSITORY "https://github.com/SFML/SFML.git"
        GIT_TAG        "${SFML_VERSION}"
)

FetchContent_GetProperties(sfml)
if(NOT sfml_POPULATED)
    FetchContent_Populate(sfml)
    add_subdirectory(${sfml_SOURCE_DIR} ${sfml_BINARY_DIR})
endif()

add_executable(${PROJECT_NAME} WIN32 main.cpp)
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-main)