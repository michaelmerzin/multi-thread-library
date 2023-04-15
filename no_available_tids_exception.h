//
// Created by michaelmerzin on 4/15/23.
//

#ifndef OS_EX2_NO_AVAILABLE_TIDS_EXCEPTION_H
#define OS_EX2_NO_AVAILABLE_TIDS_EXCEPTION_H


#include <exception>

class no_available_tids_exception : public std::exception {
public:
    virtual char *what() const throw() {
        return (char *) "no available tids";
    }

};


#endif //OS_EX2_NO_AVAILABLE_TIDS_EXCEPTION_H
