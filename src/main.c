#include "../include/gbmem.h"
#include "../include/process.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


// Emulated Primary Memory
Uint8 *memory;


void init_emulator()
{
    // Allocate memory
    memory = malloc((GB_MEMORY_SIZE + 1) * sizeof(*memory));

    // Load ROM into memory
    FILE *fptr = fopen("../roms/loopz.gb", "rb");
    if (fptr == NULL) {
        printf("Error! opening file");
        exit(1);
    }
    if (fread(memory, 1, GB_BANK_SIZE * 2, fptr) != GB_BANK_SIZE * 2)
    {
        printf("Error! reading file");
        exit(1);
    }
    memory[DMA] = 0xFF;
    fclose(fptr);
}


int main(int argc, char *args[])
{
    init_emulator();
    init_process();

    SDL_Window *emulator_window = NULL;
    SDL_Surface *emulator_surface = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    emulator_window = SDL_CreateWindow(
        "gbemu",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        160, 144,
        SDL_WINDOW_SHOWN
    );
    if (emulator_window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    emulator_surface = SDL_GetWindowSurface(emulator_window);

    SDL_Event e;
    bool quit = false;
    Uint64 start = SDL_GetPerformanceCounter();


    while (quit == false) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                quit = true;
        }

        double secondsElapsed;

        process_and_render_frame(emulator_surface);

        do {
            Uint64 end = SDL_GetPerformanceCounter();
            secondsElapsed = (end - start) / (double)SDL_GetPerformanceFrequency();
        } while (secondsElapsed < (1.0 / 60.0));

        start = SDL_GetPerformanceCounter();

        SDL_UpdateWindowSurface(emulator_window);
    }

    SDL_DestroyWindow(emulator_window);
    SDL_Quit();
    free(memory);
    return EXIT_SUCCESS;
}
