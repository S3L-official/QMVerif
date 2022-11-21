cmake_minimum_required(VERSION 3.5)
project(QMVerif)

set(CMAKE_CXX_STANDARD 11)
add_executable(QMVerif
        main.cpp
        Parser.h
        Parser.cpp
        Reasoner.h
        Reasoner.cpp
        Global.cpp Global.h)

include_directories(${PROJECT_SOURCE_DIR}/z3/include)
link_directories(${PROJECT_SOURCE_DIR}/z3/bin)
if(APPLE)
    TARGET_LINK_LIBRARIES(QMVerif ${PROJECT_SOURCE_DIR}/z3/bin/libz3.dylib)
ELSE()
    TARGET_LINK_LIBRARIES(QMVerif ${PROJECT_SOURCE_DIR}/z3/bin/libz3.so)
ENDIF()
