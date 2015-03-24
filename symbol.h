
#include	<iostream>		// Necessary for I/O Stream functions
#include	<fstream>		// Necessary for File I/O function
#include	<iomanip>		// Necessary for I/O Manipulators 
#include	<stdlib.h>		// Needed for certain functions such as exit
#include	<ctype.h>		// Necessary for Functions that check 
							//			individual characters
#include	<string.h>		//	Necessary for C-style String manipulation functions
#include	<stack>			//	The stack template

using namespace std;

//	Constant declarations
const int	tabstop = 8;	// 8 characters per tabstop
// The size of the name table, hash table, string table 
//		and attribute table
const int	nametablesize = 200, hashtablesize = 100, 
			stringtablesize = 1200, attribtablesize = 200;

const int maxline = 121;	// No more than 120 characters per line + null

// JASON contains 21 keywords and 13 other tokens with entries in 
//	the symbol table
const int numkeywords = 21, numtokens = 34, labelsize = 10;

// The tokens
enum	tokentype {tokbegin, tokcall, tokdeclare, tokdo, 
			tokelse,tokend, tokendif, tokenduntil,  tokendwhile, tokif, 
			tokinteger, tokparameters, tokprocedure, tokprogram, tokread,
			tokreal, tokset, tokthen, tokuntil, tokwhile, tokwrite,
			tokstar, tokplus, tokminus, tokslash, tokequals,
			toksemicolon, tokcomma, tokperiod, tokgreater, tokless,
			toknotequal, tokopenparen, tokcloseparen, tokfloat,
			tokidentifier, tokconstant , tokerror, tokeof, tokunknown
};//

// The semantic types, i.e, keywords, procedures, variables, constants
enum semantictype	{stunknown, stkeyword, stprogram,
					stparameter, stvariable, sttempvar,
					stconstant, stenum, ststruct, stunion, stprocedure,
					stfunction, stlabel, stliteral, stoperator
};

// The data types, i.e, real and integer
enum datatype  {dtunknown, dtnone, dtprogram, dtprocedure,
				dtinteger, dtreal
};

// The structure for name table entries, i.e, a starting point in
// a long array, a pointer to the entry in the attribute table and 
//	the next lexeme with the same hash value.
typedef struct {
     int       strstart;
     int       symtabptr;
     int       nextname;
}	nametabtype; 

// The value can be either integer or real.  We save the tag which
// tells is which it is and store the result in a union.
enum tagtype   {tint, treal};

union valtype  {
     int	ival;
     float	rval;
};

// The structure that stores the tag and the value
typedef	struct	{
     tagtype   tag;
     valtype  val;
}	valrec;

// The structure of the attribute table entry, which includes:
//	semantic, token and data types
//	index of the procedure in which this symbol appears
//	index of the name table entry
//	index of the symbol's attribute table entry outside this scope
//	index of the next sentry in this scope so we can close them all
//	value of the constant
//	a label usually indicating address in the object code.
typedef	struct	{
     semantictype	smtype;
     tokentype		tok_class;
     datatype		dataclass;
     int			owningprocedure;
     int			thisname;
     int			outerscope, scopenext;
     valrec			value;
     char			label[labelsize];
}	attribtabtype;

// The structure for each item that is pushed on the procedure stack
// This includes:
//	index of the procedure in the attribute table
//	index of the first attribute table entry for 
//		this scope
//	index of the next attribute table entry for
//		this scope
typedef struct	{
	int	proc;
	int	sstart, snext;
}	procstackitem;

// The definition of the class SymbolTable
class	symboltable	{
public:
	symboltable(void);		// The only constructor
	/*void		display(void);  // I'm not sure that I need this */
	void		dump(void);		// Prints the entire symbol table 
								//		in user-friendly form
	//	Functions that set up the symbol table
	//		Installname() - 
	//		If it is's there returns true
	//		If it's not, it installs it and returns false
	bool		installname(char *string, int &tabindex);
	//	True if it's there; false if it's not
	bool		ispresent(char string[], int &tabindex);
	//	Creates and attribute table entry
	int			installattrib(int nameindex);
	//	Initializes the semantic type and token type
	void		setattrib(int tabindex, semantictype symbol, tokentype token);
	//  Initializes the data type for the entry
	void		installdatatype(int tabindex, semantictype stype, datatype dclass);
	// Functions that handle name scoping
	int			openscope(int tabindex);	// Opens a new scope
	void		closescope(void);		// Closes the scope
	//	Set and return the attribute table index for the 
	//		current procedure
	//		setproc() - Set the current procedure
	void		setproc(int thisproc, int tabindex);
	int			getproc(int tabindex);		// Get the current procedure
	// Functions that set and return the value of a constant
	void		setvalue(int tabindex, float val);	// Set it for a real
	void		setvalue(int tabindex, int val);	// Set it for an integer
	inline float
				getrvalue(int tabindex)	//Get a real value
				{return(attribtable[tabindex].value.val.rval);}	
	inline int  getivalue(int tabindex) // Get an integer value
				{return(attribtable[tabindex].value.val.ival);}
	// Functions which handles the labels inserted in the object
	void		getlabel(int tabindex, char varlabel[]);	// Returns label
	// Returns the # of bytes that the procedures's data uses
	int			labelscope(int	procindex);
	// Sets up the label for the procedures's parameters
	void		paramlabel(int tabindex, char label[], int &bytecount);
										
	//  Print the lexeme and token in user-friendly formats
	void		printlexeme(int i);
	void		printtoken(int i);
	
	// Returns basic information about symbols
	tokentype	gettok_class(int tabindex);	// Returns the token class
	inline enum datatype	// Returns the data type
		getdatatype(int tabindex)	{return(attribtable[tabindex].dataclass);}
	inline semantictype		// Returns the semantic type
				getsmclass(int tabindex) {return(attribtable[tabindex].smtype);}
	//	True if a valid type (real or integer); False if not
	inline bool	isvalidtype(int tabindex) 
				{return(attribtable[tabindex].dataclass == dtinteger
									||attribtable[tabindex].dataclass == dtreal);}
	// Returns the size of the attribute table
	inline int	tablesize(void)	{ return(attribtablen); }
    
private:
	// Initializes the procedure stack's entry
	procstackitem
				initprocentry(int tabindex);
	// Returns the hash value for a given lexeme
	int			hashcode(char string[]);
//	True if it's there; false if it's not
	bool		ispresent(char string[], int &code, int &nameindex);
	// Prints the lexeme in all capital letters
	void		LexemeInCaps(int tabindex);
	// Creates a label for the object code
	void		makelabel(int tabindex, char *label);
	//  Returns the larger of two integer values
	inline	int	min(int a, int b)	{return ((a < b)? a : b);}
	// The string table is a long string in which all lexemes are stored
	char		stringtable[stringtablesize];
	// A series of indices pointing to the lexeme within the string table
	//	 as well as to the relevant attribute table entry
	nametabtype	nametable[nametablesize];
	// The attribute table entries
	attribtabtype
				attribtable[attribtablesize];
	int			hashtable[hashtablesize];
	// The lengths of the string table, name table and attribute table
	int			strtablen, namtablen, attribtablen;

	procstackitem
				thisproc;	// A stack entry for the current procedure
	stack <procstackitem>	ps;	// The procedure stack
};
