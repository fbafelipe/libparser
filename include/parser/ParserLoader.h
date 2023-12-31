#ifndef PARSER_PARSER_LOADER_H
#define PARSER_PARSER_LOADER_H

class Grammar;
class ParserTable;
class ScannerAutomata;

/**
 * @class ParserLoader
 * 
 * Class that load/save ScannerAutomatas and ParserTables into buffers of byte arrays.
 * 
 * @author Felipe Borges Alves
 * @see ScannerAutomata
 * @see ParserTable
 */
class ParserLoader {
	public:
		/*********************************************************************
		 * Load functions
		 *********************************************************************/
		/**
		 * Load a ScannerAutomata from a buffer.
		 * The buffer must be a buffer previously generated by automataToBuffer method.
		 * There is no need to specify the size of the buffer, since the buffer it'self
		 * know his size.
		 * 
		 * @param buffer The buffer holding the ScannerAutomata.
		 * 
		 * @return The ScannerAutomata contained in the buffer.
		 */
		static ScannerAutomata *bufferToAutomata(const unsigned char *buffer);
		
		/**
		 * Load a ParserTable from a buffer.
		 * The buffer must be a buffer previously generated by tableToBuffer method.
		 * There is no need to specify the size of the buffer, since the buffer it'self
		 * know his size.
		 * 
		 * @param buffer The buffer holding the ParserTable.
		 * 
		 * @return The ParserTable contained in the buffer.
		 */
		static ParserTable *bufferToTable(const unsigned char *buffer);
		
		/**
		 * Read the ScannerAutomata, the ParserTable and the tokens/non terminals ids from a single buffer.
		 * 
		 * @param grammar A pointer where a grammar with the tokens/non terminals ids will be stored.
		 * @param scannerAutomata A pointer where the ScannerAutomata (or NULL if a ScannerAutomata is not present) will be stored.
		 * @param parserTable A pointer where the ParserTable (or NULL if a ParserTable is not present) will be stored.
		 */
		static void loadBuffer(const unsigned char *buffer, Grammar **grammar,
				ScannerAutomata **scannerAutomata, ParserTable **parserTable);
		
		/*********************************************************************
		 * Save functions
		 *********************************************************************/
		/**
		 * Save a ScannerAutomata in a buffer.
		 * 
		 * @param scannerAutomata The ScannerAutomata that will be stored in the buffer.
		 * @param bufferSize A pointer to an <code>unsigned int</code> where the size of the created buffer will be stored.
		 * 
		 * @return The buffer allocated holding the ScannerAutomata. The buffer is allocated with <code>new</code>,
		 * you must delete it when you are done using it.
		 */
		static unsigned char *automataToBuffer(const ScannerAutomata *scannerAutomata,
				unsigned int *bufferSize);
		
		/**
		 * Save a ParserTable in a buffer.
		 * 
		 * @param parserTable The ParserTable that will be stored in the buffer.
		 * @param bufferSize A pointer to an <code>unsigned int</code> where the size of the created buffer will be stored.
		 * 
		 * @return The buffer allocated holding the ParserTable. The buffer is allocated with <code>new</code>,
		 * you must delete it when you are done using it.
		 */
		static unsigned char *tableToBuffer(const ParserTable *parserTable,
				unsigned int *bufferSize);
		
		/**
		 * Save the ScannerAutomata, the ParserTable and the tokens/non terminals ids into a single buffer.
		 * 
		 * @param grammar The grammar with the tokens/non terminals ids that will be stored in the buffer.
		 * @param scannerAutomata The ScannerAutomata that will be stored in the buffer.
		 * @param parserTable The ParserTable that will be stored in the buffer.
		 * @param bufferSize A pointer to an <code>unsigned int</code> where the size of the created buffer will be stored.
		 * 
		 * @return The buffer allocated holding the tokens/non terminals ids, the ScannerAutomata and the ParserTable. The buffer is allocated with <code>new</code>,
		 * you must delete it when you are done using it.
		 */
		static unsigned char *saveBuffer(Grammar *grammar, ScannerAutomata *scannerAutomata,
				ParserTable *parserTable, unsigned int *bufferSize);
		
	private:
		ParserLoader();
};

#endif
