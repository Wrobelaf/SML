/*
	Main.cpp

	Class holding the Main of the SML machine

*/

#include "Main.h"
#include "RecoverableError.h"

#ifdef UNIX
#include <sys/stat.h>
#include <unistd.h>
#endif

#define VERSION	 "1.0"

// ---------------- Start of items in 'Useful' static library.

extern int   NUM_INVALID;
extern int   NUM_DEC;
extern int   NUM_HEX;
extern int   NUM_REAL;
extern int   optmyerr;
extern int   optmyind;
extern int   optmyopt;
extern char *optmyarg;

extern int getmyopt(int, char **, const char * const);
// ---------------- End of items in 'Useful' static library.

namespace SML {


Main::Main (int pargc,char **pargv) :
argc(pargc),
argv(pargv)
{
}

Main::~Main ()
{
}

void Main::Emit (Token*& t,string& str,int& n,double& frac,int& sign,unsigned int& bw)
{
	if (t != 0)
	{
		if (*t == Token::NUMBER)
		{
			tokens.push_back(Token((n+frac)*sign,bw));
			n    = 0;
			frac = 0.0;
			sign = 1;
			bw   = 0;
		}
		else if (*t == Token::STRING)
		{
			tokens.push_back(Token(str,false));			// Constant string constructor
			str.erase(str.begin(),str.end());
		}
		else if (*t == Token::VAR)
		{
			tokens.push_back(Token(str));				// Analyse variable as it might be a keyword
			str.erase(str.begin(),str.end());
		}
		else
		{
			tokens.push_back(t);
		}
		delete t;
		t = 0;
	}
}

bool Main::Tokenise(int &line_no,string &s)
{
	// Convert input test to a tokenised array.
	// Return 'true' if there is something to process, On 'false' then there has ever been a error or its a blank line
	// or comment line that has not generated a token array to process.

										//  \0   \t   "   +   -   *  /   \   ,   .   ^   %   (   )   =   <   >   ab   n  rest
	static const unsigned char fsm[][20] ={{100,  0,  7,  1,  2, 13, 13, 97, 13, 13, 13, 13, 13, 13, 13, 14, 15, 11,  3, 95}, // 0
										   {100,  0,  7,  1,  2, 13, 13, 97, 13, 13, 13, 13, 13, 13, 13, 14, 15, 11,  3, 95}, // 1
										   {100,  0,  7,  1,  2, 13, 13, 97, 13, 13, 13, 13, 13, 13, 13, 14, 15, 11,  3, 95}, // 2
										   {100,  0,  7,  1,  2, 13, 13, 97, 13,  5, 13, 13, 13, 13, 13, 14, 15, 97,  4, 95}, // 3
										   {100,  0,  7,  1,  2, 13, 13, 97, 13,  5, 13, 13, 13, 13, 13, 14, 15, 11,  4, 95}, // 4
										   { 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  6, 95}, // 5
										   {100,  0,  7,  1,  2, 13, 13, 97, 13, 99, 13, 13, 13, 13, 13, 14, 15, 99,  6, 95}, // 6
										   { 98,  9,  0,  9,  9,  9,  9,  8,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9}, // 7
										   { 98, 18,  9, 18, 18, 18, 18, 10, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18}, // 8
										   { 98,  9,  0,  9,  9,  9,  9,  8,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9}, // 9
										   { 98,  9,  0,  9,  9,  9,  9,  8,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9}, // 10
										   {100,  0, 96,  1,  2, 13, 13, 97, 13, 97, 13, 13, 13, 13, 13, 14, 15, 12, 12, 95}, // 11
										   {100,  0, 96,  1,  2, 13, 13, 97, 13, 97, 13, 13, 13, 13, 13, 14, 15, 12, 12, 95}, // 12
										   {100,  0,  7,  1,  2, 13, 13, 97, 13, 97, 13, 13, 13, 13, 13, 14, 15, 11,  3, 95}, // 13
										   {100,  0,  7,  1,  2, 13, 13, 97, 13, 97, 13, 13, 13, 13, 16, 14, 19, 11,  3, 95}, // 14
										   {100,  0,  7,  1,  2, 13, 13, 97, 13, 97, 13, 13, 13, 13, 17, 14, 15, 11,  3, 95}, // 15
										   {100,  0,  7,  1,  2, 13, 13, 97, 13, 97, 13, 13, 13, 13, 13, 14, 15, 11,  3, 95}, // 16
										   {100,  0,  7,  1,  2, 13, 13, 97, 13, 97, 13, 13, 13, 13, 13, 14, 15, 11,  3, 95}, // 17
										   { 98,  9,  0,  9,  9,  9,  9,  8,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9, 95}, // 18
										   {100,  0,  7,  1,  2, 13, 13, 97, 13, 97, 13, 13, 13, 13, 13, 14, 15, 11,  3, 95}};// 19
	bool ret   = true;
	Token *t   = 0;
	int n      = 0;
	double d   = 0.0;
	double frac= 0.0;
	int pow    = 10;
	int sign   = 1;
	int state  = 0;
	int idx    = 0;
	unsigned int bw = 0;
	string str;
	char c;

	do
	{
		unsigned int fold = (idx == s.length()) ? 0 : 1;
		if (fold)
		{
			c = s[idx++];
			fold = ((c == ' ') || (c == '\t')) ? 1 : (isalpha(c)) ? 17 : (isdigit(c)) ? 18 : 0;
			if (!fold)
			{
				string leximes("\"+-*/\\,.^%()=<>");
				fold = leximes.find(c);
				fold = (fold == string::npos) ? 19 : fold+2;
			}
		}

		state = fsm[state][fold];
		switch (state)
		{
		case 0:								// Eat white spcace
			Emit (t,str,n,frac,sign,bw);
			break;
		case 1:								// '+'
		case 2:								// '-'
		case 13:							// */,^%()=
		case 14:							// <
		case 15:							// >
			Emit (t,str,n,frac,sign,bw);
			t = new Token(s.substr(idx-1,1));
			break;
		case 16:							// Convert <  to  <=
		case 17:							// Convert >  to  >=
		case 19:							// Convert <  to  <>
			delete t;
			t = new Token(s.substr(idx-2,2));
			break;
		case 3:								// First digit of a number, check for +n or -n
			if (t == 0)
			{
				t = new Token(Token::NUMBER);
				sign = 1;
			}
			else if (*t == Token::SUBTRACT)		// Convert '-' token to -ve number token.
			{
				delete t;
				t = new Token(Token::NUMBER);
				sign = -1;
			}
			else if (*t == Token::ADD)			// Convert '+' token to +ve number token.
			{
				delete t;
				t = new Token(Token::NUMBER);
				sign = 1;
			}
			else								// Other token types, just persist them and start with the number.
			{
				Emit (t,str,n,frac,sign,bw);
				t = new Token(Token::NUMBER);
				sign = 1;
			}

			// fall though

		case 4:
			n = (n*10)+(c-'0');
			break;

		case 5:									// got the '.' of a real number
			pow = 10;
			bw |= NUM_REAL;
			break;

		case 6:									// Got the fractioanl part of a real number
			frac += ((double)(c-'0'))/pow;
			pow *= 10;
			break;

		case 7:									// Got the " that indicates start of a constant string
			Emit (t,str,n,frac,sign,bw);
			t = new Token(Token::STRING);

			// Fall through

		case 8:									// Got a trip chaaracter '\' inside a constant string
			break;

		case 9:									// Got a normal char to make part of a constant string
			str += c;
			break;
		case 10:								// Get a second trip character after the first one.
			str += '\\';						// Convert the trip charater to a normal character.
			break;
		case 18:								// Got a normal character after a trip char so output both
			str = str + '\\' + c;
			break;
		case 11:								// Start of a variable like abc
			Emit (t,str,n,frac,sign,bw);
			t = new Token(Token::VAR);

			// Fall through

		case 12:								// Continuation of a variable
			str += c;
			break;

		case 95:
			cout << "Unrecognised character '" << c << "' at line " << line_no;
			ret = false;
			break;
		case 96:
			cout << "Inproperly terminated varable '" << str << "' at line " << line_no;
			ret = false;
			break;
		case 97:
			cout << "Syntax error at line " << line_no;
			ret = false;
			break;
		case 98:
			cout << "Inproperly terminated constant string at line " << line_no;
			ret = false;
			break;
		case 99:
			cout << "Inproperly terminated number at line " << line_no;
			ret = false;
			break;
		case 100:								// End of input stream, emit any remaining token.
			Emit (t,str,n,frac,sign,bw);
			break;
		}

	} while (state < 90);

	line_no+=10;
	return (ret) ? (tokens.size() > 0) : false;
}

const vector<Token>& Main::GetTokens(void) const
{
	return tokens;
}

int Main::Run (ostream & out,istream & in)
{
	// Compile program 'in' from file.
	// 'out' is for the output of the compiled program, not for communication with user.

	bool errors = false;
	int ret  = 0;
	int line = 1;
	stringstream m;

	do {

		string s;

		getline(in,s);

		if (!in.fail())
		{
			if (Tokenise(line,s))
			{
				try {
					ret = Pass1 (tokens,s);
				}
				catch (RecoverableError e)
				{
					errors = true;
					e.Report ();
				}
			}
			tokens.clear();
		}
		else if (!in.eof())
		{
			m << "Stream input failour at line '" << line << "'";
			throw runtime_error (m.str());
		}

	} while (!in.eof() && !ret);

	if (!errors)
	{
		// Now fixup unresolved target addresses

		try {
			Pass2 (out);
		}
		catch (RecoverableError e)
		{
			errors = true;
			e.Report ();
		}

		if (!errors)
			Print (out);
	}
	return 0;
}

int Main::Run (ostream & out,bool pipe)
{
	// Read program in from console (pipe=false) so chivvy the user on with comments, or pipe (pipe=true),
	// in which case don't chivvy them.
	// 'out' is for the output of the compiled program, not for communication with user.

	bool errors = false;
	int ret = 0;
	int line = 10;

	if (!pipe)
	{
		cout << "*** Welcome to Simpleton! ***" << endl;
		cout << "*** Please enter your BASIC program one line ***" << endl;
		cout << "*** at a time. (Line numbering is done for you) ***" << endl;
		cout << "*** Finish by typing EXIT. ***" << endl << endl;
	}

	do {

		if (!pipe)
		{
			cout.fill ('0');
			cout.width (2);
			cout << line << " ? " << flush;
		}

		string s;

		getline(cin,s);

		if (!cin.fail())
		{
			if (!pipe)		// If from terminal add line numbers.
			{
				stringstream ss;
				string tmp;
				ss << line;
				ss >> tmp;
				s = tmp+" "+s;
			}
			if (Tokenise(line,s))
			{
				try {
					ret = Pass1 (tokens,s);
				}
				catch (RecoverableError e)
				{
					errors = true;
					e.Report ();
				}
			}
			tokens.clear();
		}
		else if (!cin.eof())
		{
			if (pipe)
			{
				stringstream m;
				m << "Stream input failour.";
				throw runtime_error (m.str());
			}
			cout << "Input stream has encountered an error, retry." << endl;
			cin.clear();
			cin.ignore(INT_MAX,'\n');
		}
	} while (!cin.eof() && !ret);

	if (!errors)
	{
		// Now fixup unresolved target addresses.

		try {
			Pass2 (out);
		}
		catch (RecoverableError e)
		{
			errors = true;
			e.Report ();
		}

		if (!errors)
			Print (out);
	}
	return 0;
}

int Main::Run(void)
{
	int     ch;
	int     res = 0;				// Assume success
	bool    out_to_consol=false;
	filebuf ifb;
	filebuf ofb;
	ostream output_file (&ofb);
	istream input_file (&ifb);
	const string parse= "dHhi:O:o:";			// command line options.
	string executable_name= "";
	string input_file_name = "";	// cin or -f<file>
	string output_file_name= "";	// cout or -O<file>
	stringstream m(argv[0]);		// General message stream.
	bool debug = false;

	// Find filename part of exetutable. Used in constructing 'invalid argument' error messages.

	ch = m.str().find_last_of("\\/");	// DOS or unix
	executable_name = (ch == string::npos) ? m.str() : m.str().substr(ch+1);
	m.str("");

	try
	{
		// Analyse command line arguments.
		// First get all the operands and qualifiers

		while ((ch = getmyopt(argc, argv, parse.data())) != -1)
		{
			switch (ch)
			{
				case 'h':
				case 'H':
					m << executable_name << "\tVersion " << VERSION << " Built " << __DATE__ << "\n"
					  << "\t\t1 - failure\n";
					throw runtime_error (m.str());

				case 'd':
					debug = true;
					break;

				case 'i':
					input_file_name = optmyarg;
					break;

				case 'o':
				case 'O':
					output_file_name = optmyarg;
					break;

				default:
					break;
			}
		}

		SetOptions(debug);

		if (output_file_name.empty())
		{
    		output_file.copyfmt(std::cout);
    		output_file.clear(std::cout.rdstate());
    		output_file.basic_ios<char>::rdbuf(std::cout.rdbuf());
		}
		else
		{
			if (ofb.open (output_file_name.data() ,ios::out) == 0)
			{
				m << "Error: Unable to open output file " << output_file_name << ". " << strerror(errno);
				throw runtime_error (m.str());
			}
		}

		// Setup the input stream either from cin or if -f supplied then -f<file>

		if (input_file_name.empty())
		{
			// Check stdin as it may be a pipe
			bool IsAPipe = false;
#ifdef UNIX
			struct stat info;
			if (!fstat (0, &info))
			{
				IsAPipe = (info.st_mode & S_IFIFO);
			}
#else
#error "Getting PIPE info from the operation system, has to be implemented."
#endif
			res = Run (output_file,IsAPipe);
		}
		else
		{
			if (ifb.open (input_file_name.data() ,ios::in) == 0)
			{
				m << "Error: Unable to read input file " << input_file_name << ". " << strerror(errno);
				throw runtime_error (m.str());
			}

			res = Run (output_file,input_file);
		}
	}
	catch ( runtime_error &e )
	{
		cerr << e.what() << endl;
		res = 1;
	}
	catch ( invalid_argument &e )
	{
		cerr << e.what() << " ('" << executable_name << " -h' for usage)" << endl;
		res = 1;
	}
	catch ( bad_alloc &e )			// Failure from 'new'
	{
		cerr << "Error: Exception (memory) " << e.what() << endl;
		res = 1;
	}
	catch ( ... )
	{
		cerr << "Error: Unhandled exception!" << endl;
		res = 1;
	}


	if (ifb.is_open())
		ifb.close();

	if (ofb.is_open())
		ofb.close();

	return res;
}

ostream &operator<<(ostream &out,Main &p)
{
	vector<Token>::const_iterator j;

	for (j=p.GetTokens().begin() ; j != p.GetTokens().end() ; j++)
	{
		cout << *j << endl;
	}
	p.Print(out);
	return out;
}

}			// End namespace SML
