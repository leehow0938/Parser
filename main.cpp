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

// Task here is to scan a file and return the tokens.
int main(int argc, const char * argv[]) {
    scanner s;
    
    // vriable i is used to be a parameter of gettoken(), but at this point,
    // we do not have a parser, so it has no meaning and is just a parameter
    // to let the program run.
    int i=0;
    tokentype thistoken;
    
    // loop calling gettoken() scan and return a new token, and stop when
    // reach the end of file.
    while((thistoken=s.gettoken(i))!=tokeof)
    {
        switch (thistoken)
        {
            case tokword: cout<<"tokword"<<endl; break;
            case toknumber: cout<<"toknumber"<<endl;break;
            case tokop: cout<<"tokop"<<endl;break;
                
            case toksymbol:cout<<"toksymbol"<<endl; break ;
            case tokeof:cout<<"tokeof"<<endl; break ;
            case tokerror:cout<<"tokerror"<<endl; break ;
            default : break ;
        }
    }
    return 0;
}
