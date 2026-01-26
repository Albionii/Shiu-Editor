#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include "gap_buffer.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define WINDOW_TITLE "Shiu Editor"
#define WRAP_TEXT_WIDTH 700

struct Editor {
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
    SDL_Texture *text_texture;
    SDL_Rect text_rect;
    int char_w;
    int char_h;
    GapBuffer *gb;
};

bool sdl_initialize(struct Editor *editor);
void editor_cleanup(struct Editor *editor, int exit_status);
void update_text_texture(struct Editor *editor, const char *new_text);
void draw_cursor(struct Editor *editor);
void handle_key_presses(struct Editor *editor, SDL_Event *event);