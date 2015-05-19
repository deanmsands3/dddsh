/* 
 * File:   ReservedWords.h
 * Author: dean
 *
 * Created on April 29, 2011, 3:04 AM
 */

#ifndef RESERVEDWORDS_H
#define	RESERVEDWORDS_H
#include <string>
#include <cstring>
struct eqstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) == 0;
  }
};

class ReservedWords {
private:
    std::string id;
    std::string (*functor)(void **);
    std::string help;
public:
    ReservedWords(std::string myID, std::string(*myFunctor)(void **), std::string myHelp);
    ReservedWords(const ReservedWords& orig);
    virtual ~ReservedWords();

    std::string GetId() const {
        return id;
    }

    std::string(*GetFunctor())(void**) const {
        return functor;
    }

    std::string GetHelp() const {
        return help;
    }




};

#endif	/* RESERVEDWORDS_H */

