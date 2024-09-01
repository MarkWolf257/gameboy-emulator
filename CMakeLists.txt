cmake_minimum_required(VERSION 3.28)
project(gameboy_emulator C)

set(CMAKE_C_STANDARD 11)

#add_executable(emulator src/emulator.c
#        include/gbcpu.h
#        include/iblk0.h
#        include/gbmem.h
#        include/iblk1.h
#        include/iblk2.h
#        include/iblk3.h
#        include/iblkcb.h
#        include/gbppu.h
#)

add_executable(emulator src/main.c
#        src/gbcpu.c
#        src/gbppu.c
        src/process.c
        include/process.h
        src/graphics.c
        include/graphics.h
)

target_link_libraries(emulator mingw32 SDL2main SDL2)