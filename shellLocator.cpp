/*
 * Name: shellLocatorThread.cpp
 * Authors: Dean M. Sands, III of Team DDD
 * Contains functions relating to Locator Thread in DDD shell.
 */

#include "shell.h"

void shell::setupLocatorThread(){

    //Setup locator thread for internal commands
    locatorAccessSemaphore=(sem_t*)SharedMem::getSharedMemorySegment(sizeof(sem_t), DDDShellKey);//Create semaphore in shared memory.
    sem_init(locatorAccessSemaphore,1,1);
    locatorThreadHandle=new pthread_t;
    pipe(processToLocatorThreadPipe);
    pipe(locatorThreadToProcessPipe);
    pthread_create(locatorThreadHandle, NULL, locatorThread,NULL);
    //fprintf(stderr,"Locator Thread Initialized.\n");
}
char *shell::passToLocator(const char*command){
    //fprintf(stderr,"passToLocator\n");
    int bytesSent, bytesRead=-1, myError;
    char buffer[MAXBUFFER];
    char *newCommand;
    sem_wait(locatorAccessSemaphore);       //I need you tonight.
    //fprintf(stderr,"Access Semaphore received.\n");
    bytesSent=r_write(processToLocatorThreadPipe[1], (void*)command, (std::strlen(command)+1));
    bytesRead=r_read(locatorThreadToProcessPipe[0], (void*)buffer, MAXBUFFER);
    //fprintf(stderr,"Bytes read: %d!\n", bytesRead);
    sem_post(locatorAccessSemaphore);       //Let's go our separate ways.
    //fprintf(stderr,"Access Semaphore returned.\n");
    newCommand=new char[std::strlen(buffer)+1];
    std::strcpy(newCommand,buffer);
    return newCommand;
}

void *shell::locatorThread(void   *arguments){
    int bytesRead,bytesSent=-1, myError;
    char buffer[MAXBUFFER];
    std::string fullCommandName;
    while(running){
        bytesRead=r_read(processToLocatorThreadPipe[0], buffer, MAXBUFFER);
        //fprintf(stderr,"Bytes read: %d!\n", bytesRead);
        //fprintf(stderr,"Buffer contents: %s\n", buffer);
        buffer[bytesRead-1]=0;  //Cap it with a null, just to make sure
        fullCommandName=locatorFindFile(buffer);
        //bytesRead=r_write(locatorThreadToProcessPipe[0], (void*)fullCommandName.c_str(), (fullCommandName.size()+1));
        //fprintf(stderr,"File Location: %s\n", fullCommandName.c_str());
        std::strcpy(buffer, fullCommandName.c_str());
        //fprintf(stderr,"Buffer contents: %s\n", buffer);
        bytesSent=r_write(locatorThreadToProcessPipe[1], buffer, std::strlen(buffer)+1);
        //fprintf(stderr,"Bytes Sent: %d\n", bytesSent);
        fullCommandName.clear();    
    }
    return NULL;
}

void shell::locatorExtractPath(){
    int pathIndex=0;
    std::string fullPath;
    for(int i=0;i<path.size();i++){
        path[i].clear();
    }
    path.clear();
    fullPath=getenv("PATH");
    path.push_back(std::string(""));
    for(int i=0;i<fullPath.length();i++){
        if(fullPath[i]==':'){
            pathIndex++;
            path.push_back(std::string(""));
        }
        else{
                path[pathIndex].push_back(fullPath[i]);
        }
    }
}

std::string shell::locatorFindFile(std::string command){
    std::string      tempLocation;
    locatorExtractPath();
    for(int i=0;i<path.size();i++){
        tempLocation=path[i]+"/";
        tempLocation+=command;
        //fprintf(stderr, "Checking location: %s\n",tempLocation.c_str());
        if(fileExists(tempLocation.c_str())){
                //fprintf(stderr, "Found!\n");
            return path[i];
        }
    }
    if(shell::isInternalCommand(command)){
        //fprintf(stderr, "Internal Command!\n");
        return std::string(INTERNALCOMMANDPATH);
    }
    return std::string("");
}


void shell::stopLocatorThread(){
    r_write(processToLocatorThreadPipe[1], (void*)"exit", MAXBUFFER);
    pthread_join( *locatorThreadHandle, NULL);
    if(close(processToLocatorThreadPipe[0])||close(processToLocatorThreadPipe[1]))
        perror("Failed to close p2L pipe");
    if(close(locatorThreadToProcessPipe[0])||close(locatorThreadToProcessPipe[1]))
        perror("Failed to close l2P pipe");
    sem_destroy(locatorAccessSemaphore); 
}

int     shell::locatorThreadToProcessPipe[2];
int     shell::processToLocatorThreadPipe[2];
sem_t       *shell::locatorAccessSemaphore;
pthread_t   *shell::locatorThreadHandle;
