#ifndef PARSER_LIST_INPUT_H
#define PARSER_LIST_INPUT_H

#include "parser/Input.h"

#include "parser/MultInput.h"

#include <vector>

class ListInput : public MultInput {
	public:
		ListInput();
		virtual ~ListInput();
		
		virtual void resetInput();
		
		void addInput(Input *input);
		virtual Input *currentInput() const;
		
	private:
		typedef std::vector<Input *> InputList;
		
		// return true if there is another input
		virtual bool inputFinished();
		
		InputList inputList;
		
		unsigned int current;
};

#endif
