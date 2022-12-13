#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "lib_tar.h"

/**
 * You are free to use this file to write tests for your implementation
 */

void debug_dump(const uint8_t *bytes, size_t len) {
    for (int i = 0; i < len;) {
        printf("%04x:  ", (int) i);

        for (int j = 0; j < 16 && i + j < len; j++) {
            printf("%02x ", bytes[i + j]);
        }
        printf("\t");
        for (int j = 0; j < 16 && i < len; j++, i++) {
            printf("%c ", bytes[i]);
        }
        printf("\n");
    }
}
void test_list(int fd,char* path,char** entries, size_t* no_entries){
    int ret = list(fd,path,entries,no_entries);
    printf("is_list returned %d \n", ret);
    printf("no_entries vaut %ld\n",*no_entries);
    printf("Entries list vaut : [");
    for (size_t i = 0; i < *no_entries-1; i++){
        printf("%s\n",entries[i]);
    }
    printf("%s]",entries[(*no_entries)-1]);
    
}

void test_archive(int fd){
    int ret = check_archive(fd);
    printf("check_archive returned %d\n", ret);
}

void test_is_file(int fd,char* path){
    int ret = is_file(fd,path);
    printf("is_file returned %d\n", ret);
}

void test_is_symlink(int fd,char* path){
    int ret = is_symlink(fd,path);
    printf("is_symlink returned %d \n",ret);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s tar_file\n", argv[0]);
        return -1;
    }

    int fd = open(argv[1] , O_RDONLY);
    if (fd == -1) {
        perror("open(tar_file)");
        return -1;
    }

    char* path = argv[2];

    test_archive(fd);
    test_is_file(fd,path);
    test_is_symlink(fd,path);

    
    char** entries = (char **) malloc(100*sizeof(char*));
    size_t* n_entries = (size_t*) malloc(sizeof(size_t));
    *n_entries = 0;
    test_list(fd,path,entries,n_entries);

    return 0;
}