#include "SDL3/SDL_events.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_scancode.h"
#include <SDL3/SDL.h>

int main() {
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *win_sdl =
      SDL_CreateWindow("Mini Terminal", 1200, 600, SDL_WINDOWPOS_CENTERED);

  int running = 1;
  SDL_Event event;
  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        running = 0;
      } else if (event.type == SDL_EVENT_KEY_DOWN) {
        if ((event.key.mod & SDL_KMOD_LCTRL) &&
            (event.key.scancode == SDL_SCANCODE_C)) {
          running = 0;
        }
      }
    }

    SDL_Delay(10);
  }

  SDL_DestroyWindow(win_sdl);
  SDL_Quit();
  return 0;
}
