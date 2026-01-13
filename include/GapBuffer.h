#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H

#include <stddef.h>


typedef struct {
    char *buffer;
    size_t capacity;
    size_t gap_left;
    size_t gap_right;
} GapBuffer;

// Available functions
// TODO: Might remove some but now just for testing phase
// leaving all functions public.
GapBuffer* gb_create(size_t initial_capacity);
void gb_insert(GapBuffer *gb, const char *input);
void gb_move_cursor(GapBuffer *gb, size_t position);
void gb_backspace(GapBuffer *gb);
void gb_render(GapBuffer *gb);
void gb_visual_debug(GapBuffer *gb);
void gb_cleanup(GapBuffer *gb);
char* gb_get_string(GapBuffer *gb);

#endif