/*
	Eval.cpp

	Class to evaluate a reverse polish stack.
	Either for compilation or interpritation.

*/

#include "../StdAfx.h"
#include "RecoverableError.h"
#include "Eval.h"

namespace SML {

Eval::Eval (vector<Token>&p,int po) :
prefix(p),
offset(po)
{
}

Eval::~Eval (void)
{
}

int Eval::Evaluate (void)
{
	bool allowUnary=false;
	stringstream m;
	int res=0;
	int e;
	stack<Token> stk;
	vector<Token>::iterator i;
	Token tos;

	// copy(prefix.begin()+offset, prefix.end(), ostream_iterator<string>(cout, ", "));

	stk.push(Token(Token::OPEN_P));
	prefix.push_back(Token(Token::CLOSE_P));
	i = prefix.begin()+offset;

	while (!stk.empty())
	{
		if (i == prefix.end())			// If not got an empty stack but reached EOT somethings wrong.
		{
			m << "INVALID expression ";
			copy(prefix.begin(), prefix.end(), ostream_iterator<Token>(m, " "));
			throw RecoverableError (m.str());
		}

		Token t(*i);

		switch (*t)
		{
		default:
		case Token::INVALID:

			m << "Unrecognised item " << t << " from expression ";
			copy(prefix.begin(), prefix.end(), ostream_iterator<Token>(m, " "));
			throw RecoverableError (m.str());

		case Token::STRING:
		case Token::NUMBER:
		case Token::VAR:

			postfix.push_back(t);
			allowUnary=false;
			break;

		case Token::OPEN_P:

			stk.push(t);
			allowUnary=true;
			break;

		case Token::ADD:
		case Token::SUBTRACT:
		{
			if (allowUnary)
			{
				postfix.push_back(Token(0.0));
			}
		}
		case Token::MULTIPLY:
		case Token::DIVIDE:
		case Token::POWER:
		case Token::MODULUS:
		case Token::LT:
		case Token::LT_E:
		case Token::EQUAL:
		case Token::N_EQUAL:
		case Token::GT_E:
		case Token::GT:
		case Token::OR:
		case Token::AND:
			do
			{
				if (!stk.empty())
				{
					tos = stk.top();

					if (tos.isOperator() && (tos >= t.GetPresidence()))
					{
						postfix.push_back(tos);
						stk.pop();
					}
					else
						break;
				}
			} while ((!stk.empty()) && tos.isOperator());
			stk.push(t);
			allowUnary=true;
			break;

		case Token::CLOSE_P:
			do
			{
				if (!stk.empty())
				{
					tos = stk.top();

					if (*tos != Token::OPEN_P)
					{
						postfix.push_back(tos);
						stk.pop();
					}
				}
			} while ((!stk.empty()) && (*tos != Token::OPEN_P));

			if (!stk.empty())
				stk.pop();							// Discard the '('
			allowUnary=false;
			break;
		}
		i++;				// Process next input token.
	}

	// Now actually run the reverse polish stack.

	// cout << *this << endl;

	vector<Token>::const_iterator j;

	for (j=postfix.begin() ; j != postfix.end() ; j++)
	{
		if (j->isOperator())
		{
			if (stk.size() < 1)
			{
				m << "INVALID expression ";
				copy(prefix.begin(), prefix.end(), ostream_iterator<Token>(m, " "));
				throw RecoverableError (m.str());
			}

			Token t1 = stk.top();
			stk.pop();
			Token t2 = stk.top();
			stk.pop();

			if (t1.GetBaseType() == Token::INVALID)
			{
				m << "Attempting to evaluate an uninitialised variable:  " << t1.GetString() << endl;
				copy(prefix.begin(), prefix.end(), ostream_iterator<Token>(m, " "));
				throw RecoverableError (m.str());
			}
			if (t2.GetBaseType() == Token::INVALID)
			{
				m << "Attempting to evaluate an uninitialised variable:  " << t2.GetString() << endl;
				copy(prefix.begin(), prefix.end(), ostream_iterator<Token>(m, " "));
				throw RecoverableError (m.str());
			}
			if (t1.GetBaseType() != t2.GetBaseType())
			{
				m << "Cannot mix operand types, e.g 12.3 = 'ABC' is invalid.";
				copy(prefix.begin(), prefix.end(), ostream_iterator<Token>(m, " "));
				throw RecoverableError (m.str());
			}
			if ((t1.GetBaseType() == Token::STRING) && (!j->StringOp()))
			{
				m << "Cannot perform operation " << j->GetId() << " on STRING operands.";
				copy(prefix.begin(), prefix.end(), ostream_iterator<Token>(m, " "));
				throw RecoverableError (m.str());
			}

			if ((e = evaluationorder(t2,t1)) == 0)
			{						// Both operands are constants so we can constant fold
				switch (**j)		// in common code.
				{
				case Token::ADD:
					t2 += t1;
					break;
				case Token::SUBTRACT:
					t2 -= t1;
					break;
				case Token::MULTIPLY:
					t2 *= t1;
					break;
				case Token::DIVIDE:
					t2 /= t1;
					break;
				case Token::POWER:
					t2 ^= t1;
					break;
				case Token::MODULUS:
					t2 %= t1;
					break;
				case Token::LT:
					t2 = Token ((t2 <  t1) ? 1 : 0);
					break;
				case Token::LT_E:
					t2 = Token ((t2 <= t1) ? 1 : 0);
					break;
				case Token::EQUAL:
					t2 = Token ((t2 == t1) ? 1 : 0);
					break;
				case Token::N_EQUAL:
					t2 = Token ((t2 != t1) ? 1 : 0);
					break;
				case Token::GT_E:
					t2 = Token ((t2 >= t1) ? 1 : 0);
					break;
				case Token::GT:
					t2 = Token ((t2 >  t1) ? 1 : 0);
					break;
				case Token::OR:
					// Works even for STRING operands as something like "ABC" = "ABC" OR etc
					// will have had the equality operator evaluated first, (its of higher
					// presidence) and thus these resultant tokens will be numberic '0' or '1'
					t2 = Token ((t2.GetNumber() || t1.GetNumber()) ? 1 : 0);
					break;
				case Token::AND:
					t2 = Token ((t2.GetNumber() && t1.GetNumber()) ? 1 : 0);
					break;
				}
				stk.push (t2);
			}
			else   // Setup run time evaluation either compiled or interpreted.
			{
				stk.push(Run (e,t2,*j,t1));
			}
			res |= e;				// So caller cn work out if the entire expression is compile only.
		}
		else
		{
			stk.push(*j);
		}
	}

	// Save result in the class.
	postfix[0] = stk.top();
	stk.pop();
	return res;
}

int Eval::evaluationorder(const Token& op1,const Token& op2)
{
	int bw = 0;

	if ((*op1 != Token::STRING) && (*op1 != Token::NUMBER))
		bw |= 1;
	if ((*op2 != Token::STRING) && (*op2 != Token::NUMBER))
		bw |= 2;
	return bw;
}

const vector<Token>& Eval::GetPostfix(void) const
{
	return postfix;
}

const Token& Eval::GetResult(void) const
{
	return postfix[0];
}

ostream &operator<<(ostream &out, const Eval &p)
{
	vector<Token>::const_iterator j;

	for (j=p.GetPostfix().begin() ; j != p.GetPostfix().end() ; j++)
	{
		cout << *j << endl;
	}
	return out;
}

}		// End Namespace SML
