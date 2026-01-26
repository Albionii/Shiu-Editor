#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/file_io.h"
#include "../include/editor.h"


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


void open_file(struct Editor *editor, char *file_path){
    FILE *file = fopen(file_path, "r");
    if (!file) {
        printf("Could not open file");
    }
    
    //Clean current buffer before reading the file. DO NOT DESTROY IT !!!
    gb_cleanup(editor->gb);
    
    char chunk[4096];
    size_t bytes_read;
    
    while ((bytes_read = fread(chunk, 1, sizeof(chunk) - 1, file)) > 0){
        chunk[bytes_read] = '\0';
        gb_insert(editor->gb, chunk);
    }
    
    //I need this to put the cursor at the beggining of the editor
    editor->gb->gap_left = 0;
    editor->gb->gap_right = 0;
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