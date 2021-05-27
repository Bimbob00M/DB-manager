cmake_minimum_required(VERSION 3.14)

project(PatiensDBManager LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Sql Widgets REQUIRED)

file(GLOB_RECURSE SOURCES
            ${PROJECT_SOURCE_DIR}
            "*.cpp"
            "*.hpp"
            "*.h" )

add_executable( ${PROJECT_NAME} ${SOURCES} )

target_include_directories( ${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src )

target_link_libraries( ${PROJECT_NAME} PRIVATE Qt5::Widgets  Qt5::Core Qt5::Sql )
