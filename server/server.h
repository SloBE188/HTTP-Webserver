#ifndef SERVER_H
#define SERVER_H

char* load_file(const char* filename, long* filesize);
const char* get_mime_type(const char* path);
int main(int argc, char** argv);


#endif