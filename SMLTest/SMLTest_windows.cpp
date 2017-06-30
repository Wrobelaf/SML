// SMLTest.cpp : Defines the entry point for the console application.
//

#include "../StdAfx.h"
#include <windows.h>

#define VERSION	 "1.0"

// ---------------- Start of items in 'Useful' static library.
extern  int	 optmyerr;
extern  int	 optmyind;
extern  int	 optmyopt;
extern char *optmyarg;

extern int          getmyopt(int, char **, const char * const);
// ---------------- End of items in 'Useful' static library.

int main(int argc, char* argv[])
{
	int     ch;
	int     res = 0;				// Assume success
    filebuf ifb;
    filebuf ofb;
	ostream output_file (&ofb);
	istream input_file (&ifb);
	const string parse= "CcPpHhi:O:o:";			// command line options.
	string executable_name= "";
	string input_dir_name = "";	// cin or -f<file>
	string output_results_name= "";	// cout or -O<file>
	stringstream m(argv[0]);		// General message stream.
	string run_type="CompileSML";
	HANDLE hFind=INVALID_HANDLE_VALUE;

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
					input_dir_name = optmyarg;
					break;
				
				case 'o':
				case 'O':
					output_results_name = optmyarg;
					break;

				case 'c':
				case 'C':
					run_type="CompileSML";
					break;

				case 'p':
				case 'P';
					run_type="InterpretSML";
					break;
 
				default:
					break;
			}
		}

		if (input_dir_name.empty())
		{
			m << "Error: Input directory name is mandatory, is -i<dir name>";
			throw runtime_error (m.str());
		}

		if (output_results_name.empty())
		{
   			output_file.copyfmt(std::cout);
    		output_file.clear(std::cout.rdstate());
    		output_file.basic_ios<char>::rdbuf(std::cout.rdbuf());
		}
		else if (ofb.open (output_results_name.data() ,ios::out) == 0)
		{
			m << "Error: Unable to open output file " << output_results_name << ". " << strerror(errno);
			throw runtime_error (m.str());
		}

		// Now loop through each of the text files passing it to the Compiler or Interpritor.
		// Sending results to 'output_file'

		WIN32_FIND_DATA FindFileData;

		string input_test_files = input_dir_name + "\\*.bas";

		hFind = FindFirstFile(input_test_files.data(), &FindFileData);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			m << "Invalid File Handle. GetLastError reports " << GetLastError() << endl;
			throw runtime_error (m.str());
		}

		do
		{
			string f = input_dir_name + "\\" + FindFileData.cFileName;
			f.erase(f.length()-4);										// Remove extension

			output_file << "Processing: " << f << "  ";

			string command = "..\\" + run_type + "\\Release\\" + run_type + ".exe -i" + f + ".bas -o" + f + ".out";
			if (system(command.data()))
			{
				output_file << "FAILED : ";
			}
			else
			{
				output_file << "PASSED : ";
			}

			// For compilations, on a successful compile we must go ahead and run the compiled task.

			if (run_type == "CompileSML")
			{
				command = "..\\SML\\Release\\SML.exe -i" + f + ".out";
				if (system(command.data()))
				{
					output_file << ": FAILURE exit response";
				}
				else
				{
					output_file << ": SUCCESS exit response";
				}
			}
			else			// For interpretor, just copy the output file to the results file. Should contain
			{				// the test "RUN TIME PASS"
				// output_file << f + ".out"
			}
			output_file << endl;
		}
		while (FindNextFile(hFind,&FindFileData));
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

	if (hFind != INVALID_HANDLE_VALUE)
		FindClose(hFind);

	if (ifb.is_open())
		ifb.close();

	if (ofb.is_open())
		ofb.close();

	return res;
}

