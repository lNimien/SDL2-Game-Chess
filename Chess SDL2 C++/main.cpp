
#include <iostream>
#include <SDL.h>
#include "Game.h"
#include "constants.h"
#include "SDL_image.h"

Game* game = nullptr;

int main(int argc, char* argv[])
{
    const int frameDelay = 1000 / 60;

    Uint32 Start;
    int Time;

    game = new Game();
    game->m_pTablero = new Tablero();
    game->Init("Prueba", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WinValueX, WinValueY, false);
    game->m_pTablero->Init(game->GetRender());
    while (game->IsRuning())
    {
        Start = SDL_GetTicks();

        game->Input();
        game->Update();
        game->Render();

        Time = SDL_GetTicks() - Start;

        if (frameDelay > Time) // tick : better performance
        {
            SDL_Delay(frameDelay - Time);
        }
    }

    game->Clean();
    return 0;
}
