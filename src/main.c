#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../include/gap_buffer.h"
#include "../include/file_io.h"
#include "../include/editor.h"


int main() {
    struct Editor editor = {0};

    if(sdl_initialize(&editor)){
        editor_cleanup(&editor, EXIT_FAILURE);
    };

    editor.gb = gb_create(1024);

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
                case SDL_MOUSEWHEEL: 
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