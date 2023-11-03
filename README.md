# libparser

<p>libparser is a C++ library for parsing I developed in 2009 when building a C compiler for my Compiler class at college.
It is able to parse SLR(1), LL(1) and Regex.</p>
<p>The parser uses a grammar in the BNF format as input, and then generates the parsing table.
The parsing table can be generated dynamically in runtime or it can be generated with the binary (from this project)`parsergen`,
which will generate C++ code containing the parsing tables.</p>

# Building

To build libparser you will need CMake. To build the unit tests, you will need cppunit testing framework.

libparser binaries will be placed in folder build.

To build libparser run these commands:

	$ cd build
	$ cmake ..
	$ make

This will also build the parsing table generation tool parsergen.

To build the test-case run these commands:

	$ cd test/build
	$ cmake ..
	$ make

To run the tests just execute the executable (test/build/test).


# Using libparser

To use libparser you just need to add the include folder in your compiler's includes search directories,
and link your executable with libparser binary.


# TODO
	* Documentation.
		libparser is lacking documentation.
		The code should contains Doxygen documentation.
	
	* LALR(1) parsing.
		Generate the ParserTable for LALR(1).
	
	* Optimize Input.
		Currently, each character read need a virtual method call.
		this is very inefficient. An alternative would be creating a buffer
		of characters, so the virtual call could fill the buffer instead of
		reading a single character.
	
	* Rollback between Inputs in MultInput.
		MultInput doesn't support rollbacks between two Inputs.
		If while reading the end of one Input the ScannerAutomata
		still doesn't know if it already read enough characters to the next
		token or if it need to keep reading, this can cause a rollback between
		two Inputs, which is not supported by MultInput.
