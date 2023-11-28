# Computer-Systems

NAME: BRYAN MARTINEZ	

• CSC-251 - Lab 1

DESCRIPTION : Writing a simple shell program !! First, a command line is read from the terminal. This is parsed to build argv. Then we fork to create a child process and afterward, we use execvp() system call to execute the file that contains the command. This specific exec call will search the PATH directory paths automatically. The shell will interpret the "&" as a command terminator and a command terminated with & executes concurrently with the shell. An appropriate error will be printed if a command is given and execvp() cannot execute, after which the shell will prompt the user again. Finally, the shell will terminate once ^C signal is used. 

• Compile & execution instructions
To compile : cc BryanMartinez_lab1.c -o a.out
To execute : ./a.out



• CSC-251 - Lab 2

DESCRIPTION: Adding pipes to a simple shell program !! First, a command line is read from the terminal. It creates pipes from int fd[2] and fd_2[2]. First, we determine how many pipes are inputted, then we create the pipes, this way the child processes inherit the file descriptor table. If there is 1 pipe, we fork a process, redirect, then exit. Then we fork a second process for the second command. For two pipes, we create 2 pipes, we fork, redirect to the first pipe then execute. Then we fork a second time, redirect to the second pipe, then execute the second command. Finally, we fork a third to execute the third command. 

• Compile & execution instructions
To compile : cc -o a.out BryanMartinez_lab2.c
To execute : ./a.out


• CSC-251 - Lab 3

DESCRIPTION : Concurrency ! We are opening a file, and then creating a child process so that both the parent and the child read from the file at the same time. Then they will both print out the character preceded by the pid, and when the program exits, print out the number of characters read out by both the parent and the child process. 

• Compile & execution instructions
To compile : cc BryanMartinez_lab3.c -o a.out
To execute : ./a.out

• CSC-251 - Lab 4
DESCRIPTION :  

• Compile & execution instructions
To compile : cc BryanMartinez_lab4.c -o a.out
To execute : ./a.out



