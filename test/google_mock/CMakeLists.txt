cmake_minimum_required(VERSION 2.8.8)
project(google_mock_builder C CXX)
include(ExternalProject)

ExternalProject_Add(google_test
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG master
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/google_test
    INSTALL_COMMAND ""
)

set(GOOGLE_MOCK_INCLUDE_DIRS googlemock/include PARENT_SCOPE)

set(GOOGLE_MOCK_LIBS_DIR googlemock/ PARENT_SCOPE)
