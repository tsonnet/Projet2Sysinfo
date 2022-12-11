#include "print_function.h"

void print_list_entier(char * list, size_t size,char* name){
    printf("%s value : [",name);
    for (int i = 0; i < size-1; i += 1) {
      
      printf("%d,", list[i]);
    }

    printf("%d]\n", list[512]);
}

void print_list_chaine(char * list, size_t size,char* name){
    printf("%s value : [",name);
    for (int i = 0; i < size-1; i += 1) {
      
      printf("%c,", list[i]);
    }

    printf("%c]\n", list[512]);
}

void print_chaine(char* mystring, size_t size_string,char* name){
    printf("%s value : ",name);
    for (int i = 0; i < size_string; i += 1) {
      
      printf("%c,", mystring[i]);
    }
}

void print_struct_header(char *buffer){
    char dest_name[100];               /*   0 */
    char dest_mode[8];                 /* 100 */
    char dest_uid[8];                  /* 108 */
    char dest_gid[8];                  /* 116 */
    char dest_size[12];                /* 124 */
    char dest_mtime[12];               /* 136 */
    char dest_chksum[8];               /* 148 */
    char dest_typeflag[1];                /* 156 */
    char dest_linkname[100];           /* 157 */
    char dest_magic[6];                /* 257 */
    char dest_version[2];              /* 263 */
    char dest_uname[32];               /* 265 */
    char dest_gname[32];               /* 297 */
    char dest_devmajor[8];             /* 329 */
    char dest_devminor[8];             /* 337 */
    char dest_prefix[155];             /* 345 */
    char dest_padding[12];             /* 500 */

    memcpy(dest_name,buffer,100);
    memcpy(dest_mode,buffer+100,8);
    memcpy(dest_uid,buffer+108,8);
    memcpy(dest_gid,buffer+116,8);
    memcpy(dest_size,buffer+124,12);
    memcpy(dest_mtime,buffer+136,12);
    memcpy(dest_chksum,buffer+148,8);
    memcpy(dest_typeflag,buffer+156,1);
    memcpy(dest_linkname,buffer+157,100);
    memcpy(dest_magic,buffer+257,6);
    memcpy(dest_version,buffer+263,2);
    memcpy(dest_uname,buffer+265,32);
    memcpy(dest_gname,buffer+297,32);
    memcpy(dest_devmajor,buffer+329,8);
    memcpy(dest_devminor,buffer+337,8);
    memcpy(dest_prefix,buffer+345,155);
    memcpy(dest_padding,buffer+500,12);

    printf("Name : %s\n",dest_name);
    printf("Mode : %s\n",dest_mode);
    printf("Uid : %s\n",dest_uid);
    printf("Gid : %s\n",dest_gid);
    printf("Size : %s\n",dest_size);
    printf("Mtime : %s\n",dest_mtime);
    printf("Chksum : %s\n",dest_chksum);
    printf("type_flag : %s\n",dest_typeflag);
    printf("Linkname : %s\n",dest_linkname);
    printf("Magic : %s\n",dest_magic);
    printf("Version : %s\n",dest_version);
    printf("Uname : %s\n",dest_uname);
    printf("Gname : %s\n",dest_gname);
    printf("Devmajor : %s\n",dest_devmajor);
    printf("DevMinor : %s\n",dest_devminor);
    printf("Prefix : %s\n",dest_prefix);
    printf("Padding : %s\n",dest_padding);
}