#pragma once
#define MAX_FILE_PATH_SIZE 1024
#include "gap_buffer.h"

struct Editor;

char* get_filename_from_zenity();
void save_to_file(struct Editor *editor, char *filename);
void open_file(struct Editor *editor, char *file_path);


