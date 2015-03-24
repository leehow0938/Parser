//
//  main.cpp
//  Compiler
//
//  Created by HAO LI on 3/12/15.
//  Copyright (c) 2015 HAO LI. All rights reserved.
//

#include <iostream>
#include "scan.h"
using namespace std;

// this array is used to print out the tokentype. Since tokentype is enum cannot be printed out directly.
char const *typestr[] = {"tokbegin", "tokcall", "tokdeclare", "tokdo",
        "tokelse","tokend", "tokendif", "tokenduntil",  "tokendwhile", "tokif",
        "tokinteger", "tokparameters", "tokprocedure", "tokprogram", "tokread",
        "tokreal", "tokset", "tokthen", "tokuntil", "tokwhile", "tokwrite",
        "tokstar", "tokplus", "tokminus", "tokslash", "tokequals",
        "toksemicolon", "tokcomma", "tokperiod", "tokgreater", "tokless",
        "toknotequal", "tokopenparen", "tokcloseparen", "tokfloat",
        "tokidentifier", "toknumber", "tokerror", "tokeof", "tokunknown"
    };

// Task here is to scan a file and return the tokens.
int main(int argc, const char * argv[]) {
    // vriable i is used to be a parameter of gettoken()
    int i=0;
    tokentype thistoken;
    
    scanner s;
    // loop calling gettoken() scan and return a new token, and stop when
    // reach the end of file.
    while((thistoken=s.gettoken(i))!=tokeof)
    {
        cout<<"      "<<typestr[thistoken]<<endl;
        
    }
    return 0;
}
