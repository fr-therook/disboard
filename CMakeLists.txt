cmake_minimum_required(VERSION 3.16)

project(disboard VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

find_package(Qt6 6.2 COMPONENTS Quick REQUIRED)
get_target_property(QMAKE Qt::qmake IMPORTED_LOCATION)

qt_add_library(disboard STATIC)

add_subdirectory(impl)

qt_add_qml_module(disboard
        URI disboard
        VERSION 1.0
        RESOURCE_PREFIX /imports
        QML_FILES Board.qml
        )

set_target_properties(disboard PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
        )

target_compile_definitions(disboard
        PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(disboard
        PRIVATE Qt6::Quick disboardImplplugin)

target_include_directories(disboard PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Example Project
qt_add_executable(ExampleProject example/example.cpp)
qt_add_qml_module(ExampleProject
        URI ExampleProjectApp
        VERSION 1.0
        RESOURCE_PREFIX /imports
        QML_FILES example/example.qml
        )
target_link_libraries(ExampleProject PRIVATE Qt6::Quick disboardplugin)
