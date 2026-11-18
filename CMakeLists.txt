cmake_minimum_required(VERSION 3.23)
project(MazeRunner)

# Set C++ standard to version 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
set(RAYLIB_VERSION 6.2)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    SYSTEM
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(raylib)
  endif()
endif()


# Main project
add_executable(${PROJECT_NAME})

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE $<NOT:$<CONFIG:Debug>>
    )
    if(MSVC)
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:/ENTRY:mainCRTStartup>
        )
    endif()
endif()

target_sources(${PROJECT_NAME}
    PRIVATE
        src/audio.cpp
        src/boss.cpp
        src/effects.cpp
        src/enemy.cpp
        src/game.cpp
        src/main.cpp
        src/maze.cpp
        src/pixel_art.cpp
        src/player.cpp
        src/transition.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS src
        FILES
            src/audio.h
            src/boss.h
            src/effects.h
            src/enemy.h
            src/game.h
            src/maze.h
            src/pixel_art.h
            src/player.h
            src/transition.h
)

#set(raylib_VERBOSE 1)
target_link_libraries(${PROJECT_NAME} 
    PRIVATE
        raylib
)

# Set build type as a compile definition
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:DEBUG>)

# Web Configurations
if (${PLATFORM} STREQUAL "Web")
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html") # Tell Emscripten to build an example.html file.
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY --preload-file ${CMAKE_SOURCE_DIR}/assets@/assets -sABORTING_MALLOC=0 --shell-file ${CMAKE_SOURCE_DIR}/index.html")
    set_target_properties(${PROJECT_NAME} PROPERTIES ADDITIONAL_CLEAN_FILES "${PROJECT_NAME}.data;${PROJECT_NAME}.wasm;${PROJECT_NAME}.js")
endif()

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
    target_link_libraries(${PROJECT_NAME} "-framework QuartzCore")
endif()

# Copy the assets folder for the game
add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_CURRENT_BINARY_DIR}/assets
)