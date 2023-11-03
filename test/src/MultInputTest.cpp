#include "MultInputTest.h"

#include "TestDefs.h"

#include "parser/FileInput.h"
#include "parser/ListInput.h"
#include "parser/MemoryInput.h"
#include "parser/OffsetInput.h"

CPPUNIT_TEST_SUITE_REGISTRATION(MultInputTest);

static std::string readFile(const std::string & file);

void MultInputTest::setUp() {}

void MultInputTest::tearDown() {}

void MultInputTest::listInputTest1() {
	std::string inputBuf = readFile(INPUT_FOLDER + "input_c1.txt")
			+ readFile(INPUT_FOLDER + "input_parser1.txt")
			+ readFile(INPUT_FOLDER + "input_parser2.txt")
			+ readFile(INPUT_FOLDER + "input1.txt")
			+ readFile(INPUT_FOLDER + "input2.txt");
	
	Input *in1 = new FileInput(INPUT_FOLDER + "input_c1.txt");
	Input *in2 = new FileInput(INPUT_FOLDER + "input_parser1.txt");
	Input *in3 = new FileInput(INPUT_FOLDER + "input_parser2.txt");
	Input *in4 = new FileInput(INPUT_FOLDER + "input1.txt");
	Input *in5 = new FileInput(INPUT_FOLDER + "input2.txt");
	
	ListInput *listInput = new ListInput();
	listInput->addInput(in1);
	listInput->addInput(in2);
	listInput->addInput(in3);
	listInput->addInput(in4);
	listInput->addInput(in5);
	
	doTest(listInput, inputBuf);
	
	delete(listInput);
}

void MultInputTest::listInputTest2() {
	std::string inputBuf = readFile(INPUT_FOLDER + "input_c1.txt")
			+ readFile(INPUT_FOLDER + "input_parser1.txt")
			+ readFile(INPUT_FOLDER + "input_parser2.txt")
			+ readFile(INPUT_FOLDER + "input1.txt")
			+ readFile(INPUT_FOLDER + "input2.txt");
	
	Input *in1 = new MemoryInput(readFile(INPUT_FOLDER + "input_c1.txt"));
	Input *in2 = new MemoryInput(readFile(INPUT_FOLDER + "input_parser1.txt"));
	Input *in3 = new MemoryInput(readFile(INPUT_FOLDER + "input_parser2.txt"));
	Input *in4 = new MemoryInput(readFile(INPUT_FOLDER + "input1.txt"));
	Input *in5 = new MemoryInput(readFile(INPUT_FOLDER + "input2.txt"));
	
	ListInput *listInput = new ListInput();
	listInput->addInput(in1);
	listInput->addInput(in2);
	listInput->addInput(in3);
	listInput->addInput(in4);
	listInput->addInput(in5);
	
	doTest(listInput, inputBuf);
	
	delete(listInput);
}

void MultInputTest::offsetInputTest1() {
	const unsigned int TESTS = 5;
	
	const char *files[TESTS] = {
			"input_c1.txt",
			"input_parser1.txt",
			"input_parser2.txt",
			"input1.txt",
			"input2.txt"
	};
	
	const int offsets[TESTS] = {0, -10, 10, 11, 42};
	
	for (unsigned int i = 0; i < TESTS; ++i) {
		std::string inputBuf = readFile(INPUT_FOLDER + files[i]);
		Input *in = new OffsetInput(new FileInput(INPUT_FOLDER + files[i]), offsets[i]);
		doOffsetTest(in, offsets[i], inputBuf);
		delete(in);
	}
}

void MultInputTest::offsetInputTest2() {
	const unsigned int TESTS = 5;
	
	const char *files[TESTS] = {
			"input_c1.txt",
			"input_parser1.txt",
			"input_parser2.txt",
			"input1.txt",
			"input2.txt"
	};
	
	const int offsets[TESTS] = {0, -10, 10, 11, 42};
	
	for (unsigned int i = 0; i < TESTS; ++i) {
		std::string inputBuf = readFile(INPUT_FOLDER + files[i]);
		Input *in = new OffsetInput(new MemoryInput(inputBuf), offsets[i]);
		doOffsetTest(in, offsets[i], inputBuf);
		delete(in);
	}
}

void MultInputTest::doTest(Input *input, const std::string & inputBuf) {
	input->sanityCheck();
	
	for (unsigned int i = 0; i < inputBuf.size(); ++i) {
		CPPUNIT_ASSERT(input->nextChar() == inputBuf[i]);
		input->sanityCheck();
	}
	CPPUNIT_ASSERT(input->nextChar() == '\0');
	input->sanityCheck();
}

void MultInputTest::doOffsetTest(Input *input, int offset, const std::string & inputBuf) {
	input->sanityCheck();
	
	int line = 1;
	if (line >= -offset) CPPUNIT_ASSERT((int)input->getInputLine() == line + offset);
	
	for (unsigned int i = 0; i < inputBuf.size(); ++i) {
		CPPUNIT_ASSERT(input->nextChar() == inputBuf[i]);
		input->sanityCheck();
		
		if (inputBuf[i] == '\n') {
			++line;
			if (line >= -offset) {
				CPPUNIT_ASSERT((int)input->getInputLine() == line + offset);
			}
		}
	}
	CPPUNIT_ASSERT(input->nextChar() == '\0');
	input->sanityCheck();
}

static std::string readFile(const std::string & file) {
	std::string in;
	
	FILE *fp = fopen(file.c_str(), "r");
	
	char c;
	while ((c = fgetc(fp)) != EOF) in.push_back(c);
	
	fclose(fp);
	
	return in;
}
