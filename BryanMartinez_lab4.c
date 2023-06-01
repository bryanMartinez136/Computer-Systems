#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <linux/limits.h>
#include <signal.h>
#include <fcntl.h>

#include "ezipc.h"

//your program should read in the "card" text from a file called lab4.txt
//- each card is 20 characters long (not 80 column cards).
//- to simulate card input read from a file where each line has 20 characters.
//- printer process: prints 25-character lines to the screen (not 125 character lines).
//- you must use the read and write system calls

int main() {

//---initialization-------------------------------------

	char c; 
	char buffer[26]; 
	pid_t pid, pid2, pid3; 
	int r, ifp, index=0; 	
	int empty, mutex, full, empty2, mutex2, full2; 
	char *SM, *SM2;
	

	SETUP();
	SM = SHARED_MEMORY(1); 
	SM2 = SHARED_MEMORY(1); 

	empty = SEMAPHORE(SEM_BIN, 1); // number of empty buffers
	mutex = SEMAPHORE(SEM_BIN, 1); // mutual exclusion
	full = SEMAPHORE(SEM_BIN, 0); // number of full buffers
	
	empty2 = SEMAPHORE(SEM_BIN, 1); // number of empty buffers
	mutex2 = SEMAPHORE(SEM_BIN, 1); // mutual exclusion
	full2 = SEMAPHORE(SEM_BIN, 0); // number of full buffers

	ifp = open("lab4.txt", O_RDONLY, 0);

//------forking processes here - Producer--------------------------------

	pid = fork(); 
	if(pid < 0){
		perror("p1 bad"); 
		exit(10); 
	}

	if(pid == 0){ // producer
		char *eol = "<EOL>";
		char ch = '!';
		int count = 0;

		while(read(ifp, &c, 1)>0){
			
			P(empty);
			P(mutex);
			*SM = c; 
			V(mutex); 
			V(full); 

			
			count++;
			if(c == '\n'){count--; }

			if(count == 20){
				for(int i = 0; i < 5; i++){
					P(empty);
					P(mutex);
					*SM = eol[i]; 
					V(mutex); 
					V(full);
				}
				count = 0; 
			}
			
		}

		exit(0); 

	}

//-----Squasher------------------------

	pid2 = fork(); 
	if(pid2 < 0){
		perror("Error in squash"); 
		exit(10); 
	}
	if(pid2 == 0){
		// sqaush
		char prev_ch = '\0';
		char current_ch;
		while(1){
			P(full);
			P(mutex);
			// charaacter goes into buffer
			current_ch = *SM;

			V(mutex); 
			V(empty); 

			
			/*produce and send to the printer*/

			if(current_ch != '*' && prev_ch == '*'){					
				P(empty2); 
				P(mutex2); 
				*SM2 = prev_ch;  
				V(mutex2); 
				V(full2); 

				P(empty2); 
				P(mutex2); 
				*SM2 = current_ch;  
				V(mutex2); 
				V(full2); 
				prev_ch = current_ch; 
			}
			else if(current_ch == '\n'){
				//do nothing
			}
			else if(current_ch != '*'){

				P(empty2); 
				P(mutex2); 
				*SM2 = current_ch;  
				V(mutex2); 
				V(full2); 

			}
			else{
				if(prev_ch == '*'){

					P(empty2); 
					P(mutex2); 
					*SM2 = '#';  
					V(mutex2); 
					V(full2); 

					prev_ch = '#'; 
				}
				else{
					prev_ch = '*'; 
					// don't want to send the asterisk yet
				}

			}
			// *SM2 = current_ch;
			/*
			1234*6#9*ABCDEFGHI*<EOL>*
			bcdefghij#bcdefghi<EOL>##
			*XXXXXabcdefghij<EOL>####
			######<EOL>##########<EOL
			>ABCDEFGHIJKLMNOPQRST<EOL
			># 					Z<EOL>
			*/
			
		
		}
		exit(0); 
	}

//-------printer------------------------------------

	pid3 = fork(); 
	if(pid3 < 0){
		perror("Printer error"); 
		exit(10); 
	}
	if(pid3 == 0){
		// printer
		char p_char;
		char buffer[26];  
		int count = 0; 
		while(1){
			P(full2);
			P(mutex2); 
			//consume
			p_char = *SM2; 
			
			V(mutex2); 
			V(empty2); 

			printf("%c", p_char);
			count++; 
			if(count == 25){
 
				printf("\n");
				count = 0; 
			}
			// buffer[count++] = p_char; 

		}
		exit(0);
	}

	waitpid(pid, NULL, 0);
	waitpid(pid2, NULL, 0);
	waitpid(pid3, NULL, 0);

	exit(0); 

}
