/*
 * Name: main.cpp
 * Author: Dean M. Sands, III of Team DDD
 * A Simple^H^H^H^H^H^H Shell
 * The main process creates two threads, “Locator” and “Execute”. The “Locator” thread locates files in the given path or internal reserved words entered by the user. The “Execute” thread executes internal commands entered by the user. These threads are important because child processes can't change the parent process state (Directory, environment variable).
 * The main run loop consists of printing the prompt, accepting the input, then passing it to the child process. The child process breaks apart the command line into an argument array, first by pipe redirection, then file redirection and finally by program and individual arguments. If an argument is an environment variable, it is replaced by the variables value (for example $PATH or $USER).
 * It then asks the “Locator” thread (via pipe) for the location of the actual command. The Locator thread searches the command path first, then the shell's own reserved word list, and finally returns this information (via pipe) to the child process.
 * The child process resumes its execution, checking to see if the location of the command is actually within the shell itself. If so, it sends the command (via pipe) to the Execute Thread. The Execute Thread finds the required internal command, processes it, and returns it's output (via pipe) to the child process.
 * The child process spawns processes for each piped program and can even pass output from the internal commands (for example help | grep directory).
 * When the child process exits, it returns control to the main process. The main loop cycles, returning the user to the prompt.
 * For a list of internal commands type help .
 * The shell can also determine which user is controlling it and will use a white $ for a standard user and a red hash sign for the root user.
 * To compile the DDD shell, type g++ -o dddsh *.cpp -lpthread
 * Some code was borrowed from external sources. Credit is given where credit is due.
 * Lingering bugs: When using piped commands, execution returns to prompt before child programs finish.
 */

#include "shell.h"

int main(int argc, char** argv) {
    shell::setDDDShellKey(argv[0]);
    shell::go();                        //Let the games begin.
    return 0;
}

