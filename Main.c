#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <stdbool.h>
#include <wchar.h>

#define WINDOW_TITLE "Shiu Editor"
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

struct Editor {
    SDL_Window *window;
    SDL_Renderer *renderer;
};

bool sdl_initialize(struct Editor *editor);
void editor_cleanup(struct Editor *editor);

int main() {
    struct Editor editor = {
        .window = NULL,
        .renderer = NULL
    };
    
    if(sdl_initialize(&editor)){
        editor_cleanup(&editor);
        exit(0);
    };


    
    editor_cleanup(&editor);
    return 0;
}

void editor_cleanup(struct Editor *editor){
    SDL_DestroyRenderer(editor -> renderer);
    SDL_DestroyWindow(editor -> window);
    SDL_Quit();
}

bool sdl_initialize(struct Editor *editor){
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }
    
    editor->window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_HIDDEN
    );
    
    if (!editor -> window){
        fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
        return 1;
    }
    
    editor -> renderer = SDL_CreateRenderer(editor->window, -1, 0);
    
    if (!editor -> renderer){
        fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
        return 1;
    }
    
    SDL_ShowWindow(editor->window);
    
    return 0;
}
