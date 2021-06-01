cmake_minimum_required(VERSION 3.13)

project(PatiensDBManager LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Sql Widgets REQUIRED)

set( SRC_DIR ${PROJECT_SOURCE_DIR}/src )

set( CPP
        ${SRC_DIR}/main.cpp
        ${SRC_DIR}/view/add_patient_dlg.cpp
        ${SRC_DIR}/view/main_window.cpp
        ${SRC_DIR}/view/patient_info_form.cpp
        ${SRC_DIR}/view/photo_viewer.cpp
        ${SRC_DIR}/view/table_view_ex.cpp
        ${SRC_DIR}/view/date_edit_ex.cpp
        ${SRC_DIR}/utility/global.cpp
        ${SRC_DIR}/utility/utility.cpp
        ${SRC_DIR}/model/data_types.cpp
        ${SRC_DIR}/model/database.cpp
        ${SRC_DIR}/model/delegates.cpp
        ${SRC_DIR}/model/horizontal_proxy_model.cpp )

set( H/HPP
        ${SRC_DIR}/view/add_patient_dlg.h
        ${SRC_DIR}/view/main_window.h
        ${SRC_DIR}/view/patient_info_form.h
        ${SRC_DIR}/view/photo_viewer.h
        ${SRC_DIR}/view/table_view_ex.h
        ${SRC_DIR}/view/date_edit_ex.h
        ${SRC_DIR}/utility/global.h
        ${SRC_DIR}/utility/utility.h
        ${SRC_DIR}/model/data_types.h
        ${SRC_DIR}/model/database.h
        ${SRC_DIR}/model/delegates.h
        ${SRC_DIR}/model/horizontal_proxy_model.h )

set( RESOURCE_FILES
        ${PROJECT_SOURCE_DIR}/res/resources.qrc )

add_executable( ${PROJECT_NAME} ${CPP} ${H/HPP} ${RESOURCE_FILES} )

target_include_directories( ${PROJECT_NAME} PUBLIC ${SRC_DIR} )
target_link_libraries( ${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Core Qt5::Sql )
