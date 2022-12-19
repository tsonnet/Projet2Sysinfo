#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "lib_tar.h"

/**
 * You are free to use this file to write tests for your implementation
 */

/**
Questions :

Pourquoi quand on itère dans nos headers avec la fonctions read, lorsqu'on atteint
un fichier, par exemple txt, le prochain truc qu'on lit c'est ce fichier, et un pas
un autre headers ? Est ce qu'on doit déplacer notre tête de fichier par la taille
de ce fichier pour arriver au prochain header ?

Comment tester notre fonction is_symlink ?

Notre fonction renvoit un pointeur,je suppose que pour que les tests
inginious puissent tester notre fonction on ne doit pas les free ?

Je ne comprends pas exactement comment gérer les symlink, est ce si notre header est un symlink,
le nom ne sera jamais égal au path ?

TODO :

is_file : fonctionne
Archive : revoir la condition de sortie
list : revoir la condition de sortie, le reste fonctionne
       gérer le cas SYMLINK en utlisant is_symlink

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
void test_list(int fd,char* path){
    char** entries = (char **) malloc(100*sizeof(char*));
    size_t* n_entries = (size_t*) malloc(sizeof(size_t));
    *n_entries = 0;
    int ret = list(fd,path,entries,n_entries);
    printf("Pour le chemin %s is_list returned %d,",path,ret);
    printf("no_entries vaut %ld et ",*n_entries);
    if(*n_entries == 0){
        printf("Entries list vaut : []");
    }
    else {
        printf("Entries list vaut : \n[");
        for (size_t i = 0; i < *n_entries-1; i++){
            printf("%s,",entries[i]);
        }
        printf("%s]\n",entries[(*n_entries)-1]);
    }
    for (size_t i = 0; i < *n_entries; i++){
        free(entries[i]);
    }
    free(entries);
    free(n_entries);
    
}

void test_archive(int fd){
    int ret = check_archive(fd);
    printf("L'archive est correcte et contient %d fichiers et dossiers\n", ret);
}

void test_is_file(int fd,char* path){
    int ret = is_file(fd,path);
    printf("        Is File nous retourne la valeur %d pour le path %s\n", ret,path);
}

void test_exist(int fd,char*path){
    int ret = exists(fd,path);
    if (ret == 0){
        printf("        Le chemin n'existe pas");
    }
    else{
        printf("        Le chemin existe et la longueur de celui-ci est de %d char pour le path %s\n", ret,path);
    }
    char* buff = malloc(6);
    read(fd,buff,6);
}

void line(void){
    printf("\n-----------------------------------------------------------------------------\n\n");
}

void test_IsDir(int fd,char* path){
    int ret = is_dir(fd,path);
    printf("        Is Dir nous retourne la valeur %d pour le path %s\n", ret,path);
}
void test_Read_file(int fd,char* path,int offset,int lenToRead){
    size_t longueur= lenToRead;
    uint8_t buffer[longueur+1];
    buffer[longueur]='\0';
    int ret = read_file(fd,path,offset,buffer,&longueur);
    printf("        La valuer de retour de Read file est %d  avec le path %s et le buffer est '%s' il a lu %ld\n",ret,path,buffer,longueur);
}

void test_is_symlink(int fd,char* path){
    int ret = is_symlink(fd,path);
    printf("is_symlink returned %d \n",ret);
}

int main(int argc, char **argv) {
    int fd;
    if (argc== 3) {
        printf("Usage: %s tar_file\n", argv[0]);


        fd = open(argv[1] , O_RDONLY);
        if (fd == -1) {
            perror("open(tar_file)");
            return -1;
        }

        char* path = argv[2];
        printf("%s\n",path);
        printf("On fait le test ou pas ? \n");
        test_exist(fd,path);
        test_IsDir(fd,path);
        test_list(fd,path);

        //gérer les free de entries
        /* 
        Own Test of Exist Is_dir and ReadFile
        First Test : with testThibaut.tar
        */
    }
    fd =open("testThibaut.tar",O_RDONLY);

    printf("Avant toute chose, est ce que l'achive est correcte ? Combien de fichiers et dossiers contient-elle ?\n\n");
    test_archive(fd);
    printf("\n");
    line();
    printf("Est ce que le chemin existe ? \n\n");
   
        test_exist(fd,"test_complex/");
        test_exist(fd,"test_complex/Nico/");
        test_exist(fd,"test_complex/Nico/Rep1/");
        test_exist(fd,"test_complex/Nico/Rep1/jolie_perruche.txt");
        test_exist(fd,"test_complex/Nico/Rep1/grosse_perruche.txt");
        test_exist(fd,"test_complex/Nico/Rep1/belle_perruche.txt");
        test_exist(fd,"test_complex/Nico/Rep1/moche_perruche.txt");

    printf("\n");
    line();
    printf("Est ce que le chemin est un répertoire ?\n\n");
    printf("C'est un répertoire ! \n");
        test_IsDir(fd,"test_complex/");
        test_IsDir(fd,"test_complex/Nico/");
        test_IsDir(fd,"test_complex/Nico/Rep1/");
    printf("Ce n'est pas un répertoire !\n");
        test_IsDir(fd,"test_complex/Nico/Rep1/Pasjolie_perruche.txt");
        test_IsDir(fd,"test_complex/Nico/Rep2/");
        test_IsDir(fd,"test_complex2/");
    line();

    printf("Est ce que le chemin est un fichier ?\n\n");
    printf("C'est un fichier ! \n");
        test_is_file(fd,"test_complex/Nico/peruche.txt");
        test_is_file(fd,"test_complex/Nico/Rep1/belle_perruche.txt");
        test_is_file(fd,"test_complex/Nico/Rep1/grosse_perruche.txt");
    printf("Ce n'est pas un fichier !\n");
        test_is_file(fd,"test_complex/Nico/Rep1/Pasjolie_perruche.txt");
        test_is_file(fd,"test_complex/Nico/Rep1/");
        test_is_file(fd,"test_complex2/");
    line();
    printf("Que nous renvoie cette bonne fonction list ?\n\n");
    test_list(fd,"test_complex/Nico/");
    printf("\n");
    test_list(fd,"test_complex/Nico/Rep1/");
    printf("\n");
    test_list(fd,"test_complex/Nico/Rep1/belle_perruche.txt");
     printf("\n");
    line();

    printf("Que nous renvoie cette bonne fonction ReadFile avec offSet ?\n\n");
        test_Read_file(fd,"test_complex/",10,10);
        test_Read_file(fd,"test_complex/Nico/Rep1/jolie_perruche.txt",10,10);
        test_Read_file(fd,"test_complex/Nico/Rep1/grosse_perruche.txt",10,10);
        test_Read_file(fd,"test_complex/Nico/Rep1/belle_perruche.txt",10,10);
        test_Read_file(fd,"test_complex/Nico/Rep1/belle_perruche.txt",30,10);
        test_Read_file(fd,"test_complex/Nico/Rep1/belle_perruche.txt",15,10);//To read all the file
        test_Read_file(fd,"test_complex/Nico/Rep1/belle_perruche.txt",0,30);//To read all the file

    return 0;
}
