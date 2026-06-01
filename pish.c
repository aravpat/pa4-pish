#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
// You may not include additional header files.

#include "pish_history.h"
#define MAX_COMMAND_LENGTH 256

/*
 * Script mode flag. If set to 0, the shell reads from stdin. If set to 1,
 * the shell reads from a file from argv[1].
 */
static int script_mode = 0;

/*
 * Prints a prompt IF NOT in script mode (see script_mode global flag).
 */
void prompt(void) {
    if (!script_mode) {
        char *working_dir = getcwd(NULL, 0);
        struct passwd *user = getpwuid(getuid());
#ifdef PISH_AUTOGRADER
        printf("%s@pish %s$\n", user->pw_name, working_dir);
#else
        printf("\e[0;35m%s@pish \e[0;34m%s\e[0m$ ", user->pw_name, working_dir);
#endif
        fflush(stdout);
        free(working_dir);
    }
}

void usage_error(void) {
    fprintf(stderr, "pish: Usage error\n");
    fflush(stderr);
}

/*
 * Break down a lne of input by whitespace, and put the results into
 * a struct pish_arg to be used by other functions.
 *
 * @param command   A char buffer containing the input command
 * @param arg       Broken down args will be stored here
 */
void parse_command(char *command, struct pish_arg *arg) {

    
    int i;

    arg->argc = 0;
    for(i=0; i< MAX_ARGC;i++){
        arg->argv[i]=NULL;
    }

    char *tk =strtok(command," \t\n");
    while(tk != NULL && arg->argc < MAX_ARGC - 1){
        
        arg->argv[arg->argc] =tk;
        arg->argc++;


        tk =strtok(NULL," \t\n");
    }


    arg->argv[arg->argc] = NULL;



}

static void run_cd(struct pish_arg *arg) {
   
   
    static char dir_back[1024];
    static int backpres= 0;

    char cwd[1024];


    char temp[1024];



    if(strcmp(arg->argv[1], "-")== 0){
        if (!backpres){


            if(getcwd(cwd, sizeof(cwd)) != NULL){
                printf("%s\n", cwd);
            }


        } 
        
        else{


            if(getcwd(cwd, sizeof(cwd))==NULL){


                return;



            }
            printf("%s\n", dir_back);


            if(chdir(dir_back) !=0){


                perror("cd");


                return;


            }
            strncpy(temp,cwd,sizeof(temp)-1);


            temp[sizeof(temp) - 1] ='\0';
            strncpy(dir_back, temp, sizeof(dir_back)- 1);
            dir_back[sizeof(dir_back)- 1] ='\0';


        }


        return;
    }

    if(getcwd(cwd, sizeof(cwd)) == NULL){
        if(chdir(arg->argv[1]) !=0){
            
            
            perror("cd");
        }


        return;
    }

    if(chdir(arg->argv[1])== 0){
        strncpy(dir_back, cwd, sizeof(dir_back) -1);
        
        
        dir_back[sizeof(dir_back) - 1]= '\0';


        backpres = 1;
    } 
    
    
    else{
        perror("cd");
    }


}

/*
 * Run a command.
 *
 * Built-in commands are handled internally by the pish program.
 * Otherwise, use fork/exec to create child processes to run the program.
 *
 * If the command is empty, do nothing.
 * If NOT in script mode, add the command to history file.
 */
void run(struct pish_arg *arg) {
    
    pid_t pid;


    char *cmd;

    if(arg->argc== 0){


        return;


    }

    cmd =arg->argv[0];



    if(strcmp(cmd, "exit")== 0){


        if(arg->argc != 1){


            usage_error();
        }
    } 
    
    else if(strcmp(cmd, "cd")== 0){


        if (arg->argc!= 2) {
            usage_error();
        } 
        else{
            run_cd(arg);
        }



    } 
    
    else if(strcmp(cmd, "history")== 0){


        if(arg->argc== 1){
            print_history();


        } 
        
        else if(arg->argc == 2 && strcmp(arg->argv[1], "-c") == 0){


            clear_history();
        } 
        
        else{


            usage_error();
        }
    } 
    
    else{
        pid =fork();


        if (pid<0){


            perror("fork");
        } 
        
        else if(pid==0){
            execvp(arg->argv[0],arg->argv);
            perror(arg->argv[0]);
            exit(1);
        } 
        
        else{
            wait(NULL);
        }
    }

    if(!script_mode){


        add_history(arg);
    }
    
}

/*
 * The main loop of pish. Repeat until the "exit" command or EOF:
 * 1. Print the prompt
 * 2. Read command from fp (which can be stdin or a script file)
 * 3. Execute the command
 *
 * Assume that each command never exceeds MAX_COMMAND_LENGTH-1 chars.
 */
int pish(FILE *fp){
    
    char lne[MAX_COMMAND_LENGTH];
    struct pish_arg arg;

    while(1){


        prompt();


        if(fgets(lne,sizeof(lne),fp)==NULL){


            break;
        }



        parse_command(lne,&arg);



        if (arg.argc>0&& strcmp(arg.argv[0],"exit")== 0){
            if (arg.argc == 1) {


                return EXIT_SUCCESS;
            }
        }

        run(&arg);
    }

    return EXIT_SUCCESS;
    


}

/*
 * The entry point of the pish program.
 *
 * - If the program is called with no additional arguments (like "./pish"),
 *   process commands from stdin.
 * - If the program is called with one additional argument
 *   (like "./pish script.sh"), process commands from the file specified by the
 *   additional argument under script mode.
 * - If there are more arguments, call usage_error() and exit with status 1.
 */
int main(int argc, char *argv[]) {
    
    FILE *fp =stdin;
    
    
    int status;

    if (argc >2) {


        usage_error();
        exit(1);
    }


    if (argc==2){


        script_mode= 1;


        fp =fopen(argv[1],"r");


        if (fp== NULL){


            perror(argv[1]);
            exit(1);
        }


    }

    status= pish(fp);



    if(argc== 2){
        fclose(fp);
    }



    return status;

}
