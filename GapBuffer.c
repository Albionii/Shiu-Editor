#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *buffer;
int capacity = 50;
int gap_left = 0;
int gap_right = 9;

void grow(int gap_extra_size){
    char *new_buffer = malloc(capacity + gap_extra_size);
    for (int i = 0; i < capacity + gap_extra_size; i++) {
		new_buffer[i] = '_';
	}
    
    memcpy(new_buffer, buffer, gap_left);
    int right_size = capacity - gap_right - 1;
    memcpy(
        new_buffer + gap_left + gap_extra_size,
        buffer + gap_right + 1,
        right_size
    );
    
    gap_right = gap_left + gap_extra_size - 1;
    free(buffer);
    buffer = new_buffer;
    capacity += gap_extra_size;
}

void move_left(int position){
    while(position < gap_left){
		gap_left--;
		gap_right--;
		buffer[gap_right + 1] = buffer[gap_left];
		buffer[gap_left] = '_';
	}
}

void move_right(int position){
   	while(position > gap_left){
		gap_left++;
		gap_right++;
		buffer[gap_left-1] = buffer[gap_right];
		buffer[gap_right] = '_';
	}
}

void move_cursor(int position){
    if (position < gap_left){
        move_left(position);
    }else {
        move_right(position);
    }
    
}

void remove_string(int position){
    if (position != gap_left) {
        move_cursor(position);
    }
    buffer[gap_right + 1] = '_';
}


void insert(const char *input, int position){
    int len = strlen(input);
    
    if (gap_right - gap_left + 1 < len){
        grow(len > 10 ? len : 10);
    }
    
    if (position != gap_left) {
        move_cursor(position);
    }
    
    for (int i = 0; i < len; i++){
        buffer[gap_left++] = input[i];
    }
}

void print_buffer(){
    for (int i = 0; i < capacity; i++) {
        putchar(buffer[i]);
    }
    putchar('\n');
}


int main(){
    buffer = malloc(capacity);
    for (int i = 0; i < capacity; i++) {
        buffer[i] = '_';
    }
    
    insert("Hello ld", 0);
    print_buffer();

    insert("ta", 2);
    print_buffer();

    printf("l : %d\n", gap_left);
    printf("r : %d\n", gap_right);

    insert("bu", 20);
    print_buffer();

    printf("l : %d\n", gap_left);
    printf("r : %d\n", gap_right);

    remove_string(2);
    print_buffer();

    free(buffer);
    return 0;
    
}