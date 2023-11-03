#ifndef PARSER_AUTOMATA_OPERTIONS_H
#define PARSER_AUTOMATA_OPERTIONS_H

#include "parser/DynamicAutomata.h"

/**
 * @class AutomataOperations
 * 
 * Class that implements algorithms for operations over DynamicAutomatas.
 * 
 * @author Felipe Borges Alves
 * @see DynamicAutomata
 */
class AutomataOperations {
	public:
		typedef DynamicAutomata::State State;
		typedef DynamicAutomata::StateList StateList;
		typedef DynamicAutomata::StateSet StateSet;
		typedef DynamicAutomata::StateToState StateToState;
		typedef DynamicAutomata::StateIndex StateIndex;
		
		/**
		 * Perform the union operation.
		 * 
		 * @param automataA The automata to be used in the union.
		 * @param automataB The automata to be used in the union.
		 * 
		 * @return The union of automataA with automataB.
		 */
		static DynamicAutomata *automataUnion(const DynamicAutomata *automataA,
				const DynamicAutomata *automataB);
		
		/**
		 * Perform the union operation mapping the states from automataA and automataB
		 * to the states of the resulting DynamicAutomata.
		 * 
		 * @param automataA The automata to be used in the union.
		 * @param oldStatesAToNewStates A reference to a StateToState map where
		 * the stated of automataA will be mapped to the equivalent states of the
		 * resulting DynamicAutomata.
		 * 
		 * @param automataB The automata to be used in the union.
		 * @param oldStatesBToNewStates A reference to a StateToState map where
		 * the stated of automataB will be mapped to the equivalent states of the
		 * resulting DynamicAutomata.
		 * 
		 * @return The resulting DynamicAutomata.
		 */
		static DynamicAutomata *automataUnion(
				const DynamicAutomata *automataA, StateToState & oldStatesAToNewStates,
				const DynamicAutomata *automataB, StateToState & oldStatesBToNewStates);
		
		/**
		 * Perform the concatenation operation.
		 * 
		 * @param automataA The first automata of the concatenation.
		 * @param automataB The second automata of the concatenation.
		 * 
		 * @return The concatenation of automataA with automataB.
		 */
		static DynamicAutomata *automataConcatenation(const DynamicAutomata *automataA,
				const DynamicAutomata *automataB);
		
		/**
		 * Perform the concatenation operation mapping the states from automataA and automataB
		 * to the states of the resulting DynamicAutomata.
		 * 
		 * @param automataA The first automata of the concatenation.
		 * @param oldStatesAToNewStates A reference to a StateToState map where
		 * the stated of automataA will be mapped to the equivalent states of the
		 * resulting DynamicAutomata.
		 * 
		 * @param automataB The second automata of the concatenation.
		 * @param oldStatesBToNewStates A reference to a StateToState map where
		 * the stated of automataB will be mapped to the equivalent states of the
		 * resulting DynamicAutomata.
		 * 
		 * @return The resulting DynamicAutomata.
		 */
		static DynamicAutomata *automataConcatenation(
				const DynamicAutomata *automataA, StateToState & oldStatesAToNewStates,
				const DynamicAutomata *automataB, StateToState & oldStatesBToNewStates);
		
		/**
		 * Perform the closure operation.
		 * 
		 * @param automata The automata to be used in the closure. 
		 * 
		 * @return The closure of automata.
		 */
		static DynamicAutomata *automataClosure(const DynamicAutomata *automata);
		
		/**
		 * Perform the closure operation mapping the states from automata
		 * to the states of the resulting DynamicAutomata.
		 * 
		 * @param automata The automata to be used in the closure. 
		 * @param oldStatesToNewStates A reference to a StateToState map where
		 * the stated of automata will be mapped to the equivalent states of the
		 * resulting DynamicAutomata.
		 * 
		 * @return The resulting DynamicAutomata.
		 */
		static DynamicAutomata *automataClosure(const DynamicAutomata *automata,
				StateToState & oldStatesToNewStates);
		
	private:
		AutomataOperations();
};

#endif
