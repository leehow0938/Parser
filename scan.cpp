#include	"scan.h"														 

symboltable	st;
// scanner() -	Checks the command-line parameter count in
//				search of a second parameter that will be the
//				filename.  If there is only one argument, it's the
//				name of the program's executable file.  If there are
//				more than two, it is an unrecoverable error - Jason
//				does not compile multi-file programs.
scanner::scanner(int argc, char *argv[])
{
	char	filename[FileNameLen];

	// Do we need to get the file name? 
	switch (argc)	{
		case 1:	cout << "File name\t?";
				cin >> filename;
				break;

		case 2:	strcpy(filename,argv[1]);
				break;

		default:cout << "Usage: Jason <filename>\n";
				exit(1);
	}
	
	// Open the file and quit with an error message
	//	if the file cannot be opened
	if (!filefound(filename))	{
		cout << "Cannot find "<< filename << endl;
	}

	infile.open(filename, ios::in);
	if (!infile)	{
		cout << "Cannot open " << filename << endl;
		exit(2);
	}

	//  Initialize the line count and get the 
	//	lookahead character.
	linenum = 1;
	if (!infile.eof())
		lookahead = firstchar();
}

// scanner() -	Assumes that there was no file name passed using
//				command-line parameters, so it asks the user for a
//				file name.
scanner::scanner(void)
{
	char	filename[FileNameLen];
	

	cout << "File name\t?";
	//cin >> filename;
    strcpy(filename,"/Users/Leehow/Desktop/2.txt");
    cout<<filename<<endl;
    
    
	// Open the file and quit with an error message
	//	if the file cannot be opened 
	if (!filefound(filename))	{
		cout << "Cannot find " << filename << endl;
	}
	infile.open(filename, ios::in);
	if (!infile)	{
		cout << "Cannot open " << filename << endl;
		exit(2);
	}

	//  Initialize the line count and get the 
	//	lookahead character.
	linenum = 1;
	if (!infile.eof())
		lookahead = firstchar();
}

// ~scanner() -	Close the file when finished with it.
scanner::~scanner(void)
{
	infile.close();
}

// filefound() -	Returns true if the file exists
//					Returns false if it doesn't
bool	scanner::filefound(char	*filename)
{
	FILE	*fp;
	bool	value;

	//Uses the standard I/O library's fopen function
	// If the file pointer is NULL, the file does not 
	//	exist or cannot be opened for some other reason.
	if ((fp = fopen(filename, "r")) != NULL)	{
		value = true;
		fclose(fp);
	}
	else
		value = false;
	
	return(value);
}


// gettoken() -	Scan out the token strings of the language 
//				and return the corresponding token class 
//				to the parser. 
tokentype	scanner::gettoken(int &tabindex)
{
	char	c;

	
	// If this is the end of the file, send the
	// token that indicates this
	
	if ((c = lookahead) == EndOfFile/* || (int)c == -1*/)
        	return(tokeof);
	
	// If it begins with a letter, it is a word.  If
	// begins with a digit, it is a number.  Otherwise,
	// it is an error.
	lookahead = gettc();
	if (isalpha(c))
		return(scanword(c, tabindex));
	else if (isdigit(c))	
		return(scannum(c, tabindex));
	else		
		return(scanop(c, tabindex));

}

// scanword() -	Scan until you encounter something other than a letter.
tokentype	scanner::scanword(char c, int &tabindex)
{
	char	lexeme[LexemeLen];
	int		i = 0;

	// Build the string one character at a time.  It keeps
	// scanning until either the end of file or until it
	// encounters a non-letter
	lexeme[i++] = c;
	while ((c = lookahead) != EndOfFile	&& (isalpha(c) || isdigit(c)))	{
		lexeme[i++] = c;
		lookahead = gettc();
	}

	//	Add a null byte to terminate the string and get the 
	//	lookahead that begins the next lexeme.
	lexeme[i] = '\0';
    
    // Print the original lexeme
    cout<< "\"" << lexeme << "\"";
    
	ungettc(lookahead);
	lookahead = firstchar();
	
	//	If the lexeme is already in the symbol table,
	//	return its tokenclass.  If it isn't, it must
	//	be an identifier whose type we do not know yet.
	if (st.installname(lexeme, tabindex))
		return(st.gettok_class(tabindex));
	else	{
		st.setattrib(tabindex, stunknown, tokidentifier);
		return(tokidentifier);
	}

	/*return(tokidentifier);*/
}

// scannum() -	Scan for a number.
tokentype	scanner::scannum(char c, int &tabindex)
{
	int		ival, i = 0;
	bool	isitreal = false;
	float	rval;
    char	lexeme[LexemeLen];


	// Scan until you encounter something that cannot be
	// part of a number or the end of file 
	lexeme[i++] = c;
	while ((c = lookahead) != EndOfFile && isdigit(c))	{
		lexeme[i++] = c;
		lookahead = gettc();
	}

	

	// Is there a fractional part?
	if (c == '.')	{
		lookahead = gettc();
		isitreal = true;
		lexeme[i++] = c;	
		while ((c = lookahead) != EndOfFile && isdigit(c))	{
			lexeme[i++] = c;
			lookahead = gettc();
		}
	}

	//	Add a null byte to terminate the string and get the 
	//	lookahead that begins the next lexeme.
	ungettc(lookahead);
	lexeme[i] = '\0';
	cout <<"\"" << lexeme << "\"";
	lookahead = firstchar();
		
	//If there is no fractional part, it is an integer literal
	// constant.  Otherwise, it is a real literal constant.
	// Firstly, is it already in the symbol table?
	if (st.installname(lexeme, tabindex))
		return(st.gettok_class(tabindex));
	// If not, is it real?
	else if (isitreal)	{
		st.setattrib(tabindex, stunknown, tokconstant);
		st.installdatatype(tabindex, stliteral, dtreal);
		rval = atof(lexeme);
		st.setvalue(tabindex, rval);
		return(st.gettok_class(tabindex));
	}
	// Must be an integer literal
	else	{
		st.setattrib(tabindex, stunknown, tokconstant);
		st.installdatatype(tabindex, stliteral, dtinteger);
		ival = atoi(lexeme);
		st.setvalue(tabindex, ival);
		//ungettc(lookahead);
		return(st.gettok_class(tabindex));
	}
	//ungettc(lookahead);
	return(st.gettok_class(tabindex));
}

// scanop() -	Scan for an operator, which is a single character
//		other than a letter or number.
tokentype	scanner::scanop(char c, int &tabindex)
{
	char	lexeme[LexemeLen];

	
	lexeme[0] = c;
	//	Add a null byte to terminate the string and get the 
	//	lookahead that begins the next lexeme.
	ungettc(lookahead);
	lexeme[1] = '\0';
    cout << "\"" << lexeme << "\"";
	lookahead = firstchar();
	
	//	The only valid operators are already in the symbol 
	//	table.  If it's not there already, it must be 
	//	invalid
	if (!st.ispresent(lexeme, tabindex))	{
		cerr << lexeme << " is an illegal operator on line #"
			<< linenum << endl;
		exit(3);
	}

	return(st.gettok_class(tabindex));
}

// firstchar() -	Skips past both white space and comments until
//					it finds the first non-white space character
//					outside a comment.
char	scanner::firstchar(void)
{
	char	c;
	bool	goodchar = false;	


	//	If we're at the end of the file, return the EOF 
	//	marker so that we'll return the EOF token
	if (infile.eof())	
			return(EndOfFile);

	// We're looking for a non-white space character 
	// that is outside a comment.  Keep scanning until
	// we find one or reach the end of the file.
	while	(!goodchar)	{
		// Skip the white space in the program
		while (!infile.eof() && isspace(c = gettc()))
			;
			

		// Is it a comment or a real first character? 
		if  (c != '{')
			goodchar = true;
		else
			// Skip the comment 
			while (!infile.eof() && (c = gettc()) != '}')
                		;
	}

	// If we're at the end of file, return the EOF marker/
	// Otherwise, return the character.
	if (infile.eof())	
		return(EndOfFile);
	else
		return(c);
}

// gettc() -	Fetches a character from a file.  It uses getc and adjusts
//		the line number count when necessary.	
char	scanner::gettc(void)
{
	char	c;

	//	If we' re at the end of file, return a null byte, which
	//	serves to mark the end of file.
	if (infile.eof())
        	c = '\0';
	//	If the next character is a newline, 
	//	increment the line count
	else if ((c = infile.get()) == '\n')
		linenum++;
	// Return the character converted to lower case
	return(tolower(c));
}

// ungettc() -	Returns a character to the file.  Uses ungetc and will
//		adjust line number count.
void	scanner::ungettc(char c)
{
	//	If it's a newline, decrement the line count;
	//	we haven't gone to the next line yet.
	if (c == '\n')
		--linenum;
	//	Put it back into the input stream.
	//infile.putback(c);
    infile.unget();

}
