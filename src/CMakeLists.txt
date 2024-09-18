cmake_minimum_required(VERSION 3.23)

if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

project(MyLibrary VERSION 0.0.1)

set(PUBLIC_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/../include/InstrumentedClass.h
    ${CMAKE_CURRENT_LIST_DIR}/../include/MemoryManager.h
    ${CMAKE_CURRENT_LIST_DIR}/../include/TaskQueue.h
    ${CMAKE_CURRENT_LIST_DIR}/../include/MoveWrapper.h
)
source_group("Public Headers" FILES ${PUBLIC_HEADERS})

file(GLOB_RECURSE PRIVATE_HEADERS *.h)
source_group("Private Headers" FILES ${PRIVATE_HEADERS})

set(SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/InstrumentedClass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MemoryManager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/TaskQueue.cpp
)
source_group("Sources" FILES ${SOURCES})

add_library(${PROJECT_NAME} STATIC
    ${PUBLIC_HEADERS}
    ${PRIVATE_HEADERS}
    ${SOURCES}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../include
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}
)
