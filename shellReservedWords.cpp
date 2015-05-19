/*
 * Name: shellReservedWords.cpp
 * Authors: Dean M. Sands, III of Team DDD
 * Contains functions relating to reserved words in DDD shell.
 */

#include "shell.h"

std::string shell::pushDirectory(void **arguments){
    
    directories.push_back(*(new std::string(getenv("PWD"))));
    return "Directory stored.\n";
}
std::string shell::popDirectory(void **arguments){
    std::string lastDirectory;
    if(!directories.empty()){
        lastDirectory=directories.back();
        if(chdir(lastDirectory.c_str())==-1){
            perror("Error popping previous directory. Did you delete it?");
        }
        directories.pop_back();
        lastDirectory.clear();
    }
    else{
        lastDirectory.clear();
        return "Directory could not be retrieved.\n";
    }
    return "Directory retrieved.\n";
}
std::string shell::help(void **arguments){
    std::string returnString;
    
    returnString=version(NULL);
    for (ReservedWordMapType::iterator myReservedWord = reserved_words.begin();
        myReservedWord != reserved_words.end();
        ++myReservedWord){
        returnString+=myReservedWord->second->GetHelp();
    }
    return returnString;
}

std::string shell::version(void **arguments){
    std::string returnString=ANSIRESET;

    returnString+=ANSI_BOLD;
    returnString+=ANSIBLUFG;
    returnString+=NAME;
    returnString+=" ";
    returnString+=ANSICYNFG;
    returnString+=VERSION;
    returnString+=ANSIGRNFG;
    returnString+=" build ";
    returnString+=ANSIYLWFG;
    returnString+=__DATE__;     //Internal value filled in by G++
    returnString+="@";
    returnString+=__TIME__;     //Internal value filled in by G++
    returnString+="\n";
    returnString+=ANSIREDFG;
    returnString+=DESCRIPTION;
    returnString+="\n";
    returnString+=ANSIRESET;
    
    return returnString;
}

std::string shell::exitShell(void **arguments){
    done=true;
    return "Exiting.\n";
}
std::string shell::changeDirectory(void **arguments){
    if(arguments[1]==NULL?true:chdir((char*)arguments[1])==-1)
        perror("Error changing directory.");
    return "Directory changed.\n";
}

std::string shell::makeDirectory(void **arguments){
    if(arguments[1]==NULL?true:mkdir((char*)arguments[1], S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)==-1)
        perror("Error creating directory.");
    return "Directory created.\n";
}


void shell::addReservedWord(std::string myID, std::string(*myFunctor)(void **), std::string myHelp){
    reserved_words[myID]=new ReservedWords(myID, myFunctor, myHelp);
}

void shell::initReservedWords(){
    addReservedWord("cd", shell::changeDirectory,"cd: Change current directory.\n");
    addReservedWord("mkdir", shell::makeDirectory,"mkdir: Create new directory.\n");
    addReservedWord("md", shell::changeDirectory,"md: Create new directory.\n");
    addReservedWord("pushd", shell::pushDirectory,"pushd: Store current directory to stack.\n");
    addReservedWord("popd", shell::popDirectory,"popd: Retrieve directory from stack.\n");
    addReservedWord("exit", shell::exitShell,"exit: Leave the shell.\n");
    addReservedWord("ver", shell::version,"version: Display ddd shell version.\n");
    addReservedWord("help", shell::help,"help: Display this list.\n");
}
