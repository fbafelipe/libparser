#ifndef PARSER_DYNAMIC_AUTOMATA_H
#define PARSER_DYNAMIC_AUTOMATA_H

#include "parser/ParserDefs.h"

#include <map>
#include <ostream>
#include <set>
#include <vector>

class Automata;

/**
 * @class DynamicAutomata
 * 
 * Class that represent a mutable Automata.
 * The automata does not need to be deterministic and can have epsilon transitions,
 * this class has method to convert this automata in a deterministic.
 * 
 * DynamicAutomata should be used while performing operations with the automata,
 * after the operations it can be converted to an Automata (see Automata class). 
 * 
 * More states can be created after constructing the automata.
 * 
 * @author Felipe Borges Alves
 * 
 * @see Automata
 * @see AutomataOperations
 */
class DynamicAutomata {
	public:
		class State;
				
		typedef std::set<State *> StateSet;
		typedef std::vector<State *> StateList;
		typedef std::vector<StateSet> StateSetList;
		typedef std::map<State *, StateSet> StateToSet; // maps a state to a set of states
		typedef std::map<State *, State *> StateToState;
		typedef std::map<StateSet, State *> StateSetToState;
		typedef std::map<State *, unsigned int> StateIndex;
		
		/**
		 * @class State
		 * 
		 * A state from a DynamicAutomata.
		 * 
		 * @author Felipe Borges Alves
		 */
		class State {
			public:
				/**
				 * Default constructor.
				 */
				State();
				~State();
				
				/**
				 * Get the transitions of this state for a given input.
				 * 
				 * If there are no transitions with the given input, NULL is returned.
				 * 
				 * @param input The input of the transition.
				 * 
				 * @return A StateSet with the transitions of this state or NULL.
				 */
				const StateSet *getTransitions(char input) const;
				
				/**
				 * Get a transition of this state for a given input.
				 * 
				 * Since there is no warranty for which transition will be returned,
				 * this method should be used only when this state has only one
				 * transition with the given input or if you need any transition.
				 * 
				 * @param input The input of the transition.
				 * 
				 * @return The target state of the transition or NULL if this state has no
				 * transitions with the given input.
				 */
				State *getTransition(char input) const; // return the first transition or null
				
				/**
				 * Add a transition to this state.
				 * 
				 * @param input The input of the transition.
				 * @param destState The target state of the transition.
				 */
				void addTransition(char input, State *destState);
				
				/**
				 * Remove the transition of this state that has the specified input
				 * and the specified target.
				 * If this state has no such transition this method does nothing.
				 * 
				 * @param input The input of the transition.
				 * @param destState The target state of the transition.
				 */
				void removeTransition(char input, State *destState);
				
				/**
				 * Set the transition of this state.
				 * Any existing transitions with the specified input will be removed.
				 * 
				 * @param input The input of the transition
				 *  @param destState The target state of the transition.
				 */
				void setTransition(char input, State *destState);
				
				/**
				 * @return The epsilon transitions of this state.
				 */
				const StateSet *getEpsilonTransitions();
				
				/**
				 * Add an epsilon transition to this state.
				 * 
				 * @param destState The target state of the transition.
				 */
				void addEpsilonTransition(State *destState);
				
				/**
				 * Remove an epsilon transition of this state.
				 * If this state has no such transition this method does nothing.
				 * 
				 * @param destState The target state of the transition.
				 */
				void removeEpsilonTransition(State *destState);
				
				/**
				 * Remove all epsilon transitions.
				 */
				void clearEpsilonTransitions();
				
				/**
				 * Remove all transitions.
				 */
				void clearAllTransitions();
				
				/**
				 * Set this state as a final state or a non final state.
				 * 
				 * @param finalState True to set this state as a final state.
				 */
				void setFinalState(bool finalState);
				
				/**
				 * @return True if this state is a final state.
				 */
				bool isFinalState() const;
				
				/**
				 * Get the states reachable by using only epsilon transitions.
				 */
				void getEpsilonReachableStates(StateToSet & closure,
						StateSet & marked, StateSet & output);
				
			private:
				bool finalState;
				
				StateSet *transitions[ALPHABET_SIZE];
				StateSet epsilonTransitions;
		};
		
		/**
		 * Default constructor.
		 */
		DynamicAutomata();
		
		/**
		 * Construct a copy of the specified DynamicAutomata.
		 * 
		 * @param automata The automata that will be copied.
		 */
		DynamicAutomata(const DynamicAutomata *automata);
		
		/**
		 * Construct a copy of the specified Automata.
		 * 
		 * @param automata The automata that will be copied.
		 */
		DynamicAutomata(const Automata *automata);
		~DynamicAutomata();
		
		/**
		 * @return The initial state of the automata.
		 */
		State *getInitialState() const;
		
		/**
		 * Create a new state.
		 * 
		 * @return The created state.
		 */
		State *createState();
		
		/**
		 * Get the final states.
		 * 
		 * @param finalStates A StateSet where the final states will be inserted.
		 */
		void getFinalStates(StateSet & finalStates) const;
		
		/**
		 * Get the final states.
		 * 
		 * @param finalStates A StateSet where the non final states will be inserted.
		 */
		void getNonFinalStates(StateSet & nonFinalStates) const;
		
		/**
		 * Get the epsilon closure of this automata.
		 * 
		 * @param closure A StateToSet where each state of this automata will
		 * be mapped to it's epsilon closure.
		 */
		void getEpsilonClosure(StateToSet & closure) const;
		
		/**
		 * Remove the epsilon transitions, adding transitions so
		 * this automata keep representing the same language.
		 */
		void removeEpsilonTransitions();
		
		/**
		 * Determinize this automata.
		 * 
		 * The automata cannot have any epsilon transition.
		 * Unreachable states will be removed.
		 */
		void determinize();
		
		/**
		 * Determinize this automata.
		 * 
		 * The automata cannot have any epsilon transition.
		 * Unreachable states will be removed.
		 * 
		 * @param stateMap A map of the old states of this automata to the new
		 * state that replace the old one.
		 * @param oldStates A list with the old states.
		 */
		void determinize(StateSetToState & stateMap, StateList & oldStates);
		
		/**
		 * Minimize this DynamicAutomata.
		 * 
		 * The automata must be deterministic and cannot have any unreachable states.
		 */
		void minimize();
		
		/**
		 * Minimize this DynamicAutomata.
		 * 
		 * The automata must be deterministic and cannot have any unreachable states.
		 * 
		 * @param oldStateToNewState A map of the old states of this automata to the new
		 * state that replace the old one.
		 * @param oldStates A list with the old states.
		 */
		void minimize(StateToState & oldStateToNewState, StateList & oldStates);
		
		/**
		 * Minimize this DynamicAutomata.
		 * Final states will not be merged. This is useful when each final
		 * state has a different semantic (recognize a different token, for example).
		 * 
		 * The automata must be deterministic and cannot have any unreachable states.
		 */
		void minimizeNoFinalMerge();
		
		/**
		 * Minimize this DynamicAutomata.
		 * Final states will not be merged. This is useful when each final
		 * state has a different semantic (recognize a different token, for example).
		 * 
		 * The automata must be deterministic and cannot have any unreachable states.
		 * 
		 * @param oldStateToNewState A map of the old states of this automata to the new
		 * state that replace the old one.
		 * @param oldStates A list with the old states.
		 */
		void minimizeNoFinalMerge(StateToState & oldStateToNewState, StateList & oldStates);
		
		/**
		 * Minimize this DynamicAutomata.
		 * 
		 * The automata must be deterministic and cannot have any unreachable states.
		 * 
		 * @param oldStateToNewState A map of the old states of this automata to the new
		 * state that replace the old one.
		 * @param oldStates A list with the old states.
		 * @param finalMerge True if final states should be merged.
		 */
		void minimize(StateToState & oldStateToNewState, StateList & oldStates,
				bool finalMerge);
		
		/**
		 * Make this automata an empty automata, rejecting all sentences.
		 */
		void makeEmptyAutomata();
		
		/**
		 * Make this automata an empty automata, rejecting all sentences.
		 * 
		 * @param oldStates A list where the old states will be stored.
		 */
		void makeEmptyAutomata(StateList & oldStates);
		
		// remove epsilon transitions, determinize and minimize this automata
		/**
		 * Remove the epsilon transitions, determinize and minimize.
		 */
		void determineAndMinimize();
		
		/**
		 * Remove the epsilon transitions, determinize and minimize.
		 * 
		 * @param oldStateToNewState A map of the old states of this automata to the new
		 * state that replace the old one.
		 * @param oldStates A list with the old states.
		 */
		void determineAndMinimize(StateSetToState & stateMap, StateList & oldStates);
		
		/**
		 * Convert this DynamicAutomata into an Automata.
		 * 
		 * @return An Automata that represent the same language as this DynamicAutomata.
		 * 
		 * @see Automata
		 */
		Automata *toAutomata() const;
		
		/**
		 * @return The number of states of this DynamicAutomata.
		 */
		unsigned int getNumStates() const;
		
		/**
		 * @return A list with all states of this DynamicAutomata.
		 */
		const StateList & getStates() const;
		
		/**
		 * Get a state by it's index.
		 * 
		 * @param index The index of the state.
		 * 
		 * @return The state of the specified index.
		 */
		State *getState(unsigned int index) const;
		
		/**
		 * Build a map of states to it's indices.
		 * 
		 * @param stateIndex A map of states to unsigned int where the index will be built.
		 */
		void getStateIndex(StateIndex & stateIndex) const;
		
		/**
		 * @return A clone of this DynamicAutomata.
		 */
		DynamicAutomata *clone() const;
		
		// the comparation operators are meant to test purposes
		// they can be very inefficient
		// both automatas must be minimized automatas
		bool operator==(const DynamicAutomata & automata) const;
		bool operator!=(const DynamicAutomata & automata) const;
		
		// this method also maps the equivalent states
		bool compareTo(const DynamicAutomata *automata, StateToState & thisToOther) const;
		
		bool operator==(const Automata & automata) const;
		bool operator!=(const Automata & automata) const;
		
		friend std::ostream & operator<<(std::ostream & stream, const DynamicAutomata & automata);
		
	private:
		class EquivalenceClasses;
		
		// the initialState must be the first state in the list
		StateList states;
		
		State *initialState;
};

#endif
