#include "InputTest.h"

#include "TestDefs.h"

#include "parser/FileInput.h"
#include "parser/MemoryInput.h"

#include <cstdio>

CPPUNIT_TEST_SUITE_REGISTRATION(InputTest);

static std::string readFile(const std::string & file);
static std::string createInput(unsigned int size);

void InputTest::setUp() {}

void InputTest::tearDown() {}

void InputTest::test1() {
	std::string input = readFile(INPUT_FOLDER + "input1.txt");
	
	Input *fileInput = new FileInput(INPUT_FOLDER + "input1.txt");
	doTest(fileInput, input);
	delete(fileInput);
	
	Input *memoryInput = new MemoryInput(input.c_str());
	doTest(memoryInput, input);
	delete(memoryInput);
}

void InputTest::test2() {
	std::string input = readFile(INPUT_FOLDER + "input2.txt");
	
	Input *fileInput = new FileInput(INPUT_FOLDER + "input2.txt");
	doTest(fileInput, input);
	delete(fileInput);
	
	Input *memoryInput = new MemoryInput(input.c_str());
	doTest(memoryInput, input);
	delete(memoryInput);
}

void InputTest::dynamicTest1() {
	doDynamicTest(1);
}

void InputTest::dynamicTest2() {
	doDynamicTest(63);
}

void InputTest::dynamicTest3() {
	doDynamicTest(64);
}

void InputTest::dynamicTest4() {
	doDynamicTest(65);
}

void InputTest::dynamicTest5() {
	doDynamicTest(100);
}

void InputTest::doDynamicTest(unsigned int size) {
	std::string input = createInput(size);
	
	char *buf = new char[size + 1];
	for (unsigned int i = 0; i < size; ++i) buf[i] = input[i];
	buf[size] = '\0';
	
	FILE *fp = fmemopen(buf, size, "r");
	
	Input *fileInput = new FileInput(fp);
	doTest(fileInput, input);
	delete(fileInput);
	delete[](buf);
	
	Input *memoryInput = new MemoryInput(input.c_str());
	doTest(memoryInput, input);
	delete(memoryInput);
}

void InputTest::doTest(Input *input, const std::string & inputBuf) {
	input->sanityCheck();
	
	// read fully
	input->markPosition();
	for (unsigned int pos = 0; pos < inputBuf.size(); ++pos) {
		CPPUNIT_ASSERT(input->nextChar() == inputBuf[pos]);
		input->sanityCheck();
	}
	CPPUNIT_ASSERT(input->nextChar() == '\0');
	input->rollback(0);
	input->sanityCheck();
	
	// read one char a time
	for (unsigned int roll = 1; roll < inputBuf.size(); ++roll) {
		for (unsigned int m = 0; m < inputBuf.size() - roll; m += roll) {
			input->sanityCheck();
			input->markPosition();
			input->sanityCheck();
			
			for (unsigned int pos = m; pos < inputBuf.size(); ++pos) {
				CPPUNIT_ASSERT(input->getInputPos() == pos);
				CPPUNIT_ASSERT(input->nextChar() == inputBuf[pos]);
				input->sanityCheck();
			}
			CPPUNIT_ASSERT(input->nextChar() == '\0');
			
			input->rollback(roll);
			input->sanityCheck();
		}
		
		input->sanityCheck();
		input->resetInput();
		input->sanityCheck();
	}
}

static std::string readFile(const std::string & file) {
	std::string in;
	
	FILE *fp = fopen(file.c_str(), "r");
	
	char c;
	while ((c = fgetc(fp)) != EOF) in.push_back(c);
	
	fclose(fp);
	
	return in;
}

static std::string createInput(unsigned int size) {
	std::string in;
	
	char c = 'A';
	for (unsigned int i = 0; i < size; ++i) {
		in.push_back(c++);
		if (c > 'Z') c = 'A';
	}
	
	return in;
}
