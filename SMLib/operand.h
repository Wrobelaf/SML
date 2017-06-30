/*
	operand.h

	Class holding the operand part of SML instruction words.

*/

#ifndef __OPERAND_
#define __OPERAND_

namespace SML {

class Operand
{
public:

	Operand(unsigned int);
	~Operand();

	unsigned int operator*(void) const;
	Operand &operator++();
	Operand operator++(int);
	int GetSignedValue(void) const;
	void MakeAsSignedValue(void);
	void SetOperand(const unsigned int);
	friend ostream &operator<<(ostream &, const Operand &);
	bool debug(void) const;
	bool debug(const bool pb);

private:

	int op;
	bool dbug;

};

}			// Namespace SML

#endif		// __OPERAND_

