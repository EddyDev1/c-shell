# Major 2
Shell Creation Project
README

Major 2 Group 4 Members -

Sabin Sebastian, Benjamin Murley, Edward Asante, and Chiaha Bernice

**Roles designated at the start of the project**

Edward - The path function, signal/terminal and shell

Bernice - The change directory, I/O redirection, and shell

Ben - The myhistory, alias, and shell

Sabin - The exit, pipelining, and shell

**Who actually did what in the end**

Edward - path function, majority of redirection and shell

Ben - myhistory, alias, majority of: redirection, pipelining, and shell

Bernice - cd and minimal shell

Sabin - exit

**contact Ben or Edward to see the full timeline/history of major2.c**


This program is able to be compiled with and run with a Makefile. The program consists of major2.c (main) and many .c source files containing their respective functions. There are two ways to run the prgoram, the batch or interactive mode. Batch mode takes a script file and executes the contents of the file. The interactive mode uses user input and executes their input. The program will run until the user decides to quit (either with the exit builtin, typing quit, or ctrl-c and ctrl-z). 

The program supports all of the collaborative portion. The program also supports path, cd, pipes, and redirection but currently the alias, myhistory, signal handling, and exit are not working properly.  To run the program, download the source code, type "make" in bash in the directory with the c/header files and the makefile, and then type ./major2 or ./major2 yourbatchfilehere to run the program.


The builtin functions implemented in the program:

1)Change directory

    if only cd is called the  argument in the function defaults to NULL cd(NULL) and changes the directory to the home directory
    if cd {Newdirectory} is called, the function is called as cd(Newdirectory)
    if cd .. is called, the function is called as cd("..") and changes the directory to the parent directory

2)Exit

    not working properly / limited performance
    although it is not implemented there is still code for it -> check pipelining.c 
    
3)Path

    "path + XXX" -> (tells the progam the user wants to add a path (XXX) to the path list) -> calls the addPath function
    which has three parameters (the path list, the path to add, and number of current paths)
    "path - XXX" -> (tells the progam the user wants to remove a path (XXX) from the path list) -> calls the removePath function
    which has three parameters (the path list, the path to add, and number of current paths)
    "path" -> (tells the program the user wants to display the pathlist) -> calls the showPath function which has three parameters
    (the path list and the number of paths)

    the three functions work as intended and is found in path.c

4)Myhistory

    not working properly / limited performance 
    although it is not implemented there is still code for it -> check myhistory.c

5)Alias

    not working properly / limited performance
    although it is not implemented there is still code for it -> check alias.c 

