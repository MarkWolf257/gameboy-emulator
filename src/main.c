#include "../include/memory.h"
#include "../include/graphics.h"
#include "../include/process.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


// Emulated Primary Memory
Uint8 *memory;


void init_emulator(const char *rom)
{
    // Allocate memory
    memory = malloc((GB_MEMORY_SIZE + 1) * sizeof(*memory));

    // Load ROM into memory
    FILE *fptr = fopen(rom, "rb");
    if (fptr == NULL) {
        printf("Error! opening file");
        exit(1);
    }
    if (fread(memory, 1, GB_BANK_SIZE * 2, fptr) != GB_BANK_SIZE * 2)
    {
        printf("Error! reading file");
        exit(1);
    }
    memory[SC] = 0x7E;
    memory[LCDC] = 0x91;
    memory[DMA] = 0xFF;
    fclose(fptr);
}


static inline void
scaleSurface(const SDL_Surface* src, const SDL_Surface* dst, const int scaleFactor) {
    const Uint32* srcPixels = (Uint32*)src->pixels;
    Uint32* dstPixels = (Uint32*)dst->pixels;
    const int srcPitch = src->pitch / sizeof(Uint32);
    const int dstPitch = dst->pitch / sizeof(Uint32);

    for (int y = 0; y < src->h; y++) {
        for (int x = 0; x < src->w; x++) {
            const Uint32 color = srcPixels[y * srcPitch + x];

            for (int dy = 0; dy < scaleFactor; dy++) {
                for (int dx = 0; dx < scaleFactor; dx++) {
                    dstPixels[(y * scaleFactor + dy) * dstPitch + (x * scaleFactor + dx)] = color;
                }
            }
        }
    }
}



int main(int argc, char *args[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: emulator [filename]\n");
        return 1;
    }

    init_emulator(args[1]);
    init_process();

    SDL_Window *emulator_window = NULL;
    const SDL_Surface *emulator_surface = NULL;
    const SDL_Surface *drawing_surface = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    emulator_window = SDL_CreateWindow(
        "gbemu",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        GB_SCREEN_WIDTH * SCALING_FACTOR,
        GB_SCREEN_HEIGHT * SCALING_FACTOR,
        SDL_WINDOW_SHOWN
    );
    if (emulator_window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    emulator_surface = SDL_GetWindowSurface(emulator_window);

    drawing_surface = SDL_CreateRGBSurfaceWithFormat(
        0, GB_SCREEN_WIDTH, GB_SCREEN_HEIGHT,
        emulator_surface->format->BitsPerPixel,
        emulator_surface->format->format
    );


    SDL_Event e;
    bool quit = false;
    Uint64 start = SDL_GetPerformanceCounter(), end;


    while (quit == false) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                quit = true;
        }

        double secondsElapsed;

        process_and_render_frame(drawing_surface);
        scaleSurface(drawing_surface, emulator_surface, SCALING_FACTOR);

        do {
            end = SDL_GetPerformanceCounter();
            secondsElapsed = (end - start) / (double)SDL_GetPerformanceFrequency();
        } while (secondsElapsed < (1.0 / 60.0));

        start = end;

        SDL_UpdateWindowSurface(emulator_window);
    }

    SDL_DestroyWindow(emulator_window);
    SDL_Quit();
    free(memory);
    return EXIT_SUCCESS;
}