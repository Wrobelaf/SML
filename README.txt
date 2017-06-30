Environment variables explicently required for the SML project:

export DEV_HOME=/windows/D
export CONFIG=Debug
export TDIR=${DEV_HOME}/SML/TestSuite
export LOKI_DIR=${DEV_HOME}/Loki/loki-0.1.7
export LD_LIBRARY_PATH=${DEV_HOME}/Useful/${CONFIG}:${LOKI_DIR}/lib
export LIBRARY_PATH=$LD_LIBRARY_PATH
export PATH=${DEV_HOME}/SML/CompileSML/${CONFIG}:${DEV_HOME}/SML/InterpretSML/${CONFIG}:${DEV_HOME}/SML/SML/${CONFIG}:$PATH

alias s='${DEV_HOME}/SML/SMLTest/${CONFIG}/SMLTest -i $TDIR | grep FAILURE'
alias sf='${DEV_HOME}/SML/SMLTest/${CONFIG}/SMLTest -i $TDIR'


Note the ../MakeInclude is used by all of Allan's C++ work.
The SMLInclude in this directoty is the Makeinclude specific to all of SML. (still requires MakeInclude)

