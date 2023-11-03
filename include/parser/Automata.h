#ifndef PARSER_AUTOMATA_H
#define PARSER_AUTOMATA_H

#include <cassert>
#include <map>
#include <ostream>
#include <vector>

class DynamicAutomata;

/**
 * @class Automata
 * 
 * Class that represent an Automata.
 * This class uses a table of ints to store the automatas transitions.
 * The automata must be deterministic and cannot have epsilon transitions.
 * 
 * This class shouldn't be used to make automata operations.
 * If you are working with the automata (changing it, doing automata operations),
 * you should use DynamicAutomata instead and after you are done with it you can convert
 * it to a Automata. 
 * 
 * Once constructed, the size of the Automata cannot change.
 * 
 * @author Felipe Borges Alves
 * 
 * @see DynamicAutomata
 */
class Automata {
	public:
		typedef std::map<unsigned int, unsigned int> StateToState;
		
		/**
		 * Construct an Automata with <code>size</code> states.
		 * 
		 * @param size The number of states of the Automata.
		 */
		Automata(unsigned int size);
		
		/**
		 * Construct an Automata with specified transition table.
		 * See the method getTransition() table for the format documentation.
		 * 
		 * @param transitions The transitions of the automata, the automata
		 * will make a copy, leaving it unchanged.
		 * @param size The number of states of the Automata.
		 * 
		 * @see getTransitionTable
		 */
		Automata(const int * const *transitions, unsigned int size);
		virtual ~Automata();
		
		/**
		 * Set the transitions of Automata.
		 * See the method getTransition() table for the format documentation.
		 * 
		 * The transitions must have the same number of states as the Automata.
		 * 
		 * @param transitions The transitions of the automata, the automata
		 * will make a copy, leaving it unchanged.
		 * 
		 * @see getTransitionTable
		 */
		void setTransitionTable(const int * const *transitions);
		
		/**
		 * Get the transitions of the Automata.
		 * 
		 * The transitions are stored in an matrix (an array of arrays).
		 * The rows represent the input. The rows count should be always ALPHABET_SIZE.
		 * The columns represent the current state. The columns count should  be always the number of states.
		 * 
		 * The value in <code>transitions[input][state]</code> should be the target state of the transition
		 * or -1 if this transition does not exist. The target state cannot be
		 * greater or equal to the states count and cannot be less than -1 (the error state).
		 * 
		 * @return The transitions table. The returned matrix is not a copy
		 * and should not be changed or deleted.
		 */
		const int * const *getTransitionTable() const;
		
		/**
		 * @return The states count of this Automata.
		 */
		unsigned int getNumStates() const;
		
		/**
		 * @param currentState The state of the transition.
		 * @param input The input character.
		 * 
		 * @return The transition for the specified input in the specified state
		 * or -1 if there is no such transition.
		 */
		inline int getTransition(unsigned int currentState, char input) const {
			assert(currentState < numStates);
			assert(input >= 0);
			
			return transitionTable[(int)input][currentState];
		}
		
		/**
		 * Set a transition for a given input in a given state. 
		 * 
		 * @param state The state of the transition.
		 * @param input The input character.
		 * @param destinationState The target state of the transition.
		 */
		void setTransition(unsigned int state, char input, int destinationState);
		
		/**
		 * @return True if <code>state</code> is a final state.
		 */
		bool isFinalState(unsigned int state) const;
		
		/**
		 * Set a final state.
		 * 
		 * @param state The state that will be set as a final state.
		 */
		void setFinalState(unsigned int state);
		
		/**
		 * Set a vector of states as final states.
		 * 
		 * @param stateList The vector with the states that will be set as final states.
		 */
		void setFinalStates(const std::vector<unsigned int> & stateList);
		
		/**
		 * @return True if this Automata accept the input string.
		 */
		bool accept(const char *input) const;
		
		// check if this automata is in a consistent state
		/**
		 * This method is for debug.
		 * Check if this Automata is in a consistent state.
		 * 
		 * NOTE: This method will work only when compiled without NDEBUG.
		 */
		void sanityCheck() const;
		
		// the comparation operators are meant to test purposes
		// they can be very inefficient
		bool operator==(const DynamicAutomata & automata) const;
		bool operator!=(const DynamicAutomata & automata) const;
		
		bool compareTo(const Automata *automata, StateToState & thisToOther);
		
		bool operator==(const Automata & automata) const;
		bool operator!=(const Automata & automata) const;
		
		friend std::ostream & operator<<(std::ostream & stream, const Automata & automata);
		
	protected:
		void createStates();
		
		unsigned int numStates;
		int **transitionTable;
		
		bool *finalStates;
};

#endif
