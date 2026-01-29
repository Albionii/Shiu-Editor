#include "../include/editor.h"
#include "../include/file_io.h"
#include "../include/gap_buffer.h"
#include <SDL2/SDL_surface.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sync_cursor(struct Editor *editor);


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

void handle_key_presses(struct Editor *editor, SDL_Event *event){
    bool text_changed = false;
    bool cursor_moved = false;
    switch(event->key.keysym.sym){
        case SDLK_UP: {
            if (editor->gb->gap_left == 0) break;
            size_t gl = editor->gb->gap_left;
            while (gl > 0 && editor->gb->buffer[gl-1] != '\n') gl--;
            if(gl == 0) break;

            gl--;
            gb_move_cursor(editor->gb, gl);
            cursor_moved = true;
            break;
        }
        case SDLK_DOWN:;
            size_t capacity = editor->gb->capacity;
            size_t gr = editor->gb->gap_right+1;

            while (gr < capacity && editor->gb->buffer[gr] != '\n') gr++;

            if (gr >=capacity-1) break;

            size_t new_pos = editor->gb->gap_left + (gr - editor->gb->gap_right);
            gb_move_cursor(editor->gb, new_pos);
            cursor_moved = true;
            break;
        case SDLK_LEFT:
            if (editor->gb->gap_left > 0) {
                gb_move_cursor(editor->gb, editor->gb->gap_left - 1);
                cursor_moved = true;
            }
            break;
        case SDLK_RIGHT: {
            if (editor->gb->gap_right < editor->gb->capacity - 1){
                gb_move_cursor(editor->gb, editor->gb->gap_left + 1);
                cursor_moved = true;
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
        case SDLK_DELETE: 
            gb_delete(editor->gb);
            text_changed = true;
            break;
        //Save the file Ctrl + s
        case SDLK_s:
            if(event->key.keysym.mod & SDLK_LCTRL){
                char *path = get_filename_from_zenity();
                if (path != NULL) {
                    save_to_file(editor, path);
                    free(path);
                }
            }
            break;
        case SDLK_o:
            if (event->key.keysym.mod & KMOD_CTRL){
                FILE *fp = popen("zenity --file-selection --title='Open File'", "r");
                if (fp) {
                    char path[MAX_FILE_PATH_SIZE];
                    if (fgets(path, MAX_FILE_PATH_SIZE, fp) != NULL){
                        path[strcspn(path, "\n")] = 0;
                        open_file(editor, path);
                    }
                    pclose(fp);
                    text_changed = true;
                }
            }
            break;
        case SDLK_TAB : 
            gb_insert(editor->gb, "  ");
            text_changed = true;
            break;
        default:
            break;
    }
    if (cursor_moved) sync_cursor(editor);
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
    SDL_Surface *surface = TTF_RenderUTF8_Blended(
        editor->font,
        text_to_render,
        white
    );

    if (surface) {
        editor->text_texture = SDL_CreateTextureFromSurface(editor->renderer, surface);
        editor->text_rect.w = surface->w;
        editor->text_rect.h = surface->h;
        SDL_FreeSurface(surface);
    }
    sync_cursor(editor);
    free(text_to_render);
}

/**
 * Draws the cursor where the next character will be written.
 */
void draw_cursor(struct Editor *editor) {
    if ((SDL_GetTicks() / 500) % 2 != 0) return;

    SDL_Rect cursor_rect = {
        editor->text_rect.x + (editor->cursor_col * editor->char_w),
        editor->text_rect.y + (editor->cursor_row * editor->char_h),
        2,
        TTF_FontHeight(editor->font)
    };
    SDL_SetRenderDrawColor(editor->renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(editor->renderer, &cursor_rect);
}

void editor_cleanup(struct Editor *editor, int exit_status){
    SDL_DestroyRenderer(editor -> renderer);
    SDL_DestroyWindow(editor -> window);
    SDL_Quit();
    exit(exit_status);
}

void scroll_down(struct Editor *editor){
    editor->text_rect.y += 50;
    if (editor->text_rect.y > 0) editor->text_rect.y = 0;
}

void scroll_up(struct Editor *editor){
    editor->text_rect.y -= 50;
}


void sync_cursor(struct Editor *editor) {
    int r = 0;
    int c = 0;
    
    for (size_t i = 0; i < editor->gb->gap_left; i++) {
        if (editor->gb->buffer[i] == '\n') {
            r++;
            c = 0;
        } else {
            c++;
        }
    }
    
    editor->cursor_row = r;
    editor->cursor_col = c;
    
    int cursor_y_screen = editor->text_rect.y + (r * editor->char_h);

    if (cursor_y_screen < 50) {
        editor->text_rect.y = 50 - (r * editor->char_h);
    }

    int bottom_margin = SCREEN_HEIGHT;
    if (cursor_y_screen + editor->char_h > bottom_margin) {
        editor->text_rect.y = bottom_margin - ((r + 1) * editor->char_h);
    }
    
    int cursor_x_screen = editor->text_rect.x + (c * editor->char_w);
        
    if (cursor_x_screen < 20) {
        editor->text_rect.x = 20 - (c * editor->char_w);
    }
    
    if (cursor_x_screen + editor->char_w > SCREEN_WIDTH - 20) {
        editor->text_rect.x = (SCREEN_WIDTH - 20) - ((c + 1) * editor->char_w);
    }

}



void render_single_line(struct Editor *editor, const char *content, int y) {
   
    if (y + editor->char_h < 0 || y > SCREEN_HEIGHT) return;

    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *surf = TTF_RenderUTF8_Blended(editor->font, content, white);
    if (surf) {
        SDL_Texture *tex = SDL_CreateTextureFromSurface(editor->renderer, surf);
        SDL_Rect dest = { editor->text_rect.x, y, surf->w, surf->h };
        SDL_RenderCopy(editor->renderer, tex, NULL, &dest);
        SDL_DestroyTexture(tex);
        SDL_FreeSurface(surf);
    }
}

/**
 * This function allows to use new lines without SDL Wrapped function,
 * making it possible for horizontal scrolling.
 */
void draw_lines(struct Editor *editor) {
    char *text = gb_get_string(editor->gb);
    int current_y = editor->text_rect.y;
    char *start = text;
    char *end;

    while ((end = strchr(start, '\n')) != NULL) {
        size_t len = end - start;
        if (len > 0) {
            char *temp_line = strndup(start, len);
            render_single_line(editor, temp_line, current_y);
            free(temp_line);
        }
        current_y += editor->char_h;
        start = end + 1; 
    }

    if (*start != '\0') {
        render_single_line(editor, start, current_y);
    }

    free(text);
}

