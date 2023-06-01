#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

/* ----------TASK-----------------
 * Writing a simple shell program :> !!
 * 1. parse a command line of the form:
 * 	command [identifier[idnetifier]]
 * This is parsed to build argv
 *
 * 2. Use execvp() syscall to execute the file that contains the command
 * this specific exec call will search the PATH direcotry paths automatically
 * 3. shell must interpret the & as a command terminator.A command terminated with & should be executed concurrently with the shell
 *
 * 4. Print appropiate errors if a commnad is given and execvp() cannot execute. Then the shell should prompt the user again
 * 5. Should terminate once ^C signal is used (class information)
 */

 

int main(){
    // declare variables, will need a buffer for input, argv, argc
    char buffer[20]; // where the 

    pid_t pid; 
    int argc = 0;
    char *argv[20]; 
    char *token; 
    int val = 0; 

    while(1){
        argc = 0;
        val = 0; 
        // read in input from user --------------------------------------------
        printf("csuser@address:~/Lab1");
        printf("$ "); 
        gets(buffer); 
        if(strncmp(buffer, "exit",4)==0){
            break;
        }
        fflush(stdin); 
        if(buffer[strlen(buffer)-1]=='&'){
            val = 1;
            buffer[strlen(buffer)-1]='\0';
            //buffer[strlen(buffer)-1]='\0';
            // overwrite
        }
        //also check for /n using the notation above to overwrite to null terminator also check buffer -1

//---------------- parse; ----------------------------------
        token= strtok(buffer, " \n");
        while( token != NULL ) {
            argv[argc++] = token; 
            token = strtok(NULL, " \n");
        }
        argv[argc] = NULL; 
       
         pid = fork(); 
         if(pid < 0){
          perror("error creating process"); 
          exit(1); 
         }
         if(pid == 0){ // then its the child and we exec
            execvp(argv[0], argv);
            perror("exec failure ");
            exit(0);// have to exit here or else child will come back as a shell 
         }
         else{ 
            //note from TA: use a boolean (integer) to check for & (if you found an & at the top set the int to 1 )
            if(val == 0){
                waitpid(pid, NULL, 0);  
            }
     }
}
    exit(0); 

}   
