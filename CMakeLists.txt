cmake_minimum_required(VERSION 3.28)
project(gameboy_emulator C)

set(CMAKE_C_STANDARD 11)

add_executable(emulator src/emulator.c
        src/gbcpu.h
        src/iblk0.h
        src/gbmem.h
        src/iblk1.h
        src/iblk2.h
        src/iblk3.h
        src/iblkcb.h
        src/gbppu.h
)
