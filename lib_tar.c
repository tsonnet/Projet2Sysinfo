#include "lib_tar.h"

// ./tests test.tar 

/**
 * Checks whether the archive is valid.
 *
 * Each non-null header of a valid archive has:
 *  - a magic value of "ustar" and a null,
 *  - a version value of "00" and no null,
 *  - a correct checksum
 *
 * @param tar_fd A file descriptor pointing to the start of a file supposed to contain a tar archive.
 *
 * @return a zero or positive value if the archive is valid, representing the number of non-null headers in the archive,
 *         -1 if the archive contains a header with an invalid magic value,
 *         -2 if the archive contains a header with an invalid version value,
 *         -3 if the archive contains a header with an invalid checksum value
 */
int check_archive(int tar_fd) {
    int counter =0;
    struct posix_header* curr = malloc(sizeof(struct posix_header));
    char * buffer = calloc(512,sizeof(char));
    int re =read(tar_fd,buffer,512);
    printf("  Buffer = [");
    for (int i = 0; i < 512 - 1; i += 1) {
      
      printf("%c,", buffer[i]);
    }

    printf("%d]\n", buffer[512]);
    printf("Checksum : %c,%c,%c,%c\n",buffer[149],buffer[150],buffer[151],buffer[152]);
    printf("Number of bytes read %d\n", re);
    if(re ==0) return 0;
    while(is_end(buffer)){
        int res = Read_posix_header(buffer,curr);
        if(res!=0)return res;
        counter+=1;
        int re =read(tar_fd,buffer,512);
        if(re==0) break;
        printf("counter %d\n",counter);
    }
    free(curr);
    free(buffer);
    return counter;
}

/**
 * Checks whether an entry exists in the archive. Thibaut
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive.
 *
 * @return zero if no entry at the given path exists in the archive,
 *         any other value otherwise.
 */
int exists(int tar_fd, char *path) {
    return 0;
}

/**
 * Checks whether an entry exists in the archive and is a directory. Thibaut
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive.
 *
 * @return zero if no entry at the given path exists in the archive or the entry is not a directory,
 *         any other value otherwise.
 */
int is_dir(int tar_fd, char *path) {
    return 0;
}

/**
 * Checks whether an entry exists in the archive and is a file. Nico
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive.
 *
 * @return zero if no entry at the given path exists in the archive or the entry is not a file,
 *         any other value otherwise.
 */
int is_file(int tar_fd, char *path) {
    return 0;
}

/**
 * Checks whether an entry exists in the archive and is a symlink. Nico
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive.
 * @return zero if no entry at the given path exists in the archive or the entry is not symlink,
 *         any other value otherwise.
 */
int is_symlink(int tar_fd, char *path) {
    return 0;
}


/**
 * Lists the entries at a given path in the archive. Nico
 * list() does not recurse into the directories listed at the given path.
 *
 * Example:
 *  dir/          list(..., "dir/", ...) lists "dir/a", "dir/b", "dir/c/" and "dir/e/"
 *   ├── a
 *   ├── b
 *   ├── c/
 *   │   └── d
 *   └── e/
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive. If the entry is a symlink, it must be resolved to its linked-to entry.
 * @param entries An array of char arrays, each one is long enough to contain a tar entry path.
 * @param no_entries An in-out argument.
 *                   The caller set it to the number of entries in `entries`.
 *                   The callee set it to the number of entries listed.
 *
 * @return zero if no directory at the given path exists in the archive,
 *         any other value otherwise.
 */
int list(int tar_fd, char *path, char **entries, size_t *no_entries) {
    return 0;
}

/**
 * Reads a file at a given path in the archive. Thibaut
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive to read from.  If the entry is a symlink, it must be resolved to its linked-to entry.
 * @param offset An offset in the file from which to start reading from, zero indicates the start of the file.
 * @param dest A destination buffer to read the given file into.
 * @param len An in-out argument.
 *            The caller set it to the size of dest.
 *            The callee set it to the number of bytes written to dest.
 *
 * @return -1 if no entry at the given path exists in the archive or the entry is not a file,
 *         -2 if the offset is outside the file total length,
 *         zero if the file was read in its entirety into the destination buffer,
 *         a positive value if the file was partially read, representing the remaining bytes left to be read to reach
 *         the end of the file.
 *
 */
ssize_t read_file(int tar_fd, char *path, size_t offset, uint8_t *dest, size_t *len) {
    return 0;
}

int Read_posix_header(char* buffer, struct posix_header* to_fill){
    memcpy(&(to_fill->name),buffer,100); //name
    memcpy(&(to_fill->magic),buffer+257,6);//value
    memcpy(&(to_fill->version),buffer+263,2); //Version 
    memcpy(&(to_fill->chksum),buffer+148,8); //chcksm 
    if (strcmp(TMAGIC,to_fill->magic)!=0) return -1;
    if(strcmp(TVERSION,to_fill->version)!=0) return -2;
    if((uint64_t) (to_fill->chksum) != checksum(buffer))return -3;
    return 1;
}

int is_end(char* buffer){
    for(int i=0; i<512;i++){
        if(buffer[i] != 0){
            return 1;
        }
    }
    return 0;
}

uint64_t checksum(char * buffer){ 

    uint64_t mysum =0;
    for(int i=0;i<512;i++){
            mysum += TAR_INT(buffer[i]);
    }
    return mysum;
}