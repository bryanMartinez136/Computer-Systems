#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <linux/limits.h>

/* ----------TASK-----------------
* Writing a simple shell program with pipes :O  !!
*
* 1. needs to be able to read at least 2 pipes
* 2. optionally can have a history 
* 3. optionally can use the up arrows to traverse the history
*
*/

void sig(int signum){
    printf("\nOk byebye\n"); 
    exit(0); 

}

int main(){
    //---------------declare variables, will need a buffer for input, argv, argc
    char buffer[60]; // inititialize everything

    pid_t pid, pid2, pid3; 
    int argc = 0;
    char *argv[40]; 
    char *token; 
    int val = 0;
    int pipeNum = 0; 
    int loc_of_pipes[2]; 


    FILE *ifp_write= fopen(".myhistory","w"); // empties out the .myhistory file

    int count = 0; 


    if(ifp_write == NULL){
    perror("Something went wrong opening the file"); 
    exit(1); 
    }

    //-------Pipes---------------------------------------------------
    int f_des[2], f_des_2[2]; 
    if (pipe(f_des) == -1) { // error; could not pipe :^(
    perror("Pipe");
    return 1;
    }

    while(1){
        signal(SIGINT, sig);  
        argc = 0;
        val = 0; 
        pipeNum = 0; 
        //--read in input from user --------------------------------------------
        printf("csuser@address:~/Lab2");
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

//---------------- parse; -----------------------------------------------------------------------
        token= strtok(buffer, " \n");
        while( token != NULL ) {
            argv[argc++] = token; 
            token = strtok(NULL, " \n");
        }
        argv[argc] = NULL;

//-----------------move the suff to the history file---------------------------------
        FILE *ifp_append = fopen(".myhistory","a");
        if(ifp_append == NULL){
            perror("Something went wrong opening the file"); 
            exit(1); 
        }

        fprintf(ifp_append, "%d: ", count++); 

        for(int i = 0; i < argc; i++){
            fprintf(ifp_append, "%s ", argv[i]);	
            if(strncmp(argv[i], "|",1)==0){ // check for the number of pipes
                loc_of_pipes[pipeNum++] = i; 
                argv[i] = NULL; 
            }
        }
        fprintf(ifp_append, "\n"); 
        fclose(ifp_append); 

        switch(pipeNum){
//--------------------Executing the 0 pipe command-----------------------------
            case 0:
                // check if the command is history
                if(strncmp(argv[0], "history",7)==0){
                    // print out the history
                    
                    FILE *file = fopen(".myhistory", "r");
                    
                    if (file == NULL) {
                        perror("Error opening file");
                        return 1;
                    }
                    char c;
                    while ((c = fgetc(file)) != EOF) {
                        putchar(c);
                    }
                    fclose(file);
                    
                    break; 
                }
                if(strncmp(argv[0], "erase",5)==0 && strncmp(argv[1], "history",7)==0){
                    FILE *ifp_erase= fopen(".myhistory","w"); // empties out the .myhistory file
                    fclose(ifp_erase); 
                    count = 0; 
                    break;
                }    
                // just run stuff normally, no pipes !
                pid = fork(); 
                if(pid < 0){
                    perror("Child is wrong"); 
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
                break; 
            
//--------------------Executing the 1 pipe command-----------------------------

            case 1:
                // pipe 1
                pid = fork(); 
                if(pid < 0) {
                    perror("Fork");
                    return 2;
                }
                if(pid == 0){
                    close(1);                         // In the child
                    dup2( f_des[1], 1); // stdout is a copy of f_de[WRITE]
                    close(f_des[0]);
                    close(f_des[1]);
                    execvp(argv[0], &argv[0]);

                    perror("exec failure ");

                    exit(0);

                } 
                else{
                    //waitpid(pid, NULL, 0); 
                }
                pid2 = fork(); 
                if(pid2 == 0){
                    close(0);                      // In the parent
                    dup2( f_des[0], 0);   // stdin is a copy of f_de[READ]
                    close(f_des[0]);
                    close(f_des[1]);
                    execvp(argv[loc_of_pipes[0]+1], &argv[loc_of_pipes[0]+1]);
                    perror("exec failure ");
                    exit(0); 
                }

                close(f_des[0]); 
                close(f_des[1]); 

                if(val == 0){
                    waitpid(pid, NULL, 0); 
                    waitpid(pid2, NULL, 0); 
                }
                

                break; 
            
//--------------------Executing the 2 pipe commands-----------------------------
            case 2:
                //2 pipes ! create the pipes-------------------------
                
                if (pipe(f_des) == -1) { // error; could not pipe :^(
                perror("Pipe");
                return 1;
                }
                if (pipe(f_des_2) == -1) { // error; could not pipe :^(
                perror("Pipe");
                return 1;
                }
                // ----------------------now we fork for the first process
                pid = fork(); 
                
                // ---------------------check errors--------------------------------------------
                if(pid < 0 ) {
                    perror("Fork");
                    return 2;
                }
                // ===================== in the first child process==========================
                if(pid == 0){
                    //close std out and open the write portion of the pipe 
                    close(1);                         
                    dup2( f_des[1], 1); // redirect to the write portion of the pipe
                    // gotta close it all --------------------------------------
                    close(f_des[0]);
                    close(f_des[1]);
                    close(f_des_2[0]);
                    close(f_des_2[1]);
                    // exec the first command -------------------------------
                    execvp(argv[0], &argv[0]);

                    perror("exec failure ");
                    exit(0);

                }
                // --------------------------- in the second child process-----------------------
                pid2 = fork(); 
                if(pid2 < 0){
                    perror("Fork");
                    return 2;
                }
                if(pid2 == 0){

                    close(0);                
                    close(1);        
                    dup2( f_des[0], 0);   // want to close stdin and get input from the first pipe    
                    dup2( f_des_2[1], 1); // want to redirect out put to the second pipe
                    // gotta close it all ------------------------------------------------------
                    close(f_des[0]); 
                    close(f_des[1]);
                    close(f_des_2[0]);
                    close(f_des_2[1]);
                    // -----------now we exec the second set of commands---------------------------
                    execvp(argv[loc_of_pipes[0]+1], &argv[loc_of_pipes[0]+1]);
                    perror("exec failure ");
                    exit(0); 
                }
                pid3 = fork(); 
                if(pid3 == 0){ // in the parent process---------------                  
                    // we close the std in and open the read portion of the second pipe 
                    close(0);                     
                    dup2( f_des_2[0], 0);   // open read portion of the second pipe
                    // gotta close it all --------------------------------------
                    close(f_des[0]);
                    close(f_des[1]);
                    close(f_des_2[0]);
                    close(f_des_2[1]);
                    // -----------now we exec the last set of commands---------------------------
                    execvp(argv[loc_of_pipes[1]+1], &argv[loc_of_pipes[1]+1]);
                    perror("exec failure ");
                }

                close(f_des[0]); 
                close(f_des[1]); 
                close(f_des_2[0]); 
                close(f_des_2[1]); 
                if(val == 0){
                    waitpid(pid, NULL, 0); 
                    waitpid(pid2, NULL, 0); 
                    waitpid(pid3, NULL, 0); 
                }

                break;

        }
        
    }

    fclose(ifp_write); 
    exit(0);         

}   


