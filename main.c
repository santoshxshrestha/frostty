#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINES 50
#define MAX_LINE_LENGTH 128
#define MAX_INPUT_LENGTH 128

typedef struct {
  char lines[MAX_LINES][MAX_LINE_LENGTH];
  int line_count;
  char input_buffer[MAX_INPUT_LENGTH];
  int input_length;
} Terminal;

void terminal_init(Terminal *term) {
  memset(term, 0, sizeof(Terminal));
  term->line_count = 0;
  term->input_length = 0;

  snprintf(term->lines[term->line_count++], MAX_LINE_LENGTH,
           "Frostty Terminal v0.1");
  snprintf(term->lines[term->line_count++], MAX_LINE_LENGTH,
           "Type 'help' for commands (text in console)");
  snprintf(term->lines[term->line_count++], MAX_LINE_LENGTH, "");
}

void terminal_add_line(Terminal *term, const char *text) {
  if (term->line_count >= MAX_LINES) {
    memmove(term->lines[0], term->lines[1],
            sizeof(char[MAX_LINE_LENGTH]) * (MAX_LINES - 1));
    term->line_count = MAX_LINES - 1;
  }
  snprintf(term->lines[term->line_count++], MAX_LINE_LENGTH, "%s", text);
}

void terminal_execute_command(Terminal *term, const char *cmd) {
  if (strlen(cmd) == 0)
    return;

  char prompt[MAX_LINE_LENGTH];
  snprintf(prompt, MAX_LINE_LENGTH, "$ %s", cmd);
  terminal_add_line(term, prompt);
  printf("%s\n", prompt);

  if (strcmp(cmd, "help") == 0) {
    terminal_add_line(term, "Commands:");
    terminal_add_line(term, "  help, clear, pwd");
    terminal_add_line(term, "  date, echo <text>");
    terminal_add_line(term, "  exit (or Ctrl+Q)");
    printf("Available commands:\n");
    printf("  help    - Show this help\n");
    printf("  clear   - Clear terminal\n");
    printf("  pwd     - Print working directory\n");
    printf("  date    - Show current date\n");
    printf("  echo    - Echo text back\n");
    printf("  exit    - Exit terminal\n");
  } else if (strcmp(cmd, "clear") == 0) {
    term->line_count = 0;
    printf("Terminal cleared\n");
  } else if (strcmp(cmd, "pwd") == 0) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
      terminal_add_line(term, cwd);
      printf("%s\n", cwd);
    } else {
      terminal_add_line(term, "Error: getcwd");
      printf("Error getting current directory\n");
    }
  } else if (strcmp(cmd, "date") == 0) {
    FILE *fp = popen("date", "r");
    if (fp) {
      char output[MAX_LINE_LENGTH];
      if (fgets(output, sizeof(output), fp) != NULL) {
        output[strcspn(output, "\n")] = 0;
        terminal_add_line(term, output);
        printf("%s\n", output);
      }
      pclose(fp);
    }
  } else if (strncmp(cmd, "echo ", 5) == 0) {
    terminal_add_line(term, cmd + 5);
    printf("%s\n", cmd + 5);
  } else if (strcmp(cmd, "exit") == 0) {
    terminal_add_line(term, "Exiting...");
    printf("Exiting...\n");
  } else {
    char error[MAX_LINE_LENGTH];
    snprintf(error, MAX_LINE_LENGTH, "Unknown: %s", cmd);
    terminal_add_line(term, error);
    printf("Unknown command: %s\n", cmd);
  }

  terminal_add_line(term, "");
}

void draw_text_bitmap(SDL_Renderer *renderer, const char *text, int x, int y,
                      SDL_Color color, int char_width, int char_height) {
  // Simple 8x8 ASCII representation using rectangles
  // This is a placeholder - for now just draw boxes for each character
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

  for (int i = 0; text[i] != '\0'; i++) {
    int cx = x + i * char_width;
    // Draw a simple rectangle to represent each character
    SDL_FRect rect = {cx, y, char_width - 2, char_height - 2};
    SDL_RenderRect(renderer, &rect);

    // Draw a dot in the middle to show there's a character
    SDL_FRect dot = {cx + 2, y + 3, 2, 2};
    SDL_RenderFillRect(renderer, &dot);
  }
}

int main(int argc, char *argv[]) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
    return 1;
  }

  SDL_Window *win_sdl =
      SDL_CreateWindow("Frostty Terminal", 1200, 600, SDL_WINDOW_RESIZABLE);

  if (!win_sdl) {
    SDL_Log("Failed to create window: %s", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(win_sdl, NULL);
  if (!renderer) {
    SDL_Log("Failed to create renderer: %s", SDL_GetError());
    SDL_DestroyWindow(win_sdl);
    SDL_Quit();
    return 1;
  }

  SDL_Color bg_color = {0, 0, 0, 255};
  SDL_Color fg_color = {0, 255, 0, 255};

  Terminal terminal;
  terminal_init(&terminal);

  SDL_StartTextInput(win_sdl);

  int running = 1;
  SDL_Event event;
  Uint32 cursor_blink_time = 0;
  int cursor_visible = 1;

  int char_width = 10;
  int char_height = 16;
  printf("Frostty Terminal Started\n");
  printf("Window is open - text I/O is in this console\n");
  printf("Type 'help' for commands\n\n");

  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        running = 0;
      } else if (event.type == SDL_EVENT_KEY_DOWN) {
        if ((event.key.mod & SDL_KMOD_CTRL) &&
            (event.key.scancode == SDL_SCANCODE_C)) {
          terminal.input_buffer[0] = '\0';
          terminal.input_length = 0;
          terminal_add_line(&terminal, "^C");
          printf("^C\n");
        } else if ((event.key.mod & SDL_KMOD_CTRL) &&
                   (event.key.scancode == SDL_SCANCODE_Q)) {
          running = 0;
        } else if (event.key.scancode == SDL_SCANCODE_RETURN) {
          terminal_execute_command(&terminal, terminal.input_buffer);
          if (strcmp(terminal.input_buffer, "exit") == 0) {
            running = 0;
          }
          terminal.input_buffer[0] = '\0';
          terminal.input_length = 0;
        } else if (event.key.scancode == SDL_SCANCODE_BACKSPACE) {
          if (terminal.input_length > 0) {
            terminal.input_buffer[--terminal.input_length] = '\0';
            printf("\b \b");
            fflush(stdout);
          }
        }
      } else if (event.type == SDL_EVENT_TEXT_INPUT) {
        if (terminal.input_length < MAX_INPUT_LENGTH - 1) {
          strncat(terminal.input_buffer, event.text.text,
                  MAX_INPUT_LENGTH - terminal.input_length - 1);
          terminal.input_length = strlen(terminal.input_buffer);
          printf("%s", event.text.text);
          fflush(stdout);
        }
      }
    }

    Uint32 current_time = SDL_GetTicks();
    if (current_time - cursor_blink_time > 500) {
      cursor_visible = !cursor_visible;
      cursor_blink_time = current_time;
    }

    SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b,
                           bg_color.a);
    SDL_RenderClear(renderer);

    int line_height = char_height + 4;
    int start_y = 10;

    // Draw simple text representation
    for (int i = 0; i < terminal.line_count; i++) {
      draw_text_bitmap(renderer, terminal.lines[i], 10,
                       start_y + i * line_height, fg_color, char_width,
                       char_height);
    }

    // Draw prompt and input
    char prompt[MAX_LINE_LENGTH + 10];
    snprintf(prompt, sizeof(prompt), "$ %s", terminal.input_buffer);
    int prompt_y = start_y + terminal.line_count * line_height;
    draw_text_bitmap(renderer, prompt, 10, prompt_y, fg_color, char_width,
                     char_height);

    // Draw cursor
    if (cursor_visible) {
      int cursor_x = 10 + (2 + terminal.input_length) * char_width;
      SDL_SetRenderDrawColor(renderer, fg_color.r, fg_color.g, fg_color.b,
                             fg_color.a);
      SDL_FRect cursor = {cursor_x, prompt_y, char_width - 2, char_height};
      SDL_RenderFillRect(renderer, &cursor);
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(16);
  }

  SDL_StopTextInput(win_sdl);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win_sdl);
  SDL_Quit();

  printf("\nTerminal closed\n");
  return 0;
}
