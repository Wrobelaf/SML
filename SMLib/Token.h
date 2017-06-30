
/*
	Token.h

	A token of a numeric expresion analyser.

*/

#ifndef __Token_
#define __Token_

namespace SML {

class Token
{
public:

	enum elements	{	NUMBER	= 10,
						ADD	    = 11,
						SUBTRACT= 12,
						MULTIPLY= 13,
						DIVIDE  = 14,
						POWER	= 20,
						MODULUS = 21,
						OPEN_P  = 31,
						CLOSE_P = 32,
						COMMA   = 33,
						REM     = 40,
					    INPUT   = 41,
						LET     = 42,
						PRINT   = 43,
						GOTO    = 44,
						IF      = 45,
						END     = 46,
						STRING  = 47,
						VAR     = 48,
						EQUAL   = 50,
						N_EQUAL = 51,
						LT      = 52,
						LT_E    = 53,
						GT      = 54,
						GT_E    = 55,
						OR      = 60,
						AND     = 61,
						EXIT    = 98,
						INVALID = 99   };

	enum element_type {	OPERATOR, COMMAND, NOTHING };

	Token(elements,const string& = "", const unsigned int = 0);
	Token(const double,const unsigned int = 0);
	Token(const string &,bool = true,const unsigned int = 0);
	Token(const Token &);
	Token(const Token *);
	Token(void);
	~Token();

	Token& operator=(const Token &);
	Token& operator+=(const Token &);
	const Token operator+(const Token &);
	Token& operator-=(const Token &);
	const Token operator-(const Token &);
	Token& operator*=(const Token &);
	const Token operator*(const Token &);
	Token& operator/=(const Token &);
	const Token operator/(const Token &);
	Token& operator^=(const Token &);
	const Token operator^(const Token &);
	Token& operator%=(const Token &);
	const Token operator%(const Token &);
	elements operator*(void) const;

	bool operator==(const Token&) const;
	bool operator!=(const Token&) const;
	bool operator< (const Token&) const;
	bool operator<=(const Token&) const;
	bool operator>=(const Token&) const;
	bool operator> (const Token&) const;

	bool operator==(const int) const;
	bool operator!=(const int) const;
	bool operator<=(const int) const;
	bool operator>=(const int) const;

	bool operator==(const double) const;
	bool operator!=(const double) const;

	bool operator==(const Token::elements) const;
	bool operator!=(const Token::elements) const;

	elements GetBaseType(void) const;
	const double GetNumber (void) const;
	const unsigned int GetNormalisedNumber (int &,bool &) const;
	const string& GetString (void) const;
	const char * const GetId(void) const;
	const unsigned int GetBw(void) const;
	const int GetPresidence(void) const;
	const bool isOperator(void) const;
	const bool isCommand(void) const;
	const bool StringOp(void) const;

	friend ostream &operator<<(ostream &, const Token &);

private:

	static const int NUM_ELEMENTS;

	static const struct v_elements {
		elements elem;
		char *lex;
		element_type elem_type;
		int  presidence;
		bool string_op;
		char *id;
	} valid_element[];

	const v_elements   *type;
	double			   n;
	string             s;
	unsigned int	   bw;			// Any extra information pertaining to this token.

	unsigned int       norm;
	bool			   negative;
	int                power;

	void TypeCheck(const Token &,const string &,const Token &) const;

};

}			// End namespace SML

#endif		// __Token_
