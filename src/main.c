#include "../include/gbmem.h"
#include "../include/process.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


Uint8 *memory;


void init_emulator()
{
    // Load ROM into memory
    memory = malloc((GB_MEMORY_SIZE + 1) * sizeof(*memory));

    FILE *fptr = fopen("../roms/rom0.gb", "rb");
    if (fptr == NULL) {
        printf("Error! opening file");
        exit(1);
    }
    if (fread(memory, 1, GB_BANK_SIZE * 2, fptr) != GB_BANK_SIZE * 2)
    {
        printf("Error! reading file");
        exit(1);
    }
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

    while (quit == false) {
        process_and_render_frame(emulator_surface);
        SDL_UpdateWindowSurface(emulator_window);

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                quit = true;
        }
    }

    SDL_DestroyWindow(emulator_window);
    SDL_Quit();
    free(memory);
    return EXIT_SUCCESS;
}
