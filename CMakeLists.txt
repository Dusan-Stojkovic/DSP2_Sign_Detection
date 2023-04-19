cmake_minimum_required(VERSION 2.8) 
PROJECT (sign_detection)
find_package(OpenCV REQUIRED)
find_package (Threads )

set( NAME_SRC
    src/main.cpp
    src/imageprocessing.cpp
    src/messagequeue.cpp
)

set( NAME_HEADERS       
     include/imageprocessing.hpp
     include/messagequeue.hpp
)

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/include )
link_directories( ${CMAKE_BINARY_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
add_executable( sign_detection ${NAME_SRC} ${NAME_HEADERS} )

target_link_libraries( sign_detection ${CMAKE_THREAD_LIBS_INIT} )
target_link_libraries( sign_detection ${OpenCV_LIBS} )

