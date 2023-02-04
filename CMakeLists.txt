cmake_minimum_required(VERSION 3.11)

project(todocli)

include(FetchContent)

FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp
    GIT_TAG 1.9.5
)
FetchContent_MakeAvailable(json)

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE jsoncpp_lib)

