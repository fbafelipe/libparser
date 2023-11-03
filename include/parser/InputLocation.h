#ifndef PARSER_INPUT_LOCATION_H
#define PARSER_INPUT_LOCATION_H

#include <ostream>
#include <string>

/**
 * @class InputLocation
 * 
 * Store a input name and a input position.
 * 
 * @author Felipe Borges Alves
 */
class InputLocation {
	public:
		/**
		 * Construct an InputLocation that has the default input name and points to
		 * the first column of the first line.
		 */
		InputLocation();
		
		/**
		 * Construct an InputLocation with the specified name and points to
		 * the first column of the first line.
		 * 
		 * @param n The name of the input.
		 */
		InputLocation(const std::string & n);
		
		/**
		 * Construct an InputLocation with the specified name and points to
		 * the specified column in the specified line.
		 * 
		 * @param n The name of the input.
		 * @param l The line of the input.
		 * @param c The column in the line of the input.
		 */
		InputLocation(const std::string & n, unsigned int l, unsigned int c);
		
		/**
		 * @return The input name.
		 */
		const std::string & getName() const;
		
		/**
		 * Change the input name.
		 * 
		 * @param n The name of the input.
		 */
		void setName(const std::string & n);
		
		/**
		 * @return The input line number.
		 */
		unsigned int getLine() const;
		
		/**
		 * Change the line number of this InputLocation.
		 * 
		 * @param l The line of the input.
		 */
		void setLine(unsigned int l);
		
		/**
		 * @return The column of the input line.
		 */
		unsigned int getColumn() const;
		
		/**
		 * Change the column in the line of the input.
		 * 
		 * @param c The column in the line of the input.
		 */
		void setColumn(unsigned int c);
		
		InputLocation & operator=(const InputLocation & location);
		
		friend std::ostream & operator<<(std::ostream & stream, const InputLocation & location);
		
	private:
		std::string name;
		
		unsigned int line;
		unsigned int column;
};

#endif
