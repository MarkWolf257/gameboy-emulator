# My Gameboy Emulator
This is the first ever emulator I have developed on my own as a learning project.

<br />

## About the emulator?
This emulates the DMG gameboy model without any audio or mbc support which I might work on in the future.
This means the emulator can only run very few games, which are all the games that have a 32kb ROM.
Personally I have tested some test roms and 5 game roms:
- Alleyway
- Hyper Lode Runner
- LoopZ
- Tennis
- Tetris

Documentation used: https://gbdev.io/pandocs/

<br />

## Inaccuracies
This emulator is not very accurate as it does not emulate all the bugs and quirks of the DMG gameboy.
Some of the major ones worth mentioning will be the halt bug and pixel FIFO.
There is some glitch with object priority as well, only noticeable in the Tennis ROM.

<br />

## Controls
- WASD -> Movement
- J -> A
- K -> B
- L -> Select
- ; -> Start

<br />

## Roadmap
Although I will put the development on halt for now to work on other projects, if I ever want to get
back into this, I want to follow this roadmap ordered by priority:
- Audio Emulation
- A menu bar for customization
- MBC Implementation
- CGB Implementation

<br />

## How to build
To build the emulator from source code you will need the sdl header files in your compiler headers.
Then you can use either the cmake or simply compile and link all the files in the src folder with your compiler.

For example on windows I used this:

```commandline
mkdir build
cd build
cmake ..
ninja
```

or this:

```commandline
cd src
gcc main.c process.c graphics.c -lmingw32 -lSDL2Main -lSDL2 -o ../emulator
```

<br />

## How to run
To run the emulator with a ROM file:

```commandline
emulator tetris.gb
```
