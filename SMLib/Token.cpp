/*
	Token.cpp

	A token of an input numeric expression.

*/

#include "../StdAfx.h"
#include "common_sml_header.h"
#include "SymbolVariable.h"
#include "Token.h"

#include <math.h>

extern int NUM_INVALID;
extern unsigned int GetNum (const char * const,double &);
extern int normalise (double,unsigned int &,bool &,bool &);
extern double round_to (double,int);

namespace SML {

const Token::v_elements Token::valid_element[] = {  {ADD,		"+",     OPERATOR,   4, true,	"ADD"},
												    {SUBTRACT,	"-",     OPERATOR,   4, false,	"SUBTRACT"},
													{MULTIPLY,	"*",     OPERATOR,   5, false,	"MULTIPLY"},
													{DIVIDE,	"/",     OPERATOR,   5, false,	"DIVIDE"},
													{POWER,		"^",     OPERATOR,   6, false,	"POWER"},
													{MODULUS,	"%",     OPERATOR,   5, false,	"MODULUS"},
													{OPEN_P,  	"(",     NOTHING,    0, false,	"("},
													{CLOSE_P,	")",     NOTHING,    0, false,	")"},
													{EQUAL,  	"=",     OPERATOR,   3, true,	"="},
													{LT,  	    "<",     OPERATOR,   3, true,	"<"},
													{GT,     	">",     OPERATOR,   3, true,	">"},
													{COMMA,  	",",     NOTHING,    0, true,	","},
													{N_EQUAL,  	"<>",    OPERATOR,   3, true,	"<>"},
													{LT_E,  	"<=",    OPERATOR,   3, true,	"<="},
													{GT_E,    	">=",    OPERATOR,   3, true,	">="},
													{OR,        "OR",    OPERATOR,   1, false,	"OR"},
													{AND,       "AND",   OPERATOR,   2, false,	"AND"},
													{REM,       "REM",   COMMAND,    0, false,	"REMARK"},
													{INPUT,     "INPUT", COMMAND,    0, false,	"INPUT"},
													{LET,       "LET",   COMMAND,    0, false,	"LET"},
													{PRINT,     "PRINT", COMMAND,    0, false,	"PRINT"},
													{GOTO,      "GOTO",  COMMAND,    0, false,	"GOTO"},
													{IF,        "IF",    COMMAND,    0, false,	"IF"},
													{END,       "END",   COMMAND,    0, false,	"END"},				// halt run time execution
													{EXIT,      "EXIT",  NOTHING,    0, false,	"EXIT"},			// command to end input program
													{VAR,		"",      NOTHING,    0, false,	"VARIABLE"},		// retain position in array
													{STRING,	"",      NOTHING,    0, false,	"STRING"},			// retain position in array
													{NUMBER,	"",      NOTHING,    0, false,	"NUMBER"},			// retain position in array
													{INVALID,	"",      NOTHING,    0, false,	"INVALID"} };

const int Token::NUM_ELEMENTS = sizeof(Token::valid_element)/sizeof(Token::valid_element[0]);


Token::Token(Token::elements pi,const string& ps,const unsigned int pbw) :
type(0),
n(0.0),
s(ps),
bw(pbw),
norm(0),
power(0),
negative(false)
{
	for (int i = 0 ; i < NUM_ELEMENTS ; i++)
	{
		if (valid_element[i].elem == pi)
		{
			type = &valid_element[i];
			break;
		}
	}
	if (!type)
		type = &valid_element[NUM_ELEMENTS-1];			// Address the INVALID instruction
}

Token::Token(const double pn,const unsigned int pbw) :
type(&valid_element[NUM_ELEMENTS-2]),
n(pn),
s(""),
bw(pbw),
norm(0),
power(0),
negative(false)
{
	// Used to initialise when item is a digit and we have its value.
	bool nan;
	power = normalise (n,norm,negative,nan);

	if (nan)
	{
		stringstream m;
		m << "Error: Unrcecognised number. " << n << endl;
		throw runtime_error (m.str());
	}
}

Token::Token(const string &ps,bool analyse,const unsigned int pbw) :
type(&valid_element[NUM_ELEMENTS-3]),					// Assume constant STRING
n(0.0),
s(ps),
bw(pbw),
norm(0),
power(0),
negative(false)
{
	// If 'analyse' is false then 's' is a constant string and must not be analysed.

	bool found = false;
	if (analyse)
	{
		// Analyses 's' and sets the instance of this class to the type.
		// Any problems and leave the type as INVALID.

		if (ps.length() > 0)
		{
			for (int i = 0 ; i < NUM_ELEMENTS ; i++)
			{
				if ((ps.length() == strlen(valid_element[i].lex)) &&
					(strncasecmp(ps.data(),valid_element[i].lex,ps.length()) == 0))
				{
					type = &valid_element[i];
					found = true;
					break;
				}
			}
		}

		// Ok - so not a keyword or part of a numeric expression, all that's left is a number or string.

		if (!found)
		{
			if (GetNum (ps.data(),n) != NUM_INVALID)
			{
				type = &valid_element[NUM_ELEMENTS-2];			// Number

				// Work out the normalised form of the number.

				bool nan;
				power = normalise (n,norm,negative,nan);

				if (nan)
				{
					stringstream m;
					m << "Error: Unrcecognised number: " << n << endl;
					throw runtime_error (m.str());
				}
			}
			else // Empty string is INVALID everything else is a VARIABLE
			{
				type = (ps.length() > 0) ? &valid_element[NUM_ELEMENTS-4] : &valid_element[NUM_ELEMENTS-1];
			}
		}
	}
}

Token::Token(const Token & rhs) :
type(rhs.type),
n(rhs.n),
s(rhs.s),
bw(rhs.bw),
norm(rhs.norm),
power(rhs.power),
negative(rhs.negative)
{
		// Copy Constructor.
}

Token::Token(const Token * rhs) :
type(rhs->type),
n(rhs->n),
s(rhs->s),
bw(rhs->bw),
norm(rhs->norm),
power(rhs->power),
negative(rhs->negative)
{
		// Copy Constructor.
}

Token::Token(void) :
type(&valid_element[NUM_ELEMENTS-1]),
n(0),
s(""),
bw(0),
norm(0),
power(0),
negative(false)
{
	// Null constructor, setup instance to be an INVALID token.
}

Token::~Token()
{
}

Token& Token::operator=(const Token &rhs)
{
    if (this != &rhs)      // Same object?
	{
		type = rhs.type;
		n    = rhs.n;
		s    = rhs.s;
		bw   = rhs.bw;
		norm = rhs.norm;
		power= rhs.power;
		negative = rhs.negative;
	}
    return *this;
}

// ----------------------------------------------------------------
// Arithmetic operations
// ----------------------------------------------------------------

void Token::TypeCheck(const Token &op1,const string &op, const Token &op2) const
{
	stringstream m;

	if ((*op1 != Token::NUMBER) && (*op1 != Token::STRING))
	{
		m << "Error Attempting constant folding on a non constant type: " << *op1 << endl;
		throw runtime_error (m.str());
	}

	if ((*op2 != Token::NUMBER) && (*op2 != Token::STRING))
	{
		m << "Error Attempting constant folding on a non constant type: " << *op2 << endl;
		throw runtime_error (m.str());
	}

	if (*op1 != *op2)
	{
		m << "Error Attempting to operate on different constant types: " << *op1 << " " << op << " " << *op2 << endl;
		throw runtime_error (m.str());
	}
}

Token& Token::operator+=(const Token &rhs)
{
	TypeCheck (this,"+",rhs);

	switch (type->elem)
	{
	case NUMBER:
	{
		bool nan;
		n += rhs.GetNumber();
		n = round_to(n,ROUND_TO);

		// Re-normalise 'n' to setup all the other variables.
		power = normalise (n,norm,negative,nan);

		if (nan)
		{
			stringstream m;
			m << "Error: Unrcecognised number: " << n << endl;
			throw runtime_error (m.str());
		}
		break;
	}
	case STRING:
		s += rhs.GetString();
		break;
	}
	return *this;
}

const Token Token::operator+(const Token &rhs)
{
	Token t = *this;
	t += rhs;
	return t;
}

Token& Token::operator-=(const Token &rhs)
{
	TypeCheck (this,"-",rhs);

	switch (type->elem)
	{
	case NUMBER:
	{
		bool nan;
		n -= rhs.GetNumber();
		n = round_to(n,ROUND_TO);

		// Re-normalise 'n' to setup all the other variables.
		power = normalise (n,norm,negative,nan);

		if (nan)
		{
			stringstream m;
			m << "Error: Unrcecognised number: " << n << endl;
			throw runtime_error (m.str());
		}
		break;
	}
	case STRING:
	{
		stringstream m;
		m << "Subtraction operator has no meaning when applied to STRING types." << endl;
		throw runtime_error (m.str());
		break;
	}
	}
	return *this;
}

const Token Token::operator-(const Token &rhs)
{
	Token t = *this;
	t -= rhs;
	return t;
}

Token& Token::operator*=(const Token &rhs)
{
	TypeCheck (this,"*",rhs);

	switch (type->elem)
	{
	case NUMBER:
	{
		bool nan;
		n *= rhs.GetNumber();
		n = round_to(n,ROUND_TO);

		// Re-normalise 'n' to setup all the other variables.
		power = normalise (n,norm,negative,nan);

		if (nan)
		{
			stringstream m;
			m << "Error: Unrcecognised number: " << n << endl;
			throw runtime_error (m.str());
		}
		break;
	}
	case STRING:
	{
		stringstream m;
		m << "Multiplication operator has no meaning when applied to STRING types." << endl;
		throw runtime_error (m.str());
		break;
	}
	}
	return *this;
}

const Token Token::operator*(const Token &rhs)
{
	Token t = *this;
	t *= rhs;
	return t;
}

Token& Token::operator/=(const Token &rhs)
{
	TypeCheck (this,"/",rhs);

	switch (type->elem)
	{
	case NUMBER:
	{
		bool nan;
		if (rhs == 0.0)
		{
			stringstream m;
			m << "Zero divide.";
			throw runtime_error (m.str());
		}

		n /= rhs.GetNumber();
		n = round_to(n,ROUND_TO);

		// Re-normalise 'n' to setup all the other variables.
		power = normalise (n,norm,negative,nan);

		if (nan)
		{
			stringstream m;
			m << "Error: Unrcecognised number: " << n << endl;
			throw runtime_error (m.str());
		}
		break;
	}
	case STRING:
	{
		stringstream m;
		m << "Division operator has no meaning when applied to STRING types." << endl;
		throw runtime_error (m.str());
		break;
	}
	}
	return *this;
}

const Token Token::operator/(const Token &rhs)
{
	Token t = *this;
	t /= rhs;
	return t;
}

Token& Token::operator^=(const Token &rhs)
{
	TypeCheck (this,"^",rhs);

	switch (type->elem)
	{
	case NUMBER:
	{
		bool nan;

		n = round_to(pow(n,rhs.GetNumber()),ROUND_TO);

		// Re-normalise 'n' to setup all the other variables.
		power = normalise (n,norm,negative,nan);

		if (nan)
		{
			stringstream m;
			m << "Error: Unrcecognised number: " << n << endl;
			throw runtime_error (m.str());
		}
		break;
	}
	case STRING:
	{
		stringstream m;
		m << "Power operator has no meaning when applied to STRING types." << endl;
		throw runtime_error (m.str());
		break;
	}
	}
	return *this;
}

const Token Token::operator^(const Token &rhs)
{
	Token t = *this;
	t ^= rhs;
	return t;
}

Token& Token::operator%=(const Token &rhs)
{
	TypeCheck (this,"%",rhs);

	switch (type->elem)
	{
	case NUMBER:
	{
		bool nan;
		if (rhs == 0.0)
		{
			stringstream m;
			m << "Zero divide.";
			throw runtime_error (m.str());
		}

		int i = (int) n;
		i %= (int) rhs.GetNumber();
		n = round_to((double) i,ROUND_TO);

		// Re-normalise 'n' to setup all the other variables.
		power = normalise (n,norm,negative,nan);

		if (nan)
		{
			stringstream m;
			m << "Error: Unrcecognised number: " << n << endl;
			throw runtime_error (m.str());
		}
		break;
	}
	case STRING:
	{
		stringstream m;
		m << "Modulus operator has no meaning when applied to STRING types." << endl;
		throw runtime_error (m.str());
		break;
	}
	}
	return *this;
}

const Token Token::operator%(const Token &rhs)
{
	Token t = *this;
	t %= rhs;
	return t;
}

// ----------------------------------------------------------------
// DeReferencing
// ----------------------------------------------------------------

Token::elements Token::operator*(void) const
{
	// Dereferencing returns the type of this instance.
	return type->elem;
}

// ----------------------------------------------------------------
// Comparison operations
// ----------------------------------------------------------------

bool Token::operator==(const Token& rhs) const
{
	if (type->elem == *rhs)
	{
		switch (type->elem)
		{
		case NUMBER:
			return (*this == rhs.n);		// must use Token == double operator below so as to only
											// compare to within the accuracy of this compiler/interpretor.
		case STRING:
		case VAR:
			return (s == rhs.GetString());

		default:
			return false;
		}
	}
	return false;
}

bool Token::operator!=(const Token& rhs) const
{
	return (!(*this == rhs));
}

bool Token::operator< (const Token& rhs) const
{
	if (type->elem == *rhs)
	{
		switch (type->elem)
		{
		case NUMBER:
			return (n < rhs.n);

		case STRING:
		case VAR:
			return (s < rhs.GetString());

		default:
			return false;
		}
	}
	return false;
}

bool Token::operator<=(const Token& rhs) const
{
	if (type->elem == *rhs)
	{
		switch (type->elem)
		{
		case NUMBER:
			return (n <= rhs.n);

		case STRING:
		case VAR:
			return (s <= rhs.GetString());

		default:
			return false;
		}
	}
	return false;
}

bool Token::operator>=(const Token& rhs) const
{
	if (type->elem == *rhs)
	{
		switch (type->elem)
		{
		case NUMBER:
			return (n >= rhs.n);

		case STRING:
		case VAR:
			return (s >= rhs.GetString());

		default:
			return false;
		}
	}
	return false;
}

bool Token::operator>(const Token& rhs) const
{
	if (type->elem == *rhs)
	{
		switch (type->elem)
		{
		case NUMBER:
			return (n > rhs.n);

		case STRING:
		case VAR:
			return (s > rhs.GetString());

		default:
			return false;
		}
	}
	return false;
}

bool Token::operator==(const int rhs) const
{
	return (type->presidence == rhs);
}

bool Token::operator!=(const int rhs) const
{
	return (type->presidence != rhs);
}

bool Token::operator==(const double rhs) const
{
	return (fabs(n-rhs) < (double)(1.0/ROUND_TO));
}

bool Token::operator!=(const double rhs) const
{
	return (fabs(n-rhs) > (double)(1.0/ROUND_TO));
}

bool Token::operator==(const Token::elements rhs) const
{
	return (type->elem == rhs);
}

bool Token::operator!=(const Token::elements rhs) const
{
	return (type->elem != rhs);
}

bool Token::operator<=(const int rhs) const
{
	return (type->presidence <= rhs);
}

bool Token::operator>=(const int rhs) const
{
	return (type->presidence >= rhs);
}

const double Token::GetNumber (void) const
{
	return n;
}

const unsigned int Token::GetNormalisedNumber (int &pow,bool &neg) const
{
	pow = power;
	neg = negative;
	return norm;
}

const string & Token::GetString (void) const
{
	return s;
}

const char* const Token::GetId(void) const
{
	return type->id;
}

const unsigned int Token::GetBw(void) const
{
	return bw;
}

const int Token::GetPresidence(void) const
{
	return type->presidence;
}

const bool Token::isOperator(void) const
{
	return (type->elem_type == Token::OPERATOR);
}

const bool Token::isCommand(void) const
{
	return (type->elem_type == Token::COMMAND);
}

const bool Token::StringOp(void) const
{
	return type->string_op;
}

Token::elements Token::GetBaseType(void) const
{
	switch (type->elem)
	{
	case NUMBER:
		return NUMBER;

	case STRING:
		return STRING;

	case VAR:
	{
		bool found;
		SymbolVariable &v = SymbolVariable::Find(this,&found);
		if (!found)
		{
			return INVALID;
		}
		return *(v.GetAssignedTo());
	}
	default:
		return INVALID;
	}
}

ostream &operator<<(ostream &out, const Token &ms)
{
	out.width(10);
	out.fill(' ');
	out << left << ms.GetId();

	if (*ms == Token::NUMBER)
	{
		out.width(15);
		out.precision(9);
		out << right << ms.GetNumber();
	}
	else if ((*ms == Token::STRING) || (*ms == Token::VAR))
	{
		out << ms.GetString();
	}
	return out;
}

}		// End Namespace SML
