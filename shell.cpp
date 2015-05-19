/*
 * Name: shell.cpp
 * Authors: Dean M. Sands, III of Team DDD
 * Contains shell static class (should just call it a namespace)
 */

#include "shell.h"
char *getLastToken(char *buffer, char delimiter){
    char *returnPointer,*walkingPointer;
    returnPointer=buffer;
    walkingPointer=buffer;
    while(*walkingPointer!=0){
        if(walkingPointer[0]==delimiter&&walkingPointer[1]!=0)
            returnPointer=walkingPointer;
        walkingPointer++;
    }
    if(returnPointer[1]!=0)
        returnPointer++;
    return returnPointer;
}

shell::shell() {
}

shell::shell(const shell& that) {
}

shell::~shell() {
}

char *shell::recombineArguments(char **arguments){
    char*complete;
    std::string temp;
    for(int i=0;arguments[i]!=NULL;i++){
        if(i!=0){
            temp+=" ";
        }
        temp+=arguments[i];
    }
    complete=new char[temp.size()+1];
    std::strcpy(complete, temp.c_str());
    temp.clear();
    return complete;
}

char *shell::createArgumentFromEnvironment(char *argument){
    char *temp, *temp2;
    char buffer[MAXBUFFER];
    if(argument[0]=='$'){
        temp=getenv(argument+1);
        if(temp!=NULL){
            temp2=new char[strlen(temp)+1];
            strcpy(temp2, temp);
        }else{
            temp2=new char[2];
            temp2[0]=' ';
            temp2[1]=0;
        }
        return  temp2;
    }
    else
        return argument;
}

void    shell::printPrompt(){
    char *hostName=new char[24];
    char *currentWorkingDirectory=new char[2048];
    gethostname(hostName, 24);
    getcwd(currentWorkingDirectory, 2048);
    std::cout
            <<ANSIRESET<<"["            //Opening [ in default color
            <<ANSI_BOLD                 //Bold Text
            <<ANSIBLUFG<<getenv("USER") //Print User in Blue
            <<ANSICYNFG<<"@"            //Print @ in Cyan
            <<ANSIGRNFG<<hostName       //Print Hostname in Green
            <<ANSIYLWFG<<":"            //Print : in Yellow
            <<ANSIMAGFG<<getLastToken(currentWorkingDirectory,'/')      //Print current Directory in Magenta
            <<ANSIRESET<<"]";           //Print closing ] in default color
    if(strcmp(getenv("USER"), "root")==0)
        std::cout<<ANSIREDFG<<"#";      //Print hash mark for root in dark red (very awesome)
    else
        std::cout<<ANSI_BOLD<<ANSIWHTFG<<"$";      //Print dollar sign for non-root in bright White
    
        std::cout<<ANSIRESET<<" ";      //Print a friend space            
    delete currentWorkingDirectory;
    delete hostName;
}

bool shell::fileExists (const char * fileName){
        
        return (access ( fileName, F_OK )==0);  //If access returns 0, we're good!
}


bool shell::isDone() {
    return done;
}

void shell::init(){
    std::cout<<version(NULL);
    setupLocatorThread();
    setupExecuteThread();
    numberOfChildrenProcesses=(int*)SharedMem::getSharedMemorySegment(sizeof(int), DDDShellKey);
    running=true;
    done=false;
}

void shell::shutDown(){
    running=false;
    stopLocatorThread();
    stopExecuteThread();
}

void    shell::run(){
    char inputBuffer[MAXBUFFER];
    int length;
    pid_t childpid;
    while(!shell::isDone()){
        shell::printPrompt();
        std::cin.getline(inputBuffer,MAXBUFFER,'\n');
        length=strlen(inputBuffer);   
        if((childpid=fork())==-1){
            perror("Failed to fork child");
        }
        else if(childpid==0){
            shell::executeCommand(inputBuffer);
            exit(1);
        }
        else{
            wait(NULL);                                 //Still doesn't work right.
            for(int i=1;i<numberOfChildrenProcesses[0];i++){
                    wait(NULL);
            }
        }
    
    }
}

void shell::setDDDShellKey(char* DDDShellKeyPath) {
    DDDShellKey = new char[std::strlen(DDDShellKeyPath)+1];
    std::strcpy(DDDShellKey, DDDShellKeyPath);
}
void    shell::go(){
    shell::init();
    shell::initReservedWords();
    shell::run();
    shell::shutDown();
}
std::vector <std::string> shell::path;
std::vector <std::string> shell::directories;
bool  shell::running;
bool  shell::done;
char*      shell::DDDShellKey;
std::map<std::string, ReservedWords*> shell::reserved_words;
int *shell::numberOfChildrenProcesses;
