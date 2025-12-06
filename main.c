#include <SDL3/SDL.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
    return 1;
  }

  // Creating a window
  SDL_Window *win_sdl =
      SDL_CreateWindow("Mini Terminal", 1200, 600, SDL_WINDOW_RESIZABLE);

  // if it fails to create the window logging the error and quitting sdl
  if (!win_sdl) {
    SDL_Log("Failed to create window: %s", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  // Create renderer for drawing
  SDL_Renderer *renderer = SDL_CreateRenderer(win_sdl, NULL);
  if (!renderer) {
    SDL_Log("Failed to create renderer: %s", SDL_GetError());
    SDL_DestroyWindow(win_sdl);
    SDL_Quit();
    return 1;
  }

  // giving some colors for the term
  SDL_Color bg_color = {0, 0, 0, 255};
  SDL_Color fg_color = {0, 255, 0, 255};

  int running = 1;
  SDL_Event event;

  while (running) {
    // Handle events
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        running = 0;
      } else if (event.type == SDL_EVENT_KEY_DOWN) {

        if ((event.key.mod & SDL_KMOD_CTRL) &&
            (event.key.scancode == SDL_SCANCODE_C)) {
          running = 0;
        } else if ((event.key.mod & SDL_KMOD_CTRL) &&
                   (event.key.scancode == SDL_SCANCODE_Q)) {
          running = 0;
        }

        // Log key presses (for debugging)
        else {
          const char *key_name = SDL_GetKeyName(event.key.key);
          SDL_Log("Key pressed: %s", key_name);
        }
      } else if (event.type == SDL_EVENT_TEXT_INPUT) {

        // Handle text input for terminal
        SDL_Log("Text input: %s", event.text.text);
      } else if (event.type == SDL_EVENT_WINDOW_RESIZED) {
        SDL_Log("Window resized to: %dx%d", event.window.data1,
                event.window.data2);
      }
    }

    // Clear screen with background color that are already defined in the top
    SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b,
                           bg_color.a);
    SDL_RenderClear(renderer);

    // TODO: Render terminal text here
    // You'll want to add SDL_ttf for text rendering

    // Draw a simple cursor indicator (example)
    SDL_SetRenderDrawColor(renderer, fg_color.r, fg_color.g, fg_color.b,
                           fg_color.a);
    SDL_FRect cursor = {10, 10, 10, 20}; // x, y, width, height
    SDL_RenderFillRect(renderer, &cursor);

    // Present the rendered frame
    SDL_RenderPresent(renderer);

    // Small delay to reduce CPU usage
    SDL_Delay(16); // ~60 FPS
  }

  // Cleanup
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win_sdl);
  SDL_Quit();
  return 0;
}
