#include <SDL2/SDL.h>
#include <iostream>
#undef main

uint16_t worldSizeX = 1;
uint16_t worldSizeY = 1;
uint32_t iterations = 1;

uint8_t mutationRate = 10; // lowest 2 (50%), 10 is (10%)

uint16_t getRand(uint16_t max) // Not inclusive
{
  return (rand() % max);
}

uint64_t randUint64Slow()
{
    uint64_t r = 0;
    for (int i = 0; i < 64; i++)
    {
        r = r * 2 + rand() % 2;
    }

    return r;
}

uint64_t** fillWorld()
{
    uint64_t** world = (uint64_t**)malloc(worldSizeX * sizeof(int*));
    for (int i = 0; i < worldSizeX; i++)
    {
        world[i] = (uint64_t*)malloc(worldSizeY * sizeof(uint64_t));
    }

    for (int x = 0; x < worldSizeX; x++)
    {
        for (int y = 0; y < worldSizeY; y++)
        {
            world[x][y] = randUint64Slow();
        }
    }

    return world;
}

int neighborId(int genomeId, int worldSize)
{
    if (genomeId == 0)
    {
        return genomeId + 1;
    }

    if (genomeId == worldSize - 1)
    {
        return genomeId - 1;
    }

    int neighborId;

    if (getRand(2) == 0)
    {
        neighborId = (genomeId + 1);
        return neighborId;
    }

    neighborId = (genomeId - 1);
    return neighborId;
}

uint64_t mutate(uint64_t genome)
{
    if (getRand(mutationRate) == 0)
    {
        genome ^= static_cast<unsigned long long>(0b1) << getRand(64);
        return genome;
    }

    return genome;
}

uint64_t** executeGenomeInstruction(uint64_t** world, int genomeIdX, int genomeIdY, uint8_t instruction)
{
    if (instruction == 0b00) // A
    {
        // do nothing
    }
    else if (instruction == 0b01) // C
    {
        // try to make a copy
        int selectedNeighborIdX;
        int selectedNeighborIdY;
        if (getRand(2) == 0)
        {
            selectedNeighborIdX = neighborId(genomeIdX, worldSizeX);
            selectedNeighborIdY = genomeIdY;
        }
        else
        {
            selectedNeighborIdX = genomeIdX;
            selectedNeighborIdY = neighborId(genomeIdY, worldSizeY);
        }

        if (world[selectedNeighborIdX][selectedNeighborIdY] == 0b0)
        {
            world[selectedNeighborIdX][selectedNeighborIdY] = mutate(world[genomeIdX][genomeIdY]);
        }
    }
    else if (instruction == 0b10) // G
    {
        // kys
        world[neighborId(genomeIdX, worldSizeX)][neighborId(genomeIdY, worldSizeY)] = 0b00;
    }
    else if (instruction == 0b11) // T
    {
        // try to move
        int selectedNeighborIdX;
        int selectedNeighborIdY;
        if (getRand(2) == 0)
        {
            selectedNeighborIdX = neighborId(genomeIdX, worldSizeX);
            selectedNeighborIdY = genomeIdY;
        }
        else
        {
            selectedNeighborIdX = genomeIdX;
            selectedNeighborIdY = neighborId(genomeIdY, worldSizeY);
        }

        if (world[selectedNeighborIdX][selectedNeighborIdY] == 0b0)
        {
            world[selectedNeighborIdX][selectedNeighborIdY] = world[genomeIdX][genomeIdY];
            world[genomeIdX][genomeIdY] = 0b00;
        }
    }

    return world;
}

uint64_t** runAllGenomeCode(uint64_t** world, int genomeIdX, int genomeIdY, uint64_t genome)
{
    if (world[genomeIdX][genomeIdY] == 0b0)
    {
        return world;
    }

    for (uint8_t instructionId = 0; instructionId < 64; instructionId += 2)
    {
        world = executeGenomeInstruction(world, genomeIdX, genomeIdY, (genome >> instructionId) & 0b11);

        if (world[genomeIdX][genomeIdY] == 0b0)
        {
            return world;
        }
    }

    return world;
}

uint64_t** runWorld(uint64_t** world)
{
    for (int x = 0; x < worldSizeX; x++)
    {
        for (int y = 0; y < worldSizeY; y++)
        {
            world = runAllGenomeCode(world, x, y, world[x][y]);
        }
    }

    world[getRand(worldSizeX)][getRand(worldSizeY)] = 0b0;

    return world;
}

void drawToScreen(uint64_t** world, SDL_Renderer* renderer)
{
    Uint16 filter = 0b1111111111111111;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (int x = 0; x < worldSizeX; x++)
    {
        for (int y = 0; y < worldSizeY; y++)
        {
            Uint8 r = (Uint8)(((world[x][y] >> 48) & filter) / 2);
            Uint8 g = (Uint8)(((world[x][y] >> 32) & filter) / 2);
            Uint8 b = (Uint8)(((world[x][y] >> 16) & filter) / 2);
            Uint8 a = (Uint8)((world[x][y] & filter) / 2);

            SDL_SetRenderDrawColor(renderer, r, g, b, a);
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }

    SDL_RenderPresent(renderer);
}

int main()
{
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    std::cout << "worldSizeX: ";
    std::cin >> worldSizeX;

    std::cout << "worldSizeY: ";
    std::cin >> worldSizeY;

    std::cout << "iterations: ";
    std::cin >> iterations;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(worldSizeX, worldSizeY, 0, &window, &renderer);
    //SDL_RenderSetScale(renderer, 10, 10); // This option increases the pixel size by 10
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);

    srand(time(0));

    uint64_t** world;
    world = fillWorld();

    for (int i = 0; i < iterations; i++)
    {
        world = runWorld(world);
        if (i % 10 == 0)
        {
            std::cout << "iteration " << i << '\n';
        }

        drawToScreen(world, renderer);
    }

    SDL_Delay(3000);

    return 0;
}