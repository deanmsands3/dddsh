/* 
 * File:   shell.h
 * Author: dean
 *
 * Created on March 10, 2011, 9:11 PM
 */

#ifndef SHELL_H
    #define SHELL_H
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/stat.h>
    #include <sys/wait.h>
    #include <semaphore.h>
    #include <cstdlib>
    #include <cerrno>
    #include <cstring>
    #include <cstdio>
    #include <string>
    #include <vector>
    #include <map>
    #include <cstring>
    #include <iostream>
    #include "sharedMem.h"
    #include "restart.h"
    #include "ReservedWords.h"
    #define MAXBUFFER 512
    #define FFLAG (O_WRONLY | O_CREAT | O_TRUNC)
    #define FMODE (S_IRUSR | S_IWUSR)
    #define VERSION     "0.3a"
    #define NAME        "dddsh"
    #define DESCRIPTION "Daniel, David, and Dean Shell"
    #define INTERNALCOMMANDPATH "dddsh://"
    #define QUITSTRING "q"

    #define ANSIRESET   "\e[0m"
    #define ANSI_BOLD   "\e[1m"
    #define ANSIULINE   "\e[4m"
    #define ANSIHILIT   "\e[5m"
    #define ANSIREVRS   "\e[7m"

    #define ANSIBLKFG   "\e[30m"
    #define ANSIREDFG   "\e[31m"
    #define ANSIGRNFG   "\e[32m"
    #define ANSIYLWFG   "\e[33m"
    #define ANSIBLUFG   "\e[34m"
    #define ANSIMAGFG   "\e[35m"
    #define ANSICYNFG   "\e[36m"
    #define ANSIWHTFG   "\e[37m"

    #define ANSIBLKBG   "\e[40m"
    #define ANSIREDBG   "\e[41m"
    #define ANSIGRNBG   "\e[42m"
    #define ANSIYLWBG   "\e[43m"
    #define ANSIBLUBG   "\e[44m"
    #define ANSIMAGBG   "\e[45m"
    #define ANSICYNBG   "\e[46m"
    #define ANSIWHTBG   "\e[47m"

    #define DDDSHKEYID  0x0DDD


typedef std::map<std::string, ReservedWords*> ReservedWordMapType;

class shell {
public:
    static  int makeargv(const  char*in, const char *delimiter, char ***argvp);
    static  int parseAndRedirectIn(char *command);
    static  int parseAndRedirectOut(char *command);
    static  void executeCommand(char *incmd);
    static  void executeRedirect(char *s, bool in, bool out);
    static  std::string pushDirectory(void **arguments);
    static  std::string popDirectory(void **arguments);
    static  std::string exitShell(void **arguments);
    static  std::string changeDirectory(void **arguments);
    static  std::string makeDirectory(void **arguments);
    static  std::string help(void **arguments);
    static  std::string version(void **arguments);
    static  void printPrompt();
    static  bool fileExists(const char *fileName);
    static  void initReservedWords();
    static  void init();
    shell();
    shell(const shell& orig);
    virtual ~shell();
    static void shutDown();

    static  char*       passToLocator(const char*command);
    static  char*       passToExecute(const char*command);
    static  void        setupLocatorThread();
    static  void        stopLocatorThread();
    static  void        *locatorThread(void   *arguments);
    static  void        locatorExtractPath();
    static  std::string locatorFindFile(std::string command);
    
    static  void setupExecuteThread();
    static  void stopExecuteThread();
    static  void *executeThread(void   *arguments);
    
    static  void setupEnvironThread();
    static  void stopEnvironThread();
    static  void *environThread(void   *arguments);

    static void addReservedWord(std::string myID, std::string(*myFunctor)(void **), std::string myHelp);
    static bool isDone();
    static void run();
    static void go();
    static char *createArgumentFromEnvironment(char *argument);
    static char *recombineArguments(char **arguments);
    static void setDDDShellKey(char *DDDShellKeyPath);
    
private:
    static  char*      DDDShellKey;
    static  bool        running;
    static  bool        done;
    static  std::vector <std::string> path;
    static  std::vector <std::string> directories;
    static  bool    isInternalCommand(std::string command);
    static  int     processToExecuteThreadPipe[2];
    static  int     executeThreadToProcessPipe[2];
    static  sem_t       *executeThreadSemaphore;
    static  sem_t       *executeAccessSemaphore;
    static  pthread_t   *executeThreadHandle;

    static  int     processToLocatorThreadPipe[2];
    static  int     locatorThreadToProcessPipe[2];
    static  sem_t       *locatorThreadSemaphore;
    static  sem_t       *locatorAccessSemaphore;
    static  pthread_t   *locatorThreadHandle;
                        
    static  ReservedWordMapType reserved_words;
    static  int *numberOfChildrenProcesses;
};


#endif	/* SHELL_H */

