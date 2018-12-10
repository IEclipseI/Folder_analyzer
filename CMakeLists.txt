cmake_minimum_required(VERSION 2.8)
project(folder_analyzer)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -O3")

#-fsanitize=address
#--coverage
set(CMAKE_PREFIX_PATH "/home/roman/Qt/5.11.2/gcc_64/lib")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(OpenSSL REQUIRED)

include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Core_INCLUDE_DIRS})

add_executable(folder_analyzer main.cpp
        mainwindow.cpp mainwindow.h mainwindow.ui
        indexingdialog/indexingdialog.cpp indexingdialog/indexingdialog.h indexingdialog/indexingdialog.ui
        files_util/FilesUtil.cpp files_util/FilesUtil.h)

target_link_libraries(folder_analyzer Qt5::Widgets)
target_link_libraries(folder_analyzer Qt5::Core)
target_link_libraries(folder_analyzer OpenSSL::SSL)
