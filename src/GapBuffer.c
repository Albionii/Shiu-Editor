#include "../include/GapBuffer.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * Initialize the GapBuffer
 */
GapBuffer* gb_create(size_t initial_capacity){
    GapBuffer *gb = malloc(sizeof(GapBuffer));
    gb->capacity = initial_capacity > 0 ? initial_capacity : 10;
    gb->buffer = malloc(gb->capacity);
    gb->gap_left = 0;
    gb->gap_right = gb->capacity - 1;
    return gb;
}


/**
 * When the gap is smaller than the incoming input of chars,
 * increase the size of the gap and buffer.
 */
void gb_grow(GapBuffer *gb, size_t gap_extra_size){
    size_t old_capacity = gb->capacity;
    size_t new_capacity = old_capacity * 2;
    if (new_capacity < old_capacity + gap_extra_size) {
        new_capacity = old_capacity + gap_extra_size + 1024;
    }
   
    //Instead of malloc() here the realloc() is better, if the gap is increased
    //but is the capacity of the buffer does not need to change then this comes
    //in clutch.
    char *new_buffer = realloc(gb->buffer, new_capacity);
    if (!new_buffer){
        printf("Error on realloc");
        return;
    }
    
    gb->buffer = new_buffer;
    
    size_t right_size = old_capacity - gb->gap_right - 1;
    size_t new_gap_right = new_capacity - right_size - 1;
    
    memmove(gb->buffer + new_gap_right + 1, 
            gb->buffer + gb->gap_right + 1, 
            right_size
    );  
    
    gb->gap_right = new_gap_right;
    gb->capacity = new_capacity;
}


void gb_move_cursor(GapBuffer *gb, size_t position){
   //Move cursor to left
    while(position < gb->gap_left){
		gb->gap_left--;
		gb->gap_right--;
		gb->buffer[gb->gap_right + 1] = gb->buffer[gb->gap_left];
	}
    
    //Move cursor to right
    while (position > gb->gap_left) {
        gb->buffer[gb->gap_left] = gb->buffer[gb->gap_right + 1];
        gb->gap_left++;
        gb->gap_right++;
    }
}

void gb_backspace(GapBuffer *gb){
    if (gb->gap_left > 0) {
        gb->gap_left--;
    }
}

void gb_destroy(GapBuffer *gb){
    free(gb->buffer);
    free(gb);
}

void gb_cleanup(GapBuffer *gb) {
    gb->gap_left = 0;
    gb->gap_right = gb->capacity - 1;
}


void gb_insert(GapBuffer *gb, const char *input){
    size_t len = strlen(input);
    
    if (gb->gap_right - gb->gap_left + 1 <= len){
        gb_grow(gb, len > 10 ? len : 10);
    }
    
    for (int i = 0; i < len; i++){
        gb->buffer[gb->gap_left++] = input[i];
    }
}


/**
 * For debugging to see the "gap" in the buffer.
 */
void print_buffer(GapBuffer *gb) {
    for (size_t i = 0; i < gb->capacity; i++) {
        if (gb->buffer[i] == '\0' || gb->buffer[i] == '\n') {
            putchar('.');
        } else {
            putchar(gb->buffer[i]);
        }
    }
    putchar('\n');

    
    printf(" (L:%zu, R:%zu, Cap:%zu)\n\n", gb->gap_left, gb->gap_right, gb->capacity);
}

void gb_render(GapBuffer *gb){
    for (size_t i = 0; i < gb->gap_left; i++) {
        putchar(gb->buffer[i]);
    }
    
    for (size_t i = gb->gap_right+1; i < gb->capacity; i++){
        putchar(gb->buffer[i]);
    }
   putchar('\n'); 
}

char* gb_get_string(GapBuffer *gb) {
    size_t left_len = gb->gap_left;
    size_t right_len = gb->capacity - (gb->gap_right + 1);
    size_t total_len = left_len + right_len;

    char *str = malloc(total_len + 1);
    if (!str) return NULL;

    memcpy(str, gb->buffer, left_len);
    memcpy(str + left_len, gb->buffer + gb->gap_right + 1, right_len);
    
    str[total_len] = '\0'; 
    return str;
}