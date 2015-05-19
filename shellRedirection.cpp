/*
 * Name: shellRedirection.cpp
 * Authors: Ray A. Robbins, Steven Robbins
 * Retrieved from Unix Systems Programming, 10th Ed.
 * Contains modifications by Dean M. Sands, III of Team DDD
 * 
 */


#include "shell.h"

int shell::makeargv(const  char*inString, const char *delimiters, char ***argvp){
    int error;
    int i;
    int  numTokens;
    const   char    *newString;
    char    *tempString;
    if((inString==NULL) || (delimiters==NULL) ||(argvp==NULL)){
        errno=EINVAL;
        return  -1;
    }
    
    argvp[0]=NULL;
    newString=inString+std::strspn(inString,delimiters);        //Eliminate leading whitespace/delimiters
    
    if(
            (tempString=new char[strlen(newString)+1])          //Allocate a new copy of the arguments
            ==NULL
            )
        return  -1;
    
    std::strcpy(tempString,newString);                          //Copy the arguments into tempString
    
    numTokens=0;
    
    if(std::strtok(tempString,delimiters)!=NULL)
        for(numTokens=1;strtok(NULL,delimiters)!=NULL;numTokens++);     //Count the number of tokens
        
    if(
            (argvp[0]=new char*[numTokens+1])                           //Allocate an array of pointers for each argument
            ==NULL
            ){
        error=errno;
        delete tempString;
        errno=error;
        return  -1;
    }
    
    if(numTokens==0){                                                   //No arguments? Go home.
        delete tempString;
    }
    else{
        std::strcpy(tempString,newString);
        argvp[0][0]=createArgumentFromEnvironment(std::strtok(tempString,delimiters));
        for(i=1;i<numTokens;i++)
            argvp[0][i]=createArgumentFromEnvironment(std::strtok(NULL, delimiters));
    }
    
    argvp[0][numTokens]=NULL;
    
    return numTokens;
}


int shell::parseAndRedirectIn(char *command){
    int error;
    int inputFileDescriptor;
    char    *inputFile;
    
    if((inputFile=std::strchr(command, '<'))==NULL)
        return  0;
    
    *inputFile=0;
    
    inputFile=strtok(inputFile+1," \t");
    
    if(inputFile==NULL)
        return  0;
    
    if((inputFileDescriptor=open(inputFile,O_RDONLY))==-1)
        return  -1;
    
    if(dup2(inputFileDescriptor, STDIN_FILENO)==-1){
        error=errno;
        close(inputFileDescriptor);
        errno=error;
        return  -1;
    }
    
    return close(inputFileDescriptor);
}

int shell::parseAndRedirectOut(char *command){
    int error;
    int outputFileDescriptor;
    char    *outputFile;
    
    if((outputFile=std::strchr(command, '>'))==NULL)
        return  0;
    
    *outputFile=0;
    
    outputFile=strtok(outputFile+1," \t");
    
    if(outputFile==NULL){
        return  0;
    }
    fputs(outputFile,stdout);

    if((outputFileDescriptor=open(outputFile,FFLAG, FMODE))==-1)
        return  -1;
    
    if(dup2(outputFileDescriptor, STDOUT_FILENO)==-1){
        error=errno;
        close(outputFileDescriptor);
        errno=error;
        return  -1;
    }
    return close(outputFileDescriptor);
}

void shell::executeCommand(char *commands){
    int child;
    int count;
    int fileDescriptors[2];
    int i;
    char **pipeList;
    
    count=makeargv(commands, "|", &pipeList);
    
    if(count<=0){
        numberOfChildrenProcesses[0]=1;
//        fprintf(stderr, "Failed to find any commands\n");     //Why bother?
        exit(1);
    }
    numberOfChildrenProcesses[0]=count;

    for(i=0; i<count-1; i++){                   //Only execute if there is more than one
        if(pipe(fileDescriptors)==-1){
            perror("Failed to create pipes");
            exit(1);
        } else if((child=fork())==-1){
            perror("Failed to create process to run command");
            exit(1);
        }else if(child){
            if(dup2(fileDescriptors[1], STDOUT_FILENO)==-1){
                perror("Failed to connect pipeline.");
                exit(1);
            }
            if(close(fileDescriptors[0])||close(fileDescriptors[1])){
                perror("Failed to close the needed files.");
                exit(1);
            }
            executeRedirect(pipeList[i],i==0,0);
            exit(1);
        }
        if(dup2(fileDescriptors[0], STDIN_FILENO)==-1){
            perror("Failed to connect last component.");
            exit(1);
        }
        if(close(fileDescriptors[0])||close(fileDescriptors[1])){
            perror("Failed to do final close.");
            exit(1);
        }
    }
    executeRedirect(pipeList[i],i==0,1);

}

void shell::executeRedirect(char *s, bool in, bool out){
    char **chargv;
    char *pin;
    char *pout;
    char *output;
    bool        internalCommand;
    if(
        in &&                               
        ((pin=strchr(s,'<'))!=NULL) &&      
        out&&
        ((pout=strchr(s,'>')) !=NULL) &&
        (pin>pout)
    ){
        if(parseAndRedirectIn(s)==-1){
            perror("Failed to redirect input\n");
            return;
        }
        in=0;
    }
    if(out && parseAndRedirectOut(s)==-1){
        perror("Failed to redirect output\n");
    }
    else if(in && parseAndRedirectIn(s)==-1){
        perror("Failed to redirect input\n");
    }
    else if(makeargv(s, " \t", &chargv)<=0){
        fprintf(stderr, "Failed to parse command line\n");
    }
    else{
        char*command;
        char*path;
        if(chargv[0][0]=='/')
            {command=chargv[0];}                //Full command path already included
        else
            {
                path=passToLocator(chargv[0]); //chargv[0]=command;
                if(strcmp(path, INTERNALCOMMANDPATH)==0){
                    internalCommand=true;
                    command=chargv[0];
                }else{
                    command=new char[std::strlen(path)+std::strlen(chargv[0])+2];
                    std::strcpy(command, path);
                    std::strcat(command,"/");
                    std::strcat(command,chargv[0]);
                }
                chargv[0]=command;
        }
        //for(int i=0;chargv[i]!=NULL;i++) fprintf(stderr,"chargv[%d]: %s\n",i, chargv[i]);
        if(internalCommand==false)
                execvp(chargv[0], chargv);                //Execute whether it's a good command or not.
        else{
                output=passToExecute(recombineArguments(chargv));
                //fprintf(stderr,"%s",output);
                fprintf(stdout,"%s",output);
                exit(1);
        }
        fprintf(stderr, "Could not find command: %s\n", chargv[0]);
        perror("Failed to execute command");
    }
    exit(1);
}

