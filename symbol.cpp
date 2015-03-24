#include	"symbol.h"

//	The key words and operators - used in initializing the symbol
//	table
char *keystring[] = {"begin", "call", "declare",
          "do", "else", "end", "endif", "enduntil", "endwhile",
          "if", "integer", "parameters", "procedure", "program",
          "read", "real", "set", "then", "until", "while",
          "write", "*", "+", "-", "/", "=", ";",
          ",", ".", ">", "<", "!", "(", ")", "_float"
};


//	The names of the token classes in a format that can
//	be printed in a symbol table dump
const char *tokclstring[] = {   "begin     ", "call      ",
			"declare   ", "do        ", "else      ", "end       ", 
			"endif     ", "enduntil  ", "endwhile  ", "if        ", 
			"integer   ", "parameters", "procedure ", "program   ", 
			"read      ", "real      ", "set       ", "then      ",
			"until     ", "while     ", "write     ", "star      ", 
			"plus      ", "minus     ", "slash     ", "equals    ",
			"semicolon ", "comma     ", "period    ", "greater   ",
			"less      ", "notequal  ", "openparen ", "closeparen", 
			"float     ", "identifier", "constant  ", "error     ", 
			"eof       ", "unknown   "
};

//	The names of the semantic types in a format that can be 
//	printed  in a symbol table dump
const char *symtypestring[] =  {"unknown  ", "keyword  ", "program  ",
		    "parameter", "variable ", "temp. var",
		    "constant ", "enum     ", "struct   ",
		    "union    ", "procedure", "function ",
		    "label    ", "literal  ", "operator "
};

//	The names of the data types in a format that can be 
//	printed  in a symbol table dump
const char *datatypestring[] = {"unknown","none   ", "program",
                    "proced.", "integer", "real   "
};

// symboltable() -	The initializing constructor for 
//					the symbol table
symboltable::symboltable(void)
{
	int  i, nameindex;
	
	// Initialize the first entry for the procedure stack
	thisproc = initprocentry(-1);

	// Initialize the hash table, the name table's next 
	//     field and the attribute table's fields as -1.  
	for  (i = 0; i < hashtablesize;  i++)
		hashtable[i] = -1;

	for  (i = 0;  i < nametablesize;  i++)
		nametable[i].nextname = -1;

	//	The attribute table's fields are all initially
	//	unknown, 0 or -1 (if they're indices).
	for  (i = 0;  i < attribtablesize;  i++)   {
		attribtable[i].smtype = stunknown;
		attribtable[i].tok_class = tokunknown;
		attribtable[i].thisname = -1;
		attribtable[i].value.tag = tint;
		attribtable[i].value.val.ival = 0;
		attribtable[i].owningprocedure = -1;
		attribtable[i].outerscope = -1;
		attribtable[i].scopenext = -1;
		attribtable[i].label[0] = '\0';
	}

	// Install the keywords in the name table and
	//          set their attributes to keyword 
	for  (i = 0;  i < numkeywords;  i++)    {
		installname(keystring[i], nameindex);
		setattrib(nameindex, stkeyword, (tokentype)i);
	}

	// Install the operators in the name table and
	//          set their attributes to operator 
	for  (i = numkeywords; i < numtokens;  i++)  {
		installname(keystring[i],nameindex);
		setattrib(nameindex, stoperator, (tokentype)i);
	}

	//	Initialize the entry for float, the routine
	//	that converts values from integer to real
	installname(keystring[i], nameindex);
	setattrib(nameindex, stfunction, (tokentype)i);
	installdatatype(nameindex, stfunction, dtreal);

    	cout << "All initiallized" << endl;
    //dump();
}


// DumpSymbolTable() -	Prints out the basic symbol table
//						information, including the name and token 
//						class

void symboltable::dump(void)
{
	int  i, j;
	char *printstring;

	//	Print the symbol table's heading
	cout << "SYMBOL TABLE DUMP\n-----------------\n\n";
	cout << "                   Token       Symbol     Data";
	cout << "              Owning\n";
	cout << "Index   Name       Class       Type       Type";
	cout << "     Value   Procedure    Label\n";
	cout << "-----   ----       -----       ------     ----";
	cout << "     -----   ---------\n";

	//	Print the data for each entry
	for  (i = 0;  i < attribtablen;  i++) {
		//	Pause every tenth line
		if (i%10 == 9) getchar();

		//	Print the entry number and lexeme
		cout << setw(5) << i << '\t';
		printlexeme(i);

		//
		//	After printing the lexeme, move to column 20.  If
		//	the name is too long to permit, go to the next
		//	line
        
		printstring = stringtable + nametable[attribtable[i].thisname].strstart;
		if (strlen(printstring) < 11)
			for (j = 0;  j < 11 - strlen(printstring);	j++)
				cout << ' ';
		else
			cout << "\n          ";

		// Print the token class, symbol type and data type 
		cout << tokclstring[attribtable[i].tok_class] << "  ";
		cout << symtypestring[attribtable[i].smtype] << "  ";
		cout << datatypestring[attribtable[i].dataclass] << "  ";

		//	If the value is real or integer, print the 
		//	value in the correct format.
		if (attribtable[i].value.tag == tint)
			cout << setw(5) << attribtable[i].value.val.ival;
		else
			cout << setprecision(3) << setw(8) 
					<< attribtable[i].value.val.rval;

		//	If there is no procedure that owns the symbol
		//	(which is the case for reserved words, operators,
		//	and literals), print "global."
		if (attribtable[i].owningprocedure == -1)
			cout << "   global";
		//	Otherwise print the name of the owning
		//	procedure in capital letters to make it 
		//	stand out.
		else {
			cout << "   ";
			LexemeInCaps(attribtable[i].owningprocedure);
		}

		//	Print the assembly language label.
		cout << "   " << attribtable[i].label;
		cout << endl;
	}

}

// InstallName() -	Check if the name is already in the table.
//					If not add it to the name table and create
//					an attribute table entry.
bool	symboltable::installname(char string[], int &tabindex)
{
	int  i, code, length, nameindex;

	// Use the function ispresent to see if the token string
	// is in the table.  If so, return a pointer to its
	// attribute table entry.
	length = strlen(string);
	if (ispresent(string, code, nameindex)) {
		if (nametable[nameindex].symtabptr == -1)    {
			tabindex = installattrib(nameindex);
			return(false);
		}
		else	{
			tabindex = nametable[nameindex].symtabptr;
			return(true);
		}
	}

     // If not create entries in the name table, copy the name
     // into the string table and create a hash table entry
     // (linking it to its previous entry if necessary) and
     // create an entry in the attribute table with the
     // bare essentials.
     nametable[nameindex = namtablen++].strstart = strtablen;
     
	for  (i = 0;  i < length;  i++)
		stringtable[strtablen++] = string[i];
	stringtable[strtablen++] = '\0';
	nametable[nameindex].nextname = hashtable[code];
	hashtable[code] = nameindex;
	tabindex = installattrib(nameindex);
	return(false);
}


// IsPresent() -	After finding the hash value, it traces
//					through the hash list, link by link looking to see
//					if the current token string is there.
//					This version is private and is intended for use
//					by installname
bool   symboltable::ispresent(char string[], int &code, int &nameindex)
{
	bool	found = false;
	int		oldnameindex, k;

	/* Initialize the old name's index to -1; 
                                   it may not be there */
	oldnameindex = -1;

	// Find the hash value 
	code = hashcode(string);

     
	// Starting with the entry in the hash table, trace through
	// the name table's link list for that hash value.
    for  (nameindex = hashtable[code];
				!found && nameindex != -1;
			oldnameindex = nameindex,
			nameindex = nametable[nameindex].nextname) {
		k = nametable[nameindex].strstart;
		found = !strcmp(string, stringtable+k);
	}

	// If it's there, we actually went right past it.
	if (found)     
		nameindex = oldnameindex;
	return(found);
}

// IsPresent() -	After finding the hash value, it traces
//					through the hash list, link by link looking to see
//					if the current token string is there.
//					This version is public and can be used to 
//					determine if the lexeme is in the symbol table
//					without installing it and also returns the index
//					within the attribute table.
bool   symboltable::ispresent(char string[], int &tabindex)
{
	bool	found = false;
	int		nameindex, oldnameindex, k, code;

	// Initialize the old name's index to -1; 
	//	it may not be there 
	oldnameindex = -1;

	// Find the hash value 
	code = hashcode(string);

     
	// Starting with the entry in the hash table, trace through
	// the name table's link list for that hash value.      
	for  (nameindex = hashtable[code];
				!found && nameindex != -1;
			oldnameindex = nameindex,
			nameindex = nametable[nameindex].nextname) {
		k = nametable[nameindex].strstart;
		found = !strcmp(string, stringtable+k);
	}

	// If it's there, we actually went right past it.
	if (found)     
		nameindex = oldnameindex;
	tabindex = nametable[nameindex].symtabptr;
	return(found);
}

// HashCode() -	A hashing function which uses the characters
//				from the end of the token string.  The algorithm comes
//				from Matthew Smosna of NYU.
int  symboltable::hashcode(char string[])
{
	int       i, numshifts, startchar, length;
	unsigned  code;

	length = strlen(string);
	//	The number of shifts cannot exceed the bytes in an integers
	//	less 8; any more and bits within a given character will be
	//	lost.
	numshifts = (int) min(length, (8*sizeof(int)-8));

	startchar = ((length-numshifts) % 2);
	code = 0;

	//	Left shift one place and add the current character's ASCII
	//	value to the total.
	for (i = startchar;  i <= startchar + numshifts - 1;  i++)
		code = (code << 1) + string[i];

	//	Divide by the table size and use the remainder as the hash 
	//	value.
	return(code % hashtablesize);
}

// InstallAttrib() -	Create a new entry in the attribute
//						table and have this name table 
//						entry point to it.
int	symboltable::installattrib(int nameindex)
{
	int  tabindex;

	tabindex = nametable[nameindex].symtabptr = attribtablen++;
	attribtable[tabindex].thisname = nameindex;
	attribtable[tabindex].smtype = stunknown;
	attribtable[tabindex].dataclass = dtunknown;

	// Return the index of the attribute table entry
	return(tabindex);
}

// SetAttrib() -	Set attribute table information, given
//					a pointer to the correct entry in the table.
void symboltable::setattrib(int tabindex, semantictype symbol, tokentype token)
{
	//	Install semantic type and token class
	attribtable[tabindex].smtype = symbol;
	attribtable[tabindex].tok_class = token;

	//	Reserved words and operators do not need data types
	if (attribtable[tabindex].smtype == stkeyword
               || attribtable[tabindex].smtype == stoperator)
		attribtable[tabindex].dataclass = dtnone;
	else
		//	Other symbols have not yet had their 
		//	data types determined
		attribtable[tabindex].dataclass = dtunknown;

	//	If it's an identifier and it isn't global
	if (gettok_class(tabindex) == tokidentifier
                              && thisproc.proc != -1)
		//	If no other scope has a variable with this name
		//	connect its listing to other identifiers in
		//	this scope
    {
		if (thisproc.sstart == -1)
        {
			thisproc.sstart = tabindex;
			thisproc.snext = tabindex;
		}
		//	Otherwise, connect it to the attribute table
		//	entries for this name in outer scopes
		else
        {
			attribtable[thisproc.snext].scopenext = tabindex;
			thisproc.snext = tabindex;
        }
    }
}

// InstallDataType() -	Install basic data type information, 
//						i.e., the data type and semantic type
void	symboltable::installdatatype(int tabindex, semantictype stype, datatype dclass)
{
	attribtable[tabindex].smtype = stype;
	attribtable[tabindex].dataclass = dclass;
}

// OpenScope() -	Open a new scope for this identifier
int  symboltable::openscope(int tabindex)
{
	int  newtabindex, nameindex;

	// Get the index in the name table
	nameindex = attribtable[tabindex].thisname;
	//	Create a new attribute table entry and 
	//	initialize its information
	newtabindex = installattrib(nameindex);
	setattrib(newtabindex, stunknown, tokidentifier);
	// Have this entry point to the outer scope's entry
	attribtable[newtabindex].outerscope = tabindex;
	return(newtabindex);
}

// CloseScope() -	Close the scope for ALL the 
//					identifiers for the scope
void symboltable::closescope(void)
{
     int  nmptr, symptr;

     //	Start at the first identifier that belongs to the
	 //	procedure and for each identifier
	 for (symptr = thisproc.sstart;  symptr != -1;
               symptr = attribtable[symptr].scopenext) {
		  // Have it point to the outer scope's 
		  // attribute table entry
          nmptr = attribtable[symptr].thisname;
          nametable[nmptr].symtabptr = attribtable[symptr].outerscope;
        }

}

// SetProc() -	Set the identifier's owning procedure
void symboltable::setproc(int thisproc, int tabindex)
{
     attribtable[tabindex].owningprocedure = thisproc;
}

// GetProc() -	Returns the identifier's owning procedure
int  symboltable::getproc(int tabindex)
{
     return(attribtable[tabindex].owningprocedure);
}

// SetValue() -	Set the value for a real identifier
void symboltable::setvalue(int tabindex, float val)
{
     attribtable[tabindex].value.tag = treal;
     attribtable[tabindex].value.val.rval = val;  
}

// SetValue() -	Set the value for an integer identifier
void symboltable::setvalue(int tabindex, int val)
{
     attribtable[tabindex].value.tag = tint;
     attribtable[tabindex].value.val.ival = val;  
}

//	getlabel() -		Gets a label which is used by the final code
//						generator.  If the label is not installed in the
//						symbol table, it creates one and returns it.
void	symboltable::getlabel(int tabindex, char varlabel[])
{
	if (attribtable[tabindex].label[0] != '\0')
		strcpy(varlabel, attribtable[tabindex].label);
	else
        makelabel(tabindex, varlabel);
}

// makelabel() -	Makes a label which is used by the final code
//					generator and installs it in the symbol table.
void	symboltable::makelabel(int tabindex, char *label)
{
	int			i;
	//float		fvalue;
	char		indexstr[5];
	//enum symboltype thissymbol;

	/*cout << "in makelabel, ";
	printlexeme(tabindex);
	cout <<" is it"<< endl;*/
	label[0] = '\0';
	switch (getsmclass(tabindex))	{
	  case stliteral:
          	if (getdatatype(tabindex) == dtinteger)	{
	  		i = attribtable[tabindex].thisname;
			strcpy(label, stringtable+nametable[i].strstart);
			break;
		}


	  case sttempvar:	label[0] = '_';
				label[1] = 't';
				label[2] = '\0';
				//itoa(tabindex, indexstr, 10);
                sprintf(indexstr, "%d",tabindex);
				strcat(label, indexstr);
				break;

	  case stlabel:		strcpy(label, "_loop");
				//itoa(tabindex, indexstr, 10);
                sprintf(indexstr, "%d",tabindex);
				strcat(label, indexstr);
				break;
	  case stprogram:
	  case stvariable:	
	  case stparameter:
	  case stprocedure:
	  	i = attribtable[tabindex].thisname;

		strcpy(label, stringtable+nametable[i].strstart);
		if (strlen(label) >= 5)	{
			//itoa(tabindex, indexstr, 10);
            sprintf(indexstr, "%d",tabindex);
			strcat(label, indexstr);
		}
	}
		strcpy(attribtable[tabindex].label, label);
		cout << "Tabindex is " << tabindex << endl;
}

int	symboltable::labelscope(int	procindex)
{
	int		oldsymptr, symptr, numbytes = 0, totalbytes;
	char	label[labelsize];

	for  (symptr = getivalue(procindex); symptr != 0;
			symptr = getivalue(symptr))
		numbytes += (getdatatype(symptr) == dtinteger)? 2 : 4;

	totalbytes = numbytes;
	for  (oldsymptr = symptr, symptr = getivalue(procindex);
			symptr != 0;
			oldsymptr = symptr, symptr = getivalue(symptr))	
		paramlabel(symptr, label, numbytes);


	numbytes -=2;
	for (symptr = (getivalue(procindex) == 0)?
			procindex+1: attribtable[oldsymptr].scopenext;
			symptr != -1 && getsmclass(symptr) != stprocedure;
			symptr = attribtable[symptr].scopenext)	
		paramlabel(symptr, label, numbytes);
	return(-numbytes-2);
}

void	symboltable::paramlabel(int tabindex, char label[], int &bytecount)
{
	//	int				i, j, k, m, n, ivalue;
	//	float			fvalue;
	char			indexstr[5];
	//	enum symboltype thissymbol;

	if (bytecount < 0)	{
        	if (getdatatype(tabindex) == dtinteger)
			strcpy(label, "[bp");
		else
			strcpy(label, "[bp");
	}
	else
        	strcpy(label, "[bp");
	if (bytecount > 0)
		strcat(label, "+");
	//itoa(bytecount, indexstr, 10);
    sprintf(indexstr, "%d",bytecount);
	strcat(label, indexstr);
	bytecount -= getdatatype(tabindex) == dtinteger? 2: 4;
	strcat(label, "]");
	strcpy(attribtable[tabindex].label, label);
}

// PrintLexeme() - Print the lexeme for a given token
void	symboltable::printlexeme(int tabindex)
{
	int  i, j;
	char	*s;

	i = attribtable[tabindex].thisname;

	j = nametable[i].strstart;
	s = stringtable + j;
	cout << s ;
}

// PrintToken() -	Print the token class's name given the token
//                  class.
void symboltable::printtoken(int i)
{
     cout << tokclstring[(int)gettok_class(i)];
}

// GetTok_Class()	- Returns the token class for the symbol
tokentype	symboltable::gettok_class(int tabindex)	{
	return(attribtable[tabindex].tok_class);
}

// InitProcEntry() -	Initialize the entry on the 
//						Procedure Stack with the current symbol
procstackitem	symboltable::initprocentry(int tabindex)
{
	procstackitem	thisproc;

	thisproc.proc = tabindex;
	thisproc.sstart = -1;
	thisproc.snext = -1;
	return(thisproc);
}


// LexemeInCaps() -	Print the lexeme in capital letters
//					This makes it more distinctive
void	symboltable::LexemeInCaps(int tabindex)
{
	int		i, j;

	//	Get the index within the string table 
	//	where the lexeme starts
	i = attribtable[tabindex].thisname;
	//	Until you encounter the ending null byte,
	//	Print the character in upper case.
	for  (j = nametable[i].strstart;  stringtable[j] != '\0';  j++)
		cout <<toupper(stringtable[j]);
}
