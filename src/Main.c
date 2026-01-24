#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_touch.h>
#include <SDL2/SDL_video.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <wchar.h>
#include <SDL2/SDL_ttf.h>

#include "../include/GapBuffer.h"


#define WINDOW_TITLE "Shiu Editor"
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define WRAP_TEXT_WIDTH 700
#define MAX_BUFFER 2048
#define MAX_FILE_PATH_SIZE 1024

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

//Function definitions
bool sdl_initialize(struct Editor *editor);
void editor_cleanup(struct Editor *editor, int exit_status);
void window_customize(struct Editor *editor);
void handle_key_presses(struct Editor *editor, SDL_Event *event);
void update_text_texture(struct Editor *editor, const char *new_text);
void draw_cursor(struct Editor *editor);
void save_to_file(struct Editor *editor, char *filename);
char* get_filename_from_zenity();

int main() {
    struct Editor editor = {0};

    if(sdl_initialize(&editor)){
        editor_cleanup(&editor, EXIT_FAILURE);
    };
    
    editor.gb = gb_create(1024);
    
    //Here is the main editor logic

    // window_customize(&editor);
    
    SDL_Event event;

    editor.text_rect.x = 50;
    editor.text_rect.y = 50;

    bool needs_update = true;
    while (true) {
        while (SDL_PollEvent(&event)) {
            switch(event.type){
                case SDL_QUIT:
                    editor_cleanup(&editor, EXIT_SUCCESS);
                    break;
                case SDL_TEXTINPUT:
                    update_text_texture(&editor, event.text.text);
                    break;
                case SDL_KEYDOWN:
                    handle_key_presses(&editor, &event);
                    break;
                case SDL_MOUSE_TOUCHID:
                    break;
                default:
                    break;
            }
        }


        SDL_SetRenderDrawColor(editor.renderer, 30, 30, 30, 255);
        SDL_RenderClear(editor.renderer);
        SDL_RenderCopy(editor.renderer, editor.text_texture, NULL, &editor.text_rect);
        draw_cursor(&editor);
        SDL_RenderPresent(editor.renderer);

        //Delay the loop to be around 60 frames/s
        SDL_Delay(16);
    }



    editor_cleanup(&editor, EXIT_SUCCESS);
    return 0;
}


void editor_cleanup(struct Editor *editor, int exit_status){
    SDL_DestroyRenderer(editor -> renderer);
    SDL_DestroyWindow(editor -> window);
    SDL_Quit();
    exit(exit_status);
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

    TTF_Init();

    //Font Monospace Local to my Linux machine.
    editor->font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf", 24);
    if (!editor->font) {
        fprintf(stderr, "Font load error: %s\n", TTF_GetError());
        return 1;
    }

    TTF_SizeUTF8(editor->font, "B", &editor->char_w, &editor->char_h);
    SDL_StartTextInput();
    return 0;
}

void window_customize(struct Editor *editor){
    SDL_Color color = {};
}

void handle_key_presses(struct Editor *editor, SDL_Event *event){
    bool text_changed = false;
    switch(event->key.keysym.sym){  
        case SDLK_UP: {
            if (editor->gb->gap_left == 0) break;
            size_t gl = editor->gb->gap_left;
            while (gl > 0 && editor->gb->buffer[gl-1] != '\n') gl--;
            if(gl == 0) break;

            gl--;
            gb_move_cursor(editor->gb, gl);
            break;
        }
        case SDLK_DOWN:;
            size_t capacity = editor->gb->capacity;
            size_t gr = editor->gb->gap_right+1;
            
            while (gr < capacity && editor->gb->buffer[gr] != '\n') gr++;
            
            if (gr >=capacity-1) break;
            
            size_t new_pos = editor->gb->gap_left + (gr - editor->gb->gap_right);
            gb_move_cursor(editor->gb, new_pos);
            break;
        case SDLK_LEFT:
            if (editor->gb->gap_left > 0) {
                gb_move_cursor(editor->gb, editor->gb->gap_left - 1);
            }
            break;
        case SDLK_RIGHT: {
            size_t total = editor->gb->gap_left + (editor->gb->capacity - editor->gb->gap_right - 1);
            if (editor->gb->gap_left < total){
                gb_move_cursor(editor->gb, editor->gb->gap_left + 1);
            }
            break;
        }
        case SDLK_KP_ENTER:
        case SDLK_RETURN:
            gb_insert(editor->gb, "\n");
            text_changed = true;
            break;
        case SDLK_BACKSPACE:
            gb_backspace(editor->gb);
            text_changed = true;
            break;
        //Save the file Ctrl + s
        case SDLK_s:
            if(event->key.keysym.mod & SDLK_LCTRL){
                char *path = get_filename_from_zenity();
                if (path != NULL) {
                    save_to_file(editor, path);
                    printf("Saved to: %s\n", path);
                    free(path);
                }
            }
            break;
        default:
            break;
    }
    if (text_changed) update_text_texture(editor, NULL);
}
void update_text_texture(struct Editor *editor, const char *new_text) {
    if (new_text){
        gb_insert(editor->gb, new_text);
    }

    char *text_to_render = gb_get_string(editor -> gb);

    if (editor->text_texture) {
        SDL_DestroyTexture(editor->text_texture);
    }


    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(
        editor->font,
        text_to_render,
        white,
        WRAP_TEXT_WIDTH);

    if (surface) {
        editor->text_texture = SDL_CreateTextureFromSurface(editor->renderer, surface);
        editor->text_rect.w = surface->w;
        editor->text_rect.h = surface->h;
        SDL_FreeSurface(surface);
    }
    free(text_to_render);
}


/**
 * Draws the cursor where the next character will be written.
 */
void draw_cursor(struct Editor *editor) {
    if ((SDL_GetTicks() / 500) % 2 != 0) return;

    int row = 0;
    int col = 0;

    for (size_t i = 0; i < editor->gb->gap_left; i++){
        if (editor->gb->buffer[i] == '\n'){
            row++;
            col = 0;
        }else {
            col++;
        }
    }

    SDL_Rect cursor_rect = {
        editor->text_rect.x + (col * editor->char_w),
        editor->text_rect.y + (row * editor->char_h),
        2,
        TTF_FontHeight(editor->font)
    };
    SDL_SetRenderDrawColor(editor->renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(editor->renderer, &cursor_rect);

}


char* get_filename_from_zenity(){
    FILE *file = popen("zenity --file-selection --save --confirm-overwrite --title='What shall be thy name?'","r");
    if (!file) {
        printf("ERROR: Could not open zenity %p : ", file);
        return NULL;
    }
    
    char *path = malloc(MAX_FILE_PATH_SIZE);
    if (fgets(path, MAX_FILE_PATH_SIZE, file) != NULL) {
        path[strcspn(path, "\n")] = 0;
        pclose(file);
        return path;
    }
    pclose(file);
    free(path);
    return NULL;    
}


/**
 * Saved the text inside editor into a file.
 */
void save_to_file(struct Editor *editor, char *filename){
    // filename_popup(editor);
    FILE *file = fopen(filename, "w");
    if (!file){
        printf("ERROR: Could not open file %s for writing : ", filename);
        return;
    }

    if (editor->gb->gap_left > 0){
        fwrite(editor->gb->buffer, 1, editor->gb->gap_left, file);
    }

    size_t right_side = editor->gb->gap_right + 1;
    size_t right_side_length = editor->gb->capacity - right_side;

    if (right_side_length > 0) {
        fwrite(editor->gb->buffer + right_side, 1, right_side_length, file);
    }
    fclose(file);
}
