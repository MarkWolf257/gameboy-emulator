cmake_minimum_required(VERSION 3.28)
project(gameboy_emulator C)

set(CMAKE_C_STANDARD 11)

find_package(SDL2 CONFIG REQUIRED)

add_executable(emulator src/main.c
        src/process.c
        include/process.h
        src/graphics.c
        include/graphics.h
)

target_link_libraries(emulator mingw32 SDL2::SDL2main SDL2::SDL2)