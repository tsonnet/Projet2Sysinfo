#include "lib_tar.h"
#include "print_function.h"

// ./tests test.tar
// tar cvf file.tar

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

#define ASCII_TO_NUMBER(num) ((num)-48) //Converts an ascii digit to the corresponding number

char* buffer;
tar_header_t* structure;
int counter;

int check_archive(int tar_fd) {

    ///////////////// Initialisation des variables ////////////////////
    counter =0;
    structure = malloc(sizeof(tar_header_t));
    buffer = calloc(512,sizeof(char));
    //////////////////////////////////////////////////////////////////

    int re =read(tar_fd,buffer,512);

    ///////////////// Test Print /////////////////////////////////////
    print_list_entier(buffer, 512, "Buffer");
    print_list_chaine(buffer, 512, "Buffer");
    print_chaine(buffer+148,8,"Checksum");
    printf("Number of bytes read %d\n", re);
    ///////////////////////////////////////////////////////////////////

    if(re ==0) return 0;
    char name[32];
    memcpy(name,buffer+263,32); //on enregistre le nom de l'utilisateur, qui sera toujours le meme, sauf pour les données du fichier

    while(1){

        int res = Read_posix_header(buffer,structure,name);
        if(res == 4){
            break;
        }
        if(res!=0){
            return res;
        }
        int re = read(tar_fd,buffer,512);
        if(re==0) {
            break;
        }
        counter+=1;
        printf("counter %d\n",counter);
    }

    free(structure);
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

    buffer = calloc(512,sizeof(char));
    int re =read(tar_fd,buffer,512);


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

    exists(tar_fd,path); //check si l'entrée existe, on peut donc ensuite considérer que notre boucle s'arretera quoi qu'il arrive

    buffer = calloc(512,sizeof(char));
    lseek(tar_fd,0,SEEK_SET); // IMPORTANT remettre le fichier au début
    int re =read(tar_fd,buffer,512);
    int len_path = strlen(path);
    
    if(re ==0) return 0;
    counter = 0;

    while(1){
        //print_struct_header(buffer);
        char* current_path = (char*) malloc(len_path);
        memcpy(current_path,buffer,len_path);
        if(strcmp(path,current_path) == 0){
            char* current_type_flag = (char*) malloc(1);
            memcpy(current_type_flag,buffer+counter+156,1);
            if(atoi(current_type_flag) != 0  && *current_type_flag != '\0'){
                free(current_type_flag);
                free(current_path);
                return 0;
            }
            else{
                free(current_path);
                free(current_type_flag);
                return 1;
            }
        }
        else{
            free(current_path);
            int re = read(tar_fd,buffer,512);
            if(re ==0)return 0;
        }
    }
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
    exists(tar_fd,path); //check si l'entrée existe, on peut donc ensuite considérer que notre boucle s'arretera quoi qu'il arrive

    buffer = calloc(512,sizeof(char));
    lseek(tar_fd,0,SEEK_SET); // IMPORTANT remettre le fichier au début
    int re =read(tar_fd,buffer,512);
    int len_path = strlen(path);

    if(re ==0) return 0;
    counter = 0;

    while(1){
        char* current_path = (char*) malloc(len_path);
        memcpy(current_path,buffer,len_path);
        if(strcmp(path,current_path) == 0){
            char* current_type_flag = (char*) malloc(1);
            memcpy(current_type_flag,buffer+counter+156,1);
            if(atoi(current_type_flag)!=SYMTYPE){
                free(current_type_flag);
                free(current_path);
                return 0;
            }
            else{
                free(current_path);
                free(current_type_flag);
                return 1;
            }
        }
        else{
            free(current_path);
            int re = read(tar_fd,buffer,512);
            if(re ==0) return 0;
        }
    }
    
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

    exists(tar_fd,path);

    ///////////////// récupérer l'invariant //////////////
    char* buffer_for_name = (char*) calloc(512,sizeof(char));
    int res = read(tar_fd,buffer_for_name,100);
    char* name = (char*)malloc(32);
    memcpy(name,buffer_for_name+265,32);
    free(buffer_for_name);
    //////////////////////////////////////////////////////

    buffer = calloc(512,sizeof(char));
    int len_path = strlen(path);
    list_recu(tar_fd,buffer,path,len_path,entries,no_entries,name);

}

int list_recu(int tar_fd,char* buffer,char *path,size_t len_path, char **entries, size_t *no_entries,char*invariant) {


    char* current_path = (char*) malloc(len_path);
    char* all_current_path = (char*)malloc(100);

    int re =read(tar_fd,buffer,512);

    ////////////////// Condition de sortie ///////////
    char* name = (char*) malloc(32);
    memcpy(name,buffer+263,32);
    if(strcmp(name,invariant)!= 0){
        return 0;
    }
    free(name);
    /////////////////////////////////////////////////

    memcpy(current_path,buffer,len_path);
    memcpy(all_current_path,buffer,100);

    if(strcmp(current_path,path) != 0){ //tant qu'on a pas trouvé le chemin
        free(current_path);
        free(all_current_path);
        list_recu(tar_fd,buffer,path,len_path,entries,no_entries,invariant);
    }
    else{
        if (!contains(all_current_path,entries,no_entries)){
            entries[*no_entries] = all_current_path;
            *no_entries ++;
            free(current_path);
            list_recu(tar_fd,buffer,path,len_path,entries,no_entries,invariant);
        }
        else{
            free(all_current_path);
            free(current_path);
            list_recu(tar_fd,buffer,path,len_path,entries,no_entries,invariant);
        }
    }

    return 0;
}

// Question à poser : est ce que les fichiers sont dans l'ordre ?

int contains(char* path, char **entries,size_t* no_entries){
    for (size_t i = 0; i < *no_entries; i++){
        char* path_to_compare = malloc(100);
        memcpy(path_to_compare,path,strlen(entries[i])); //On ne veut pas les sous-dosiers, on se limite à la longeur des entrées
        if(strcmp(path_to_compare,entries[i])){
            return 1;
        }
        free(path_to_compare);
    }
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

int Read_posix_header(char* buffer, tar_header_t* to_fill,char * uname){

    memcpy(&(to_fill->name),buffer,100); //name
    memcpy(&(to_fill->magic),buffer+257,5);//value
    memcpy(&(to_fill->version),buffer+263,2); //Version 
    memcpy(&(to_fill->chksum),buffer+148,8); //chcksum
    memcpy(&(to_fill->uname),buffer+263,32); //uname
    print_struct_header(buffer);

    if(strcmp(uname,to_fill->uname) != 0) return 4;
    if (strcmp(TMAGIC,to_fill->magic)!=0)return -1;  
    //if(strcmp(TVERSION,to_fill->version)!=0) return -2;
    if(!checkChecksum(buffer,buffer+148))return -3;

    return 0;
}

int is_end(char* buffer){
    for(int i=0; i<512;i++){
        if(buffer[i] != 0){
            return 1;
        }
    }
    return 0;
}

/**
 * Decode a TAR octal number.
 * Ignores everything after the first NUL or space character.
 * @param data A pointer to a size-byte-long octal-encoded
 * @param size The size of the field pointer to by the data pointer
 * @return
 */

static uint64_t decodeTarOctal(char* data) {
    unsigned char* currentPtr = (unsigned char*) data+17;
    uint64_t sum = 0;
    uint64_t currentMultiplier = 1;
    //Skip everything after the last NUL/space character
    //In some TAR archives the size field has non-trailing NULs/spaces, so this is neccessary
    unsigned char* checkPtr = currentPtr; //This is used to check where the last NUL/space char is
    for (; checkPtr >= (unsigned char*) data; checkPtr--) {
        if ((*checkPtr) == 0 || (*checkPtr) == ' ') {
            currentPtr = checkPtr - 1;
        }
    }
    for (; currentPtr >= (unsigned char*) data; currentPtr--) {
        sum += ASCII_TO_NUMBER(*currentPtr)*currentMultiplier;
        currentMultiplier *= 8;
    }
    return sum;
}

/**
     * Return true if and only if the header checksum is correct
     * @return
     */
    int checkChecksum(char * buffer,char* checksum) {

        char originalChecksum[8];
        memcpy(originalChecksum, checksum, 8);
        memset(checksum, ' ', 8);
       
        int64_t unsignedSum = 0;
        for (int i = 0; i < 512; i++) {
            unsignedSum += ((unsigned char*) buffer)[i];
        }
        
        memcpy(checksum, originalChecksum, 8);
        uint64_t referenceChecksum = decodeTarOctal(originalChecksum);
    
        return (referenceChecksum == unsignedSum);
    }
