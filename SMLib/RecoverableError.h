/*
	RecoverableError.h

	Exception class to handle recoverable errors, say syntax error on a particular line.

*/

#ifndef __RecoverableError_
#define __RecoverableError_

#include <string>

using namespace std;

class RecoverableError
{
public:

	RecoverableError(string m) { message = m; };
	void Report(void) { cerr << message; };

private:

	string message;

};

#endif		// __RecoverableError_
