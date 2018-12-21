cmake_minimum_required(VERSION 2.8)
project(folder_analyzer)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined")

#-fsanitize=address
#--coverage

set(CMAKE_PREFIX_PATH "/home/roman/Qt/5.12.0/gcc_64/lib")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)



find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
#find_package(OpenSSL REQUIRED)

include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Core_INCLUDE_DIRS})
#include_directories(${Boost_INCLUDE_DIR})



add_executable(folder_analyzer
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        indexingdialog/indexingdialog.cpp
        indexingdialog/indexingdialog.h
        indexingdialog/indexingdialog.ui
        files_util/FilesUtil.cpp
        files_util/FilesUtil.h
        files_util/TrigramsExtracter/TrigramsExtracter.cpp
        files_util/TrigramsExtracter/TrigramsExtracter.h
        files_util/StringSearcher/StringSearcher.cpp
        files_util/StringSearcher/StringSearcher.h
        files_util/TrigramsContainer/TrigramsContainer.h
        files_util/TrigramsContainer/TrigramsContainer.cpp
        )

target_link_libraries(folder_analyzer Qt5::Widgets)
target_link_libraries(folder_analyzer Qt5::Core)
#target_link_libraries(folder_analyzer OpenSSL::SSL)
#target_link_libraries(folder_analyzer ${Boost_LIBRARIES})
