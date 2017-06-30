// SML.cpp : Defines the entry point for the console application.
//

#include "../StdAfx.h"
#include "memory.h"
#include "machine.h"
#include "machine_state.h"

#ifdef UNIX
#include <sys/stat.h>
#include <unistd.h>
#endif

#define VERSION	 "1.0"

// ---------------- Start of items in 'Useful' static library.
extern  int	 optmyerr;
extern  int	 optmyind;
extern  int	 optmyopt;
extern char *optmyarg;

extern int          getmyopt(int, char **, const char * const);
// ---------------- End of items in 'Useful' static library.

using namespace SML;

int main(int argc, char* argv[])
{
	int     ch;
	int     res = 0;				// Assume success
	bool    out_to_consol=false;
    filebuf ifb;
    filebuf ofb;
	ostream output_file (&ofb);
	istream input_file (&ifb);
	const string parse= "Hhi:O:o:";	// command line options.
	string executable_name= "";
	string input_file_name = "";	// cin or -f<file>
	string output_file_name= "";	// cout or -O<file>
	stringstream m(argv[0]);		// General message stream.
	memory *mem = NULL;

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


		// Setup the input stream either from cin or if -f supplied then -f<file>

		if (input_file_name.empty())
		{
			input_file.copyfmt(std::cin);
			input_file.clear(std::cin.rdstate());
			input_file.basic_ios<char>::rdbuf(std::cin.rdbuf());

			// input stream may be pipe, in which case process the same way as file input.
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
			if (IsAPipe)
			{
				mem = new memory (input_file);
			}
			else
			{
				mem = new memory (input_file,cout);
			}
		}
		else
		{
			if (ifb.open (input_file_name.data() ,ios::in) == 0)
			{
				m << "Error: Unable to read input file " << input_file_name << ". " << strerror(errno);
				throw runtime_error (m.str());
			}
			mem = new memory (input_file);
		}

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

		machine_state state;
		machine Machine(mem,state);
		Machine.Execute(output_file);

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

	delete mem;

	if (ifb.is_open())
		ifb.close();

	if (ofb.is_open())
		ofb.close();

	return res;
}

