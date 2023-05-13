cmake_minimum_required(VERSION 3.21)
project(pixel_game_demo VERSION 0.1.0 LANGUAGES CXX)


file(COPY ${CMAKE_SOURCE_DIR}/resource
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB_RECURSE all_source_file ${CMAKE_SOURCE_DIR}/src/*.c  ${CMAKE_SOURCE_DIR}/src/*.cpp )
file(GLOB_RECURSE all_hpp_file ${CMAKE_SOURCE_DIR}/src/*.hpp)
set(include_dir ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME} main.cpp ${all_source_file} )

if(WIN32)
    set(flags -luser32 -lgdi32 -lopengl32 -lgdiplus -lShlwapi -ldwmapi -lstdc++fs)
endif(WIN32)

target_link_libraries(${PROJECT_NAME} PRIVATE ${flags} -static )
target_include_directories(${PROJECT_NAME} PRIVATE ${include_dir})

