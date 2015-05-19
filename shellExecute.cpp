/*
 * Name: shellLocatorThread.cpp
 * Authors: Dean M. Sands, III of Team DDD
 * Contains functions relating to Execute Thread in DDD shell.
 */

//#include <stdexcept>

#include "shell.h"

void shell::setupExecuteThread(){
    //Setup execute thread for internal commands
    executeAccessSemaphore=(sem_t*)SharedMem::getSharedMemorySegment(sizeof(sem_t), DDDShellKey);//Create semaphore in shared memory.
    sem_init(executeAccessSemaphore,1,1);       //Initialize as sharable, one
    executeThreadHandle=new pthread_t;
    pipe(processToExecuteThreadPipe);
    pipe(executeThreadToProcessPipe);
    pthread_create(executeThreadHandle, NULL, executeThread,NULL);
}

char *shell::passToExecute(const char*command){
    //fprintf(stderr,"passToExecute\n");
    int bytesSent, bytesRead=-1, myError;
    char buffer[MAXBUFFER];
    char *newCommand;
    sem_wait(executeAccessSemaphore);       //I need you tonight.
    //fprintf(stderr,"Access Semaphore received.\n");
    bytesSent=r_write(processToExecuteThreadPipe[1], (void*)command, (std::strlen(command)+1));
    bytesRead=r_read(executeThreadToProcessPipe[0], (void*)buffer, MAXBUFFER);
    //fprintf(stderr,"pass2Execute Thread!\n");
    //fprintf(stderr,"Bytes read: %d!\n", bytesRead);
    //fprintf(stderr,"Buffer contents: %s\n", buffer);
    //fprintf(stderr,"Bytes read: %d!\n", bytesRead);
    sem_post(executeAccessSemaphore);       //Let's go our separate ways.
    //fprintf(stderr,"Access Semaphore returned.\n");
    newCommand=new char[std::strlen(buffer)+1];
    std::strcpy(newCommand,buffer);
    return newCommand;
}



void *shell::executeThread(void   *arguments){
    int bytesRead,bytesSent=-1, myError;
    char buffer[MAXBUFFER];
    char **command;
    
    std::string commandString, returnValue;
    while(running){
        std::memset(buffer,0,MAXBUFFER);
        bytesRead=r_read(processToExecuteThreadPipe[0], buffer, MAXBUFFER);
        //fprintf(stderr,"Execute Thread!\n");
        //fprintf(stderr,"Bytes read: %d!\n", bytesRead);
        //fprintf(stderr,"Buffer contents: %s\n", buffer);
        buffer[bytesRead-1]=0;  //Cap it with a null, just to make sure
        shell::makeargv(buffer, " \t", &command);
        commandString=command[0];
        if(!isInternalCommand(commandString)){
            std::strcpy(buffer, "Error.\n");
        }else{
            returnValue=reserved_words[commandString]->GetFunctor()((void**)command);
            strcpy(buffer, returnValue.c_str());
            //fprintf(stderr, "Command executed. Returned %s", returnValue.c_str());
        }
        commandString.clear();
        returnValue.clear();
        //fprintf(stderr,"Buffer contents: %s\n", buffer);
        bytesSent=r_write(executeThreadToProcessPipe[1], buffer, MAXBUFFER);
        //fprintf(stderr,"Bytes wrote: %d!\n", bytesSent);
        //fprintf(stderr,"Buffer contents: %s\n", buffer);
        //fprintf(stderr,"Bytes Sent: %d\n", bytesSent);
    }
    return NULL;
}

void shell::stopExecuteThread(){
    r_write(processToExecuteThreadPipe[1], (void*)"exit", MAXBUFFER);
    pthread_join( *executeThreadHandle, NULL);
    sem_destroy(executeAccessSemaphore); 
}
bool    shell::isInternalCommand(std::string command){
    if(reserved_words[command]==NULL)
        return false;
    else
    return      true;
}

int         shell::executeThreadToProcessPipe[2];
int         shell::processToExecuteThreadPipe[2];
sem_t       *shell::executeAccessSemaphore;
pthread_t   *shell::executeThreadHandle;

