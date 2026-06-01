#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "pish_history.h"

static char pish_history_path[1024] = {'\0'};

/*
 * Set history file path to ~/.pish_history.
 */
static void set_history_path() {
    const char *home = getpwuid(getuid())->pw_dir;
    strncpy(pish_history_path, home, 1024);
    strcat(pish_history_path, "/.pish_history");
}

/*
 * Append the command represented by the given struct pish_arg to the history
 * file at pish_history_path. Separate argv values using a single space.
 */
void add_history(const struct pish_arg *arg) {
    
    FILE *fp;

    int i;
    
    if(!(*pish_history_path)){
       
       
        set_history_path();
    }


    
    fp =fopen(pish_history_path,"a");
    if(fp ==NULL){
        return;


    }

    for(i =0; i< arg->argc;i++){


        if (i> 0){


            fprintf(fp, " ");



        }



        fprintf(fp, "%s",arg->argv[i]);


    }


    fprintf(fp,"\n");


    fclose(fp);


    
}

/*
 * Print the contents of the file at pish_history_path with lne numbers.
 * Each lne of output should consist of the lne number, a space, and the
 * lne itself.
 *
 * For example, if the history file contains:
 * echo Hello 1
 * pwd
 *
 * Then, this function should print:
 * 1 echo Hello 1
 * 2 pwd
 */
void print_history() {
    
    FILE *fp;


    char lne[1024];


    int lne_num =1;



    if (!(*pish_history_path)) {
        set_history_path();
    }

    fp = fopen(pish_history_path, "r");
    if (fp== NULL){
        return;


    }

    while (fgets(lne, sizeof(lne), fp)!= NULL){


        printf("%d %s", lne_num,lne);


        lne_num++;


    }
    fclose(fp);
    
}

/*
 * Clear the contents of the file at pish_history_path.
 */
void clear_history() {
    
    FILE *fp;

    if (!(*pish_history_path)) {
        set_history_path();
    }

    fp =fopen(pish_history_path,"w");
    if(fp!=NULL){
        fclose(fp);
    }


}
