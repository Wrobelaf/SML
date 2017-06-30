/*
    instructions.h

    Class holding a mapping of number to instruction for the SML machine

*/

#ifndef __INSTRUCTIONS_
#define __INSTRUCTIONS_

namespace SML {

class Instruction
{
public:

    Instruction(int);
    virtual ~Instruction();

    enum instructions   {   READ        = 10,
                            WRITE       = 11,
                            WRITE_LITERAL= 12,
                            READ_STRING = 13,
                            WRITE_STRING= 14,
                            LOAD        = 20,
                            STORE       = 21,
                            LOAD_LITERAL= 22,
                            ADD         = 30,
                            SUBTRACT    = 31,
                            DIVIDE      = 32,
                            MULTIPLY    = 33,
                            MODULUS     = 34,
                            POWER       = 35,
                            SHIFT       = 36,
                            ADD_LITERAL = 37,
                            MULTIPLY_LITERAL = 38,
                            BRANCH      = 40,
                            BRANCH_NEG  = 41,
                            BRANCH_ZERO = 42,
                            HALT        = 43,
                            CMP_STRING  = 44,
                            INDIRECT_CMP_STRING  = 45,
                            NOOP        = 49,
                            SML_DEBUG   = 50,
                            SET_HEAP_ADD= 51,
                            INVALID     = 99};

    static const int OPERATOR_SHIFT;
    static const int MAX_POSITIVE_NUM;
    static const int MAX_NEGATIVE_NUM;

    bool operator==(const Instruction &) const;
    bool operator!=(const Instruction &) const;
    instructions operator*(void) const;
    friend ostream &operator<<(ostream &, const Instruction &);
    const char * const GetId(void) const;
    bool debug(void) const;
    bool debug(const bool);

private:

    static const int NUM_INSTRUCTIONS;

    static const struct v_inst {
        instructions inst;
        char *id;
    } valid_inst[];

    const v_inst *xx;
    bool dbug;

};

}			// End namespace SML

#endif      // __INSTRUCTIONS_

