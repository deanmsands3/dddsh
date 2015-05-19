/* 
 * File:   ReservedWords.cpp
 * Author: dean
 * 
 * Created on April 29, 2011, 3:04 AM
 */

#include "ReservedWords.h"

ReservedWords::ReservedWords(std::string myID, std::string(*myFunctor)(void **), std::string myHelp) {
    id=myID;
    help=myHelp;
    functor=myFunctor;
}

ReservedWords::ReservedWords(const ReservedWords& orig) {
    id=orig.id;
    functor=orig.functor;
    help=orig.help;
}

ReservedWords::~ReservedWords() {
    help.clear();
    id.clear();
}
