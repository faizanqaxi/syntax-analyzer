
#include <iostream>
#include <fstream>
#include <string>
using namespace std;


bool isID(const string &str)
{
	if (isdigit(str[0]))
		return false;
	int lineCouter = 0;
	if (str[0] == '_')
		lineCouter++;

	for (; lineCouter < str.size(); lineCouter++)
		if (!(isalnum(str[lineCouter])))
			return false;

	return true;
}

bool markComment(const string &str)
{
	return str == "/*" || str == "//";
}

bool stringFound(const string &str)
{
	return str[0] == '"' && str[str.size() - 1] == '"';
}

bool booleanFound(const string &str)
{
	return str == "true" || str == "false";
}


void parseAnalyze(const string &nameOfFile)
{
	int lineNum = 0;
	char ch;
	string buffer;
	fstream file(nameOfFile, fstream::in);

	if (!file.is_open())
	{
		cout << "error while opening the file\n";
		exit(0);
	}

	bool miltiCm = false, singleCm = false;
	while (file >> noskipws >> ch)
	{
		if (singleCm || miltiCm)
		{
			if (singleCm && ch == '\n')
			{
				lineNum++;
				singleCm = false;
			}

			if (miltiCm && ch == '*')
			{
				file >> ch;
				if (ch == EOF)
					break;

				if (ch == '/')
					miltiCm = false;
			}
			continue;
		}

		if (ch == '/')
		{
			string comm(1, ch);
			file >> ch;
			if (ch == EOF)
			{
				cout << comm;
				break;
			}

			if (ch == '*')
			{
				miltiCm = true;
				comm += ch;
			}
			else if (ch == '/')
			{
				singleCm = true;
				comm += ch;
			}
			if (miltiCm || singleCm)
			{
				continue;
			}
		}
	}
	file.close();
}

class Token {
private:
	string CP; //ClassPart
	string VP; //ValuePart
	int lineNo;
public:
	Token(string CP, string VP, int lineNo) {
		this->CP = CP;
		this->VP = VP;
		this->lineNo = lineNo;
	}
	Token(const Token & A) {
		this->CP = A.CP;
		this->VP = A.VP;
		this->lineNo = A.lineNo;
	}
	//Function for printing on console
	void print() {
		cout << "(Classpart: " << CP << " , Valuepart: " << VP << ", lineno: " << lineNo << endl;
	}
	//Function for printing in file
	void printToFile(ofstream &fout) {
		fout << "(" << CP << " , " << VP << " , " << lineNo << " )"
			<< endl;
	}
};
int main(int argc, char** argv) {
	string line, a;
	int lineno = 0;
	char inFileName[] = "program.txt";
	char outFileName[] = "tokens.txt";
	ifstream file(inFileName);
	int arrSize = 100;
	int currIndex = 0;
	Token **TokenArray = new Token*[arrSize];
	//No object of token is made yet, only references are made(for efficiency)
	//Handling Possible File Opening Errors
		if (!file.is_open())
		{
			cout << "*********ERROR OPENING " << inFileName << " FOR INPUT**********\n";
		}
		else
		{
			bool multiLineComment = false; //Turns true if multi line comment found.Outside loop as it can be on multiple lines
				while (!file.eof())
				{
					//This Loop Inputs One Line At A Time Until File End
					lineno++;
					getline(file, line);
					a = line;
					bool singleLineComment = false; //Turns true if single line comment found.Inside loop as single line comment is only applicable on one line
						bool doubleQuote = false; //Turns true when double quote starts.False when it ends.
						string strToStoreValue = ""; //For Double Quotes
													 //Following loop does lexical analysis on one line
					for (int i = 0; i < a.length() && !singleLineComment;
						i++)
					{
						
						if (currIndex + 10 >= arrSize) {
							arrSize += 20;
							Token **temp = TokenArray;
							TokenArray = new Token *[arrSize];
							for (int q = 0; q < currIndex; q++)
								TokenArray[q] = temp[q];
						}
						int temp = 0;
						while (temp < (a.length() - 1))
							temp++;
						if (a[temp] != ';')
						{
							if (a[temp] != ')' && a[temp] != '}' && a[temp] != '{')
							{
								if (a.substr(0, 4) == "class")
								{
									TokenArray[currIndex++] = new Token("Class Declared ", a, lineno);
								}
								else
								{
									if (a[temp] != ';')
										TokenArray[currIndex++] = new Token("Syntax Error, Invalid delimeter ", a, lineno);
									else
										TokenArray[currIndex++] = new Token("Semicolor Missing Error ", a, lineno);
								}
							}
						}

						//********************************************************MULTIPLE
						//LINE COMMENTS HANDLES
							//HERE*************************************************
							if (multiLineComment) {
								//If multiline comment has started and not
								//ended yet, ignore everything until comment end
									if (a[i] == '*' && a[i + 1] == '/')
									{
										multiLineComment = false;
										i++;
									}
							}
						//******************************************************DOUBLE
						//QUOTES HANDLED
							//HERE************************************************************
							else if (doubleQuote) {
							if (a[i] == '\\' && a[i + 1] == '"') {
								//Ignore in this case because it is
							//	allowed inside a string
									strToStoreValue += '\"';
								i += 1;
							}
							else if (a[i] == '\\' && a[i + 1] == '\\')
							{
								//In strings // means simply /, so:
								i += 1;
								strToStoreValue += '\\';
							}
							else if (a[i] == '"')
							{ //i.e double quote has ended
								doubleQuote = false;
								TokenArray[currIndex++] = new
									Token("String_Constant", strToStoreValue, lineno);
								strToStoreValue = ""; //Flushing the
								//value so that var can be used again for same line
							}
							else if (i == a.length() - 1 && a[i] != '"')
							{ //i.e. line has ended but closing
								//quote was not found
									strToStoreValue += a[i];
								TokenArray[currIndex++] = new
									Token("String_Constant Error!", strToStoreValue, lineno);
								strToStoreValue = "";
							}
							else
							{
								strToStoreValue += a[i];
								//TokenArray[currIndex++] = new Token("Syntax Error ", a, lineno);
								//TokenArray[currIndex++] = new Token("Syntax Error ", a, lineno);
							}
							//Storing this so that it can be stored in file if no String_Const error
						}
							else {
								//********************************************************SIMPLE
								//CHARACTERS HANDLED
									//HERE****************************************************
									if (a[i] == ' ')
									{
										//If white space, do nothing
									}
									else if (a[i] == '/' && a[i + 1] == '/')
									{
										//Single line comment
										singleLineComment = true; //for loop
									//	handles the rest
											i++;
									}
									else if (a[i] == '/' && a[i + 1] == '*')
									{ //Not checking for */ as it should be
									//	checked only if multiLineComment == true
											//Multi Line Comment
											multiLineComment = true; //while
									//	loop handles the rest
											i++;
									}
									else if (a[i] == ':')
									{
										//Added for ':'
										TokenArray[currIndex++] = new
											Token(":", ":", lineno);
									}
									else if (a[i] == '!' && a[i + 1] == '=')
									{
										//Added for '!'
										TokenArray[currIndex++] = new
											Token("Compound", "!=", lineno);
										i++;
									}
									else if (a[i] == '!')
									{
										//Added for '!'
										TokenArray[currIndex++] = new
											Token("!", "!", lineno);
									}
									else if (a[i] == ',')
									{
										//Added for ','
										TokenArray[currIndex++] = new
											Token("','", "','", lineno);
									}
									else if (a[i] == ';')
									{
										//Added for ';'
										TokenArray[currIndex++] = new
											Token(";", ";", lineno);
										if (a[i - 1] == ';')
											TokenArray[currIndex++] = new Token("Semicolor(;) error ", a, lineno);
									}
									else if (a[i] == '"')
									{
										//Assuming that a string can not be on
										//multiple lines
											doubleQuote = true;
									}
									else if (a[i] == '\'')
									{
										//for single quote start
										if (a[i + 1] == '\\' && a[i + 2] ==
											'\\' && a[i + 3] == '\'') //if '\\'
										{
											TokenArray[currIndex++] = new
												Token("Char_Constant", "\\", lineno);
											i += 3;
										}
										else if (a[i + 1] == '\\' && a[i + 2]
											== 'n' && a[i + 3] == '\'') //if '\n'
										{
											TokenArray[currIndex++] = new
												Token("Char_Constant", "\\n", lineno);
											i += 3;
										}
										else if (a[i + 1] == '\\' && a[i + 2]
											== '\t' && a[i + 3] == '\'') //if '\t'
										{
											TokenArray[currIndex++] = new
												Token("Char_Constant", "\\t", lineno);
											i += 3;
										}
										else if (a[i + 1] == '\\' && a[i + 2]
											== '\'' && a[i + 3] == '\'') //if '\''
										{
											TokenArray[currIndex++] = new
												Token("Char_Constant", "'", lineno);
											i += 3;
										}
										else if (a[i + 2] == '\'' && a[i + 1]
											!= '\'' && a[i + 1] != '\\') //Normal case in which only one character is
											//inside and that character is not \ or '
										{
											string temp = "";
											temp += '\'';
											temp += a[i + 1];
											temp += '\'';
											TokenArray[currIndex++] = new
												Token("Char_Constant", temp, lineno);
											i += 2;
										}
										else {
											string temp = "";
											temp += a[i];
											temp += a[i + 1];
											temp += a[i + 2];
											temp += a[i + 3];
											TokenArray[currIndex++] = new
												Token("Char_Constant Error!", temp, lineno);
											i += 3;
										}
									}
								//**************************************************C++ IDENTIFIERS
								//HANDLED HERE***********************************************************
									else if (a[i] == 'w' && a[i + 1] == 'h' &&
										a[i + 2] == 'i' && a[i + 3] == 'l' && a[i + 4] == 'e')
								{
									//For while
									TokenArray[currIndex++] = new
										Token("ID", "while", lineno);
									i += 4;
								}
									else if (a[i] == 's' && a[i + 1] == 't' &&
										a[i + 2] == 'r' && a[i + 3] == 'i' && a[i + 4] == 'n' && a[i + 5] == 'g')
									{
										//For string
										TokenArray[currIndex++] = new
											Token("ID", "string", lineno);
										i += 5;
									}
									else if (a[i] == 'f'&& a[i + 1] == 'o'&& a[i
										+ 2] == 'r')
									{
										TokenArray[currIndex++] = new
											Token("ID", "for", lineno);
										i += 2;
									}
									else if (a[i] == 'i'&& a[i + 1] == 'f')
									{
										TokenArray[currIndex++] = new
											Token("ID", "if", lineno);
										i += 1;
									}
									else if (a[i] == 'p'&& a[i + 1] == 'u'&& a[i
										+ 2] == 'b'&& a[i + 3] == 'l'&& a[i + 4] == 'i'&& a[i + 5] == 'c')
									{
										TokenArray[currIndex++] = new
											Token("ID", "public", lineno);
										i += 4;
									}
									else if (a[i] == 'p'&& a[i + 1] == 'r'&& a[i
										+ 2] == 'i'&& a[i + 3] == 'v'&& a[i + 4] == 'a'&& a[i + 5] == 't'&& a[i +
										6] == 'e')
									{
										TokenArray[currIndex++] = new
											Token("ID", "private", lineno);
										i += 5;
									}
									else if (a[i] == 'p'&& a[i + 1] == 'r'&& a[i
										+ 2] == 'o'&& a[i + 3] == 't'&& a[i + 4] == 'e' && a[i + 5] == 'c'&& a[i
										+ 6] == 't'&& a[i + 7] == 'e'&& a[i + 8] == 'd')
									{
										TokenArray[currIndex++] = new
											Token("ID", "protected", lineno);
										i += 7;
									}
									else if (a[i] == 's'&&a[i + 1] == 't'&&a[i +
										2] == 'a'&& a[i + 3] == 't'&& a[i + 4] == 'i'&& a[i + 5] == 'c')
									{
										TokenArray[currIndex++] = new
											Token("ID", "static", lineno);
										i += 5;
									}
									else if (a[i] == 'c'&& a[i + 1] == 'l'&&a[i
										+ 2] == 'a' && a[i + 3] == 's' && a[i + 4] == 's')
									{
										TokenArray[currIndex++] = new
											Token("class", "class", lineno);
										i += 4;
									}
									else if (a[i] == 'a' && a[i + 1] == 'r' &&
										a[i + 2] == 'r' && a[i + 3] == 'a' && a[i + 4] == 'y')
									{
										TokenArray[currIndex++] = new
											Token("ID", "array", lineno);
										i += 4;
									}
									else if (a[i] == 'c' && a[i + 1] == 'o' &&
										a[i + 2] == 'n' && a[i + 3] == 't' && a[i + 4] == 'i' && a[i + 5] ==
										'n'&& a[i + 6] == 'u' && a[i + 7] == 'e')
									{
										TokenArray[currIndex++] = new
											Token("ID", "continue", lineno);
										i += 7;
									}
									else if (a[i] == 'm'&& a[i + 1] == 'a' &&
										a[i + 2] == 'i' && a[i + 3] == 'n')
									{
										TokenArray[currIndex++] = new
											Token("ID", "main", lineno);
										i += 7;
									}
									else if (a[i] == 'v' && a[i + 1] == 'o' &&
										a[i + 2] == 'i' && a[i + 3] == 'd')
									{
										TokenArray[currIndex++] = new
											Token("ID", "void", lineno);
										i += 3;
									}
									else if (a[i] == 't' && a[i + 1] == 'r' &&
										a[i + 3] == 'u' && a[i + 3] == 'e')
									{
										TokenArray[currIndex++] = new
											Token("ID", "true", lineno);
										i += 3;
									}
									else if (a[i] == 'f' && a[i + 1] == 'a' &&
										a[i + 2] == 'l' && a[i + 3] == 's' && a[i + 4] == 'e')
									{
										TokenArray[currIndex++] = new
											Token("ID", "false", lineno);
										i += 4;
									}
									else if (a[i] == 'a' && a[i + 1] == 'b' &&
										a[i + 2] == 's'&& a[i + 3] == 't'&& a[i + 4] == 'r' && a[i + 5] == 'a' &&
										a[i + 6] == 'c' && a[i + 7] == 't')
									{
										TokenArray[currIndex++] = new
											Token("ID", "abstract", lineno);
										i += 7;
									}
									else if (a[i] == 's' && a[i + 1] == 'e' &&
										a[i + 2] == 'a' && a[i + 3] == 'l' && a[i + 4] == 'e' && a[i + 5] == 'd')
									{
										TokenArray[currIndex++] = new
											Token("ID", "sealed", lineno);
										i += 5;
									}
									else if (a[i] == 'b' && a[i + 1] == 'r' &&
										a[i + 2] == 'e' && a[i + 3] == 'a' && a[i + 4] == 'k')
									{
										TokenArray[currIndex++] = new
											Token("ID", "break", lineno);
										i += 4;
									}
									else if (a[i] == 'r'&& a[i + 1] == 'e' &&
										a[i + 2] == 't' && a[i + 3] == 'u' && a[i + 4] == 'r' && a[i + 5] == 'n')
									{
										TokenArray[currIndex++] = new
											Token("ID", "return", lineno);
										i += 5;
									}
								//**************************************************OPERATORS
								//HANDLED
									//HERE****************************************************************
									else if (a[i] == '&' && a[i + 1] == '&')
								{
									TokenArray[currIndex++] = new
										Token("&", "&", lineno);
								}
									else if (a[i] == '|' && a[i] == '|')
									{
										TokenArray[currIndex++] = new
											Token("|", "|", lineno);
									}
									else if (a[i] == '=')
									{
										//Added for handling simple '=', '=='
										//is not required so not adding check for that
											TokenArray[currIndex++] = new
											Token("=", "=", lineno);
									}
									else if (a[i] == '+'&& a[i + 1] == '=')
									{
										TokenArray[currIndex++] = new
											Token("Assignment", "+=", lineno);
										i++;
									}
									else if (a[i] == '-'&& a[i + 1] == '=')
									{
										TokenArray[currIndex++] = new
											Token("Assignment", "-=", lineno);
										i++;
									}
									else if (a[i] == '*'&& a[i + 1] == '=')
									{
										TokenArray[currIndex++] = new
											Token("Assignment", "*=", lineno);
										i++;
									}
									else if (a[i] == '/'&& a[i + 1] == '=')
									{
										TokenArray[currIndex++] = new
											Token("Assignment", "/=", lineno);
										i++;
									}
									else if (a[i] == '>' && a[i + 1] == '=')
									{
										//For >=
										TokenArray[currIndex++] = new
											Token("Comparison", ">=", lineno);
										i++;
									}
									else if (a[i] == '<' && a[i + 1] == '=')
									{
										//for <=
										TokenArray[currIndex++] = new
											Token("Comparison", "<=", lineno);
										i++;
									}
									else if (a[i] == '>')
									{
										TokenArray[currIndex++] = new
											Token("Comparison", ">", lineno);
									}
									else if (a[i] == '<')
									{
										TokenArray[currIndex++] = new
											Token("Comparison", "<", lineno);
									}
									else if (a[i] == '+')
									{
										TokenArray[currIndex++] = new
											Token("+", "+", lineno);
									}
									else if (a[i] == '-')
									{
										TokenArray[currIndex++] = new Token("-", " - ", lineno);
									}
									else if (a[i] == '*')
									{
										TokenArray[currIndex++] = new
											Token("*", "*", lineno);
									}
									else if (a[i] == '/')
									{
										TokenArray[currIndex++] = new
											Token("/", "/", lineno);
									}
									else if (a[i] == '%')
									{
										TokenArray[currIndex++] = new
											Token("%", "%", lineno);
									}
								//********************************************************BRACKETS
								//HANDLED HERE****************************************************
									else if (a[i] == '(')
								{
									TokenArray[currIndex++] = new
										Token("(", "(", lineno);
								}
									else if (a[i] == ')')
									{
										TokenArray[currIndex++] = new
											Token(")", ")", lineno);
									}
									else if (a[i] == '{')
									{
										TokenArray[currIndex++] = new
											Token("{", "{", lineno);
									}
									else if (a[i] == '}')
									{
										TokenArray[currIndex++] = new
											Token("}", "}", lineno);
									}
								//******************************************************INTEGERS
							//	AND FLOATING POINT NUMBERS HANDLED
							//		HERE*****************************************************
									else if (a[i] == '.' || (a[i] >= 49 && a[i]
										<= 57))
								{
									//if number is found or decimal point
								//	is found.Using Ascii values of numbers here 49 to 57
										bool alphabetBreak = false;
									bool decimalPoint = false;
									bool secondDecimalPoint = false;
									bool isDouble = false;
									if (a[i] == '.') {
										isDouble = true;
										decimalPoint = true;
									}
									string strForStoring = "";
									strForStoring += a[i];
									while (!file.eof() && i < a.length() &&
										a[i + 1] != ':' && a[i + 1] != '\n' && a[i] != '\0' && !secondDecimalPoint &&
										a[i + 1] != ';' && a[i + 1] != ';' && a[i + 1] != ' ' && a[i + 1] != '='
										&& a[i + 1] != '<' && a[i + 1] != '>' && a[i + 1] != '+' && a[i + 1] !=
										'"' && a[i + 1] != '\'' && a[i + 1] != '-' && a[i + 1] != '/' && a[i + 1]
										!= '*') {
										//If alphabet occurs between the
									//	number the number is not legal identifier or a legal number
											i++;
										if (a[i] >= 49 && a[i] <= 57)
											strForStoring += a[i];
										else if (a[i] == '.')
										{
											//The number is double in
										//	this case
												isDouble = true;
											if (decimalPoint == false)
											{
												decimalPoint = true;
												strForStoring +=
													a[i];
											}
											else {
												secondDecimalPoint =
													true;
												//Having a second
											//	decimal point should instantly break the loop
													i -= 1; //i-=2 so
												//that this decimal point can be checked against another number
											}
										}
										else if (i + 1 < a.length()) {
											//The number is not legal
											//because of some unexpected character
												alphabetBreak = true;
											strForStoring += a[i];
										}
									}
									if (alphabetBreak)
									{
										TokenArray[currIndex++] = new
											Token("invalid", strForStoring, lineno);
									}
									else if (isDouble) {
										TokenArray[currIndex++] = new
											Token("Double_Constant", strForStoring, lineno);
									}
									else
									{
										TokenArray[currIndex++] = new
											Token("Integer_Constant", strForStoring, lineno);
										//TokenArray[currIndex++] = new Token("Syntax Error ", strForStoring, lineno);
									}
								}
								//**************************************************SIMPLE VARIABLE
								//IDENTIFIERS HANDLED
									//HERE***********************************************************
									else if ((a[i] >= 65 && a[i] <= 90) || (a[i]
										>= 97 && a[i] <= 122))
								{
									//variable starts with a character
									string strForStorage = "";
									strForStorage += a[i];
									i++;
									while (!file.eof() && i < a.length() &&
										a[i + 1] != ':' && a[i + 1] != '\n' && a[i] != '\0' && a[i + 1] != '.' &&
										a[i + 1] != ';' && a[i + 1] != ';' && a[i + 1] != ' ' && a[i + 1] != '='
										&& a[i + 1] != '<' && a[i + 1] != '>' && a[i + 1] != '+' && a[i + 1] !=
										'"' && a[i + 1] != '\'' && a[i + 1] != '-' && a[i + 1] != '/' && a[i + 1]
										!= '*')
									{
										strForStorage += a[i];
										i++;
									}
									i--;
									TokenArray[currIndex++] = new
										Token("ID", strForStorage, lineno);
								}
									else
									{
									//	TokenArray[currIndex++] = new Token("Syntax Error ", a, lineno);
									}
							}
					}
				}

			//The close instruction makes sure that file is properly
			//closed.Not writing this can curropt the file data
				file.close(); //Close program.txt
		}
	ofstream FileOut;
	FileOut.open(outFileName); //Writing the output in "tokens.txt".
	//If it exists it will be overwritten, else it will be automatically
		//created.
		if (!FileOut.is_open())
			cout << "*********ERROR OPENING " << outFileName << " FOR OUTPUT*********\n";
		else {
			for (int i = 0; i < currIndex; i++) {
				TokenArray[i]->print();
				TokenArray[i]->printToFile(FileOut);
			}
			FileOut.close();
		}
}