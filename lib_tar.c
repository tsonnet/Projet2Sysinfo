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
int verbose = 0;

int check_archive(int tar_fd) {

    ///////////////// Initialisation des variables ////////////////////
    counter =0;
    structure = malloc(sizeof(tar_header_t));
    buffer = calloc(512,sizeof(char));
    lseek(tar_fd,0,SEEK_SET);
    //////////////////////////////////////////////////////////////////

    while(read(tar_fd,buffer,512) != 0){
      
        int res = Read_posix_header(buffer,structure);

        ////////// Décalage si on tombe sur un dossier ///////////////
        int current_position = lseek(tar_fd,0,SEEK_CUR);
        int size_of_file = TAR_INT(structure->size);
        if(size_of_file > 1){

            if(size_of_file%512 == 0){
                lseek(tar_fd,current_position+512*(size_of_file/512),SEEK_SET);
            }
            else{
                lseek(tar_fd,current_position+512*((size_of_file/512)+1),SEEK_SET);
            }
        }
       
        ////////////////////////////////////////////////////////////////
        /**
        printf("la valeur de version vaut %s\n", structure->version);
        printf("la taille de version devrait valoir 2 mais vaut %ld", strlen(structure->version));
        printf("la vraie valeur de version vaut %s\n", TVERSION);
        **/

        if(res < 0){
            if(structure->name[0] == '\0'){
                continue;
            }
            else{
                return res;
            }
        }
        else{
            counter+=1;
        }

    }

    free(structure);
    free(buffer);
    return counter;
}

// Question à poser en complexe : Pourquoi on entre dans le fichier texte ?

/**
 * Checks whether an entry exists in the archive. Thibaut
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive.
 *
 * @return zero if no entry at the given path exists in the archive,
 *         any other value otherwise.
 *         Return the size of the file in bytes +1 to prevent the case where the file length is 0 
 */

int exists(int tar_fd, char *path) {
    lseek(tar_fd,0,SEEK_SET);
    buffer = calloc(512,sizeof(char));
    tar_header_t  Header;
    int re =read(tar_fd,buffer,512);
    if(re == 0){return 0;}
    Read_posix_header(buffer,&Header);
    int length= TAR_INT( Header.size);
    while (strcmp(Header.name,path)!=0)
    {
        //printf("%s\n",Header->name);
        lseek(tar_fd,length,SEEK_CUR);
        int re =read(tar_fd,buffer,512);
        if(re != 512) return 0;
        Read_posix_header(buffer,&Header);
        length = TAR_INT(Header.size);
        length = count_block(length)*512;
    }
    length = TAR_INT(Header.size);
    if(strcmp(Header.name,path)==0) return length+1;
    return 0;
}
int count_block(int len){
    int to_ret = len/512;
    if(len%512) return to_ret+1;
    return to_ret;
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
    lseek(tar_fd,0,SEEK_SET);
    buffer = calloc(512,sizeof(char));
    tar_header_t  Header;
    int re =read(tar_fd,buffer,512);
    if(re == 0){return 0;}
    Read_posix_header(buffer,&Header);
    int length= TAR_INT( Header.size);
    length = count_block(length)*512;
    while (strcmp(Header.name,path)!=0)
    { 
        lseek(tar_fd,length,SEEK_CUR);
        int re =read(tar_fd,buffer,512);
        if(re != 512) return 0;
        Read_posix_header(buffer,&Header);
        length = TAR_INT(Header.size);
        length = count_block(length)*512;
    }
    return Header.typeflag==DIRTYPE;
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

    if (exists(tar_fd,path) == 0){return 0;}//check si l'entrée existe, on peut donc ensuite considérer que notre boucle s'arretera quoi qu'il arrive

    buffer = calloc(512,sizeof(char));
    lseek(tar_fd,0,SEEK_SET); // IMPORTANT remettre le fichier au début
    int re =read(tar_fd,buffer,512);
    
    if(re ==0) return 0;
    counter = 0;

    while(1){
        if(verbose) print_struct_header(buffer);
        char* current_path = (char*) malloc(100);
        memcpy(current_path,buffer,100);
        if(verbose) printf("current path : %s\n",current_path);
        if(strcmp(path,current_path) == 0){
            char* current_type_flag = (char*) malloc(1);
            memcpy(current_type_flag,buffer+counter+156,1);
            if(atoi(current_type_flag) != 0  && *current_type_flag != '\0'){
                free(current_type_flag);
                free(current_path);
                if(verbose) printf("Je ne suis pas un fichier zebi\n");
                return 0;
            }
            else{
                char* size_of_file = (char*) malloc(8);
                memcpy(size_of_file,buffer+128,8);
                int size_of_file_int = TAR_INT(size_of_file);
                free(current_path);
                free(current_type_flag);
                free(size_of_file);
                return size_of_file_int;
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
    if (exists(tar_fd,path) == 0){return 0;}

    lseek(tar_fd,0,SEEK_SET);
    buffer = calloc(512,sizeof(char));
    tar_header_t  Header;
    int re =read(tar_fd,buffer,512);
    if(re == 0){return 0;}
    Read_posix_header(buffer,&Header);
    int length= TAR_INT( Header.size);
    length = count_block(length)*512;
    while (strcmp(Header.name,path)!=0)
    { 
        lseek(tar_fd,length,SEEK_CUR);
        int re =read(tar_fd,buffer,512);
        if(re != 512) return 0;
        Read_posix_header(buffer,&Header);
        length = TAR_INT(Header.size);
        length = count_block(length)*512;
    }
    return Header.typeflag == SYMTYPE;
}

int is_symlink_for_list(int tar_fd, char *path) {
    if (exists(tar_fd,path) == 0){return 0;}

    lseek(tar_fd,0,SEEK_SET);
    buffer = calloc(512,sizeof(char));
    tar_header_t  Header;
    int re =read(tar_fd,buffer,512);
    if(re == 0){return 0;}
    Read_posix_header(buffer,&Header);
    int length= TAR_INT( Header.size);
    length = count_block(length)*512;
    while (strcmp(Header.name,path)!=0)
    { 
        lseek(tar_fd,length,SEEK_CUR);
        int re =read(tar_fd,buffer,512);
        if(re != 512) return 0;
        Read_posix_header(buffer,&Header);
        length = TAR_INT(Header.size);
        length = count_block(length)*512;
    }
    
    if(Header.typeflag == SYMTYPE){
        int mark = 0;
        for (size_t i = 0; i < strlen(path); i++){
            if (path[i] == '/'){
                mark = i;
            }
        }
        memcpy(path+mark+1,Header.linkname,100-mark-1);
        return 1;
    }
    else{
        return 0;
    }
    return Header.typeflag == SYMTYPE;
}

int is_symlink_for_list2(int tar_fd, char *path) {
    if (exists(tar_fd,path) == 0){return 0;}

    lseek(tar_fd,0,SEEK_SET);
    buffer = calloc(512,sizeof(char));
    tar_header_t  Header;
    int re =read(tar_fd,buffer,512);
    if(re == 0){return 0;}
    Read_posix_header(buffer,&Header);
    int length= TAR_INT( Header.size);
    length = count_block(length)*512;
    while (strcmp(Header.name,path)!=0)
    { 
        lseek(tar_fd,length,SEEK_CUR);
        int re =read(tar_fd,buffer,512);
        if(re != 512) return 0;
        Read_posix_header(buffer,&Header);
        length = TAR_INT(Header.size);
        length = count_block(length)*512;
    }
    
    if(Header.typeflag == SYMTYPE){
    
        memcpy(path,Header.linkname,100);
        return 1;
    }
    else{
        return 0;
    }
    return Header.typeflag == SYMTYPE;
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

    // Check si le path existe
    if (exists(tar_fd,path) == 0){return 0;}
    lseek(tar_fd,0,SEEK_SET);

    buffer = calloc(512,sizeof(char));
    int len_path = strlen(path);
    
    // Nouveau chemin pour enregistrer le linkname dans le cas d'un symlink
    char* new_path = malloc(100);
    memcpy(new_path,path,100);

    if(is_symlink_for_list(tar_fd,new_path)){
        lseek(tar_fd,0,SEEK_SET);
        printf("LE  NOUVEAU CHEMIN VAUT : %s \n",new_path);
        *no_entries = list_recu(tar_fd,buffer,new_path,strlen(new_path),entries,0);
        free(new_path);
        return 1;
    }
    else{
        lseek(tar_fd,0,SEEK_SET);
        *no_entries = list_recu(tar_fd,buffer,path,len_path,entries,0);
        return 1;
    }
}

int list_recu(int tar_fd,char* buffer,char *path,size_t len_path, char **entries,int no_entries) {
    

    char* current_path = (char*) malloc(100);

    int re =read(tar_fd,buffer,512);
    //////////////// Condition de sortie ////////////////
    if(re == 0){
        free(current_path);
        return no_entries;
    }
    /////////////////////////////////////////////////////
    //print_struct_header(buffer);

    memcpy(current_path,buffer,100);
    int current_position = lseek(tar_fd,0,SEEK_CUR);
    /////////////// Check File //////////////////////////
    int size_of_file = is_file(tar_fd,current_path); //0 sinon
    if(size_of_file%512 == 0){
        lseek(tar_fd,current_position+512*(size_of_file/512),SEEK_SET);
    }
    else{
        lseek(tar_fd,current_position+512*((size_of_file/512)+1),SEEK_SET);
    }

    /////////////// Main function ///////////////////////

    if(strncmp(current_path,path,len_path) != 0 || strcmp(current_path,path)==0){ //on ne veut pas le path dans la liste, juste les sous-fichiers/dossiers
         //tant qu'on a pas trouvé le chemin
        free(current_path);
        return list_recu(tar_fd,buffer,path,len_path,entries,no_entries);

    }
    else{
       
        if (contains(current_path,entries,no_entries)==1){
    
            //On ajoute le chemin dans la liste, et on incrémente sa taille
            entries[no_entries] = current_path;
            no_entries ++;
            return list_recu(tar_fd,buffer,path,len_path,entries,no_entries);

        }
        else{
            //Si elle contient déjà le path, on continue
            free(current_path);
            return list_recu(tar_fd,buffer,path,len_path,entries,no_entries);
        }
    }
}


int contains(char* path, char **entries,int no_entries){
    if(no_entries == 0){ //entries est vide
        return 1;
    }
    else{
        for (size_t i = 0; i < no_entries; i++){
            size_t len_entry_i = strlen(entries[i]);
            if(strncmp(path,entries[i],len_entry_i)==0){
                return 0;
            }
        }
    }
    return 1;   
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
    int length_file;
    int found = 0;
    int length;
    lseek(tar_fd,0,SEEK_SET); // To be sure we start at the beginning of the file
    buffer = calloc(512,sizeof(char));
    tar_header_t * Header = malloc(sizeof(tar_header_t));
    int re =read(tar_fd,buffer,512);
    Read_posix_header(buffer,Header);

    ////////// Check Symlink ///////////////

    char * new_path = malloc(100);
    memcpy(new_path,path,100);
    is_symlink_for_list2(tar_fd,new_path);
    lseek(tar_fd,0,SEEK_SET);

    //////////////////////////////////////////
    while (re==512) {// Read until EOF
        if (strcmp(Header->name, new_path) == 0) {
        found = 1;
        break;
        }
        length = count_block(TAR_INT(Header->size))*512;
        lseek(tar_fd,length, SEEK_CUR);
        re =read(tar_fd,buffer,512);
        Read_posix_header(buffer,Header);
    }

    if (found==0) {// We didn't find (found==0)! 
        free(new_path);
        return -1;
    }
    if (Header->typeflag != REGTYPE && Header->typeflag !=AREGTYPE) { //REGTYPE et AREGTYPE pour regular File
        free(new_path);
        return -1;
    }

    length_file = TAR_INT(Header->size);// Octal to Int

    if (offset > length_file) {// IS the offSet bigger thant length file 
        free(new_path);
        return -2;
    }

    lseek(tar_fd, offset, SEEK_CUR); // Put the reader at the good position
    if(*len> length_file-offset){
        *len = length_file-offset;
    }

    *len  = read(tar_fd, dest, *len); // Read the file and set the IN-out arg

    int to_ret = length_file - offset - *len;
    if(to_ret<0) {
        free(new_path);
        return 0;
    }
    free(new_path);
    return to_ret;
}

int Read_posix_header(char* buffer, tar_header_t* to_fill){

    memcpy(&(to_fill->name),buffer,100); //name
    memcpy(&(to_fill->size),buffer+124,12); //size 
    memcpy(&(to_fill->typeflag),buffer+156,1); //TypeFlag
    memcpy(&(to_fill->magic),buffer+257,6);//value
    memcpy(&(to_fill->version),buffer+263,2); //Version 
    memcpy(&(to_fill->chksum),buffer+148,8); //chcksum
    memcpy(&(to_fill->uname),buffer+263,32); //uname
    memcpy(&(to_fill->linkname),buffer+157,100); //linkname

    /**
    if(strlen(to_fill->name) == 18){
        char* type = malloc(1);
        type[0] = '2';
        memcpy(&(to_fill->typeflag),type,1);
        char* link = malloc(100);
        link = "test_complex/Nico/Rep1/";
        memcpy(&(to_fill->linkname),link,100);
    }
    **/

    
    
    //printf("la longueur du nom est : %ld",strlen(to_fill->name));
    
    if(strncmp(to_fill->magic,TMAGIC,TMAGLEN-1) != 0){
        return -1;  
    }
    if(strncmp(to_fill->version,TVERSION,TVERSLEN) != 0) {
        return -2;
    }
    if(!checkChecksum(buffer,buffer+148)){
        return -3;
    }
    
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

/**
 * Decode a TAR octal number.
 * Ignores everything after the first NUL or space character.
 * @param data A pointer to a size-byte-long octal-encoded
 * @param size The size of the field pointer to by the data pointer
 * @return
 */

uint64_t decodeTarOctal(char* data) {
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
