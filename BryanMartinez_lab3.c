#include <stdio.h>
#include <stdlib.h>

int main(int argc, char*argv[]){
/********************NEVER DO THIS AGAIN*********************************/

    // while(1) {
    //     int pid = fork(); 
    // }

/********************************************************************/
// open then fork, then two while loops one for parent and one for child, in there will want to read. 
    char c; 
    
    FILE * ifp = fopen(argv[1], "r");
    if ( ifp == NULL ) {
        fprintf( stderr, "File '%s' not found.\n", argv[1] ) ;
        exit(1);
    }
    pid_t pid = fork(); 
    if(pid ==0){
        //child do stuff
        int char_count = 0; 
        int my_pid = 0; 
        c = fgetc( ifp);

        while(c != EOF){
            printf("%d: %c \n",my_pid, c);
            char_count++; 
            c = fgetc( ifp);

        }
        sleep(1);
        printf("%d : Total chars: %d\n",my_pid, char_count);
        fclose(ifp); 
    }
    else{
        int char_count = 0; 
        int my_pid = getpid(); 
        c = fgetc( ifp);

        while(c != EOF){
            //throw sleep here 
            printf("%d: %c\n",my_pid, c);
            char_count++; 
            c = fgetc( ifp);

        }
        printf("%d : Total chars: %d\n",my_pid, char_count);
        fclose(ifp); 
    }
        
}