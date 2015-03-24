/*#include	<iostream>		// Necessary for I/O Stream Functions*/
#include	<stdio.h>		// Necessary for Pausing the output
/*#include	<fstream>		// Necessary for File I/O Functions

#include	<stdlib.h>		// Needed for certain function such as exit
#include	<string.h>		// Necessary for C-style String manipulation functions
#include	<ctype.h>		// Necessary for Functions that check
							//			individual characters*/
#include	"symbol.h"		//	Uses the Symbol Table Manager

//	This is used before the symbol table manager is included so
// that there are dummy token s to return
/*enum tokentype	{tokidentifier, toknumber, toksymbol, tokeof, tokerror};
const char	*tokenstring[] = {"identifier", "number", "symbol",
				 "eof", "error"};*/

const	int		FileNameLen = 41;	// Maximum length of file names
const	int		LexemeLen = 81;		// Maximum length of lexemes
const	char	EndOfFile = '\0';	// Indicates "End of File"

// The Scanner Class
class	scanner	{
public:
	scanner(int argc, char *argv[]);	// Constructor that gets the file name
	scanner(void);						// Default constructor
	~scanner(void);						// The destructor
	tokentype	gettoken(int &tabindex);	// Returns the next token to the parser
private:
	char		gettc(void);		// Get the next character, updating the line count
    void		ungettc(char c);	// Takes back a character, updating the line count
	// Returns the first character after white space and comments
	char		firstchar(void);
	// Scans a word - assumes that the first character is a letter
	tokentype	scanword(char c, int &tabindex);
	// Scans a number - assumes that the first character is a digit
	tokentype	scannum(char c, int &tabindex);
	// Scans an operator - assumes that the first character is 
	//					non-alphanumeric
	tokentype	scanop(char c, int &tabindex);
	// True if the file is found, false if not
	bool		filefound(char	*filename);
	char		lookahead;		// The lookahead character
	ifstream	infile;			// The source file
protected:
	int			linenum;		// The current line number
};
