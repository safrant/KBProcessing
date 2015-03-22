class header template

#ifndef _H
#define _H

#include <iostream>

class  Item
{
private:
			// ********** Data Members
	errorstate;

	static string stringDelimiter;
	static bool delimit;
	static char propertyDelimiter;

public:
			// ********** Constants 

			// ********** Constructor/Destructor
	Item(type val var = default);
	Item(const Item &);
	virtual ~Item();

			// ********** Assignment Operator
	Item & operator= (const Item &);

			// ********** Mutator Functions
	Item & set_ (type val);
	Item & add_ (type val);
	Item & remove_ (type val);
	Item & swap_(Item &);
	string getstringdelimiter (void) const;
	char getpropertydelimiter (void) const;

			// ********** Accessor Functions
	type get_ (void) const;

			// ********** Input Functions
	Item & input (void);
	Item & input_ (void);

			// ********** Print Functions
	void print (void) const;
	void print_ (void) const;

			// ********** Validation Functions 
	bool is_valid (val) const;
	bool isempty (val) const;
	bool is_empty (val) const;

			// ********** Comparison Functions 
	bool issame_ (const val &) const;

			// ********** Relational Operators 
	bool operator== (const Item &) const;
	bool operator!= (const Item &) const;
	bool operator<= (const Item &) const;
	bool operator>= (const Item &) const;
	bool operator< (const Item &) const;
	bool operator> (const Item &) const;
	
			// ********** Arithmetic Operators 
	Item & operator++ (void);
	Item & operator++ (int);
	Item & operator-- (void);
	Item & operator-- (int);
	Item & operator+ (const Item &) const;
	Item & operator- (const Item &) const;
	Item & operator* (const Item &) const;
	Item & operator/ (const Item &) const;
	Item & operator% (const Item &) const;

			// ********** Utility Functions 
			// ********** File I/O Functions

};
			// ********** Non-Member Operators
	ostream & operator<< (ostream &, const Item &);
	istream & operator>> (istream &, Item &);


#endif

//  **************************** CONSTRUCTOR ******************************
//  **************************** DESTRUCTOR *******************************
//  ************************** ASSIGNMENT OPERATOR ************************
//  *************************** MUTATOR FUNCTIONS *************************

void Item::setstringdelimiter(string value)				// string delimiter
{
	stringDelimiter = value;
	delimit = true;
}
void Item::setpropertydelimiter(char value)				// component delimiter
	{propertyDelimiter = value; }

//  ************************** ACCESSOR FUNCTIONS *************************
const type & Item::get___ () const
{
	return this->___;
}
string Item::getstringdelimiter (void) const
	{return stringDelimiter; }

char Item::getpropertydelimiter (void) const
	{return propertyDelimiter; }

//  *************************** INPUT FUNCTIONS ***************************
Item & Item::input (void)
{
	input___();
	return *this;
}
Item & Item::input___ (void)
{
	char answer;
	string stringName;
	do {
		cout << "Enter ____ name:  ";
		getline(cin, stringName);
		cout << "Is \"" << stringName << "\" correct? (y/n) ";
		cin.get(answer);
		cin.clear();
		cin.ignore(80, '\n');
	} while (answer != 'Y' && answer != 'y');

	set___(stringName);
	return *this;
}

//  *************************** PRINT FUNCTIONS ***************************
void Item::print(void) const
{
	print___();
	cout << propertyDelimiter;
	print__2();
}
void Item::print___(void) const
{
	cout << stringDelimiter << var << stringDelimiter;
}
void Item::print__2(void) const
{
	cout << stringDelimiter << var2 << stringDelimiter;
}

//  ************************* VALIDATION FUNCTIONS ************************
//  ************************* COMPARISON FUNCTIONS ************************
//  ************ COMPARISON FUNCTIONS and RELATIONAL OPERATORS ************
//  ************************* RELATIONAL OPERATORS ************************
//  ************************* ARITHMETIC OPERATORS ************************
//  ************************** UTILITY FUNCTIONS **************************
//  ************************** FILE I/O FUNCTIONS *************************
//  ************************* NON-MEMBER OPERATORS ************************
ostream & operator<< (ostream & output, const Item & right)
{
	right.print();
	return output;
}
istream & operator>> (istream & input, Item & right)
{
	string nameString;
	input >> nameString;
	right.set___(nameString);
	return input;
}

//  **************************** CONSTRUCTOR ******************************
//  **************************** DESTRUCTOR *******************************
//  ************************** ASSIGNMENT OPERATOR ************************
//  *************************** MUTATOR FUNCTIONS *************************
//  ************************** ACCESSOR FUNCTIONS *************************
//  *************************** INPUT FUNCTIONS ***************************
//  *************************** PRINT FUNCTIONS ***************************
//  ************************* VALIDATION FUNCTIONS ************************
//  *************************** ERROR FUNCTIONS ***************************
//  ************************* COMPARISON FUNCTIONS ************************
//  ************************* RELATIONAL OPERATORS ************************
//  ************************* ARITHMETIC OPERATORS ************************
//  ************************** UTILITY FUNCTIONS **************************
//  ************************** FILE I/O FUNCTIONS *************************
//  ************************* NON-MEMBER OPERATORS ************************

