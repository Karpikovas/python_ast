#include "stdafx.h"
#include "Lexer.h"


void Lexer::tokenize() {
	string inputStr;

	while (getline(fin, inputStr)) {
		input = inputStr.c_str();

		while (*input != NULL) {

			while (strncmp(input, " ", 1) == 0) ++input;

			bool isTokenFinded =    isNumber()
								 || isDelimiter()
								 || isOperation()
								 || isBracket()
								 || isKeyword()
								 || isIdentifier()
								 || isStringConst();

			if (!isTokenFinded) {
				throw std::invalid_argument("Unknown token!");
			}
		}
	}

	//for (Token t : tokens)
		//std::cout << t.value <<" ---" << t.type << endl;
};

bool Lexer::isNumber() {
	if (isdigit(*input)) {
		string number;
		while (isdigit(*input) || *input == '.') number.push_back(*input++);
		tokens.push_back(Token(number, TOKEN_NUMBER));
		return true;
	}

	return false;
};

bool Lexer::isBracket() {
	if (strncmp(input, "(", 1) == 0) {
		input++;
		tokens.push_back(Token("(", TOKEN_LEFT_BRACKET));
		return true;
	}

	if (strncmp(input, ")", 1) == 0) {
		input++;
		tokens.push_back(Token(")", TOKEN_RIGHT_BRACKET));
		return true;
	}

	if (strncmp(input, "[", 1) == 0) {
		input++;
		tokens.push_back(Token("(", TOKEN_SQUARE_LEFT_BRACKET));
		return true;
	}

	if (strncmp(input, "]", 1) == 0) {
		input++;
		tokens.push_back(Token(")", TOKEN_SQUARE_RIGHT_BRACKET));
		return true;
	}

	return false;
};

bool Lexer::isStringConst() {
	if (strncmp(input, "\"", 1) == 0) {
		string str;
		str.push_back(*input++);
		while (!(strncmp(input, "\"", 1) == 0)) str.push_back(*input++);
		str.push_back(*input++);
		tokens.push_back(Token(str, TOKEN_STRING_CONST));
		return true;
	}

	return false;
};

bool Lexer::isIdentifier() {

	if (isalpha(*input) || strncmp(input, "_", 1) == 0) {
		string id;
		while(isalnum(*input) || isalpha(*input) || strncmp(input, "_", 1) == 0)  id.push_back(*input++);
		tokens.push_back(Token(id, TOKEN_IDENTIFIER));
		return true;
	}

	return false;
};

bool Lexer::isDelimiter() {
	if (strncmp(input, "\t", 1) == 0) {
		input++;
		tokens.push_back(Token("<tab>", TOKEN_TAB));
		return true;
	}

	if (strncmp(input, ":", 1) == 0) {
		input++;
		tokens.push_back(Token(":", TOKEN_COLON));
		return true;
	}

	if (strncmp(input, ".", 1) == 0) {
		input++;
		tokens.push_back(Token(".", TOKEN_DOT));
		return true;
	}

	if (strncmp(input, ",", 1) == 0) {
		input++;
		tokens.push_back(Token(",", TOKEN_COMMA));
		return true;
	}

	return false;
};



bool Lexer::isOperation() {

	static const string arithmeticOperations[] =
	{ "+", "-", "%", "*", "/" };

	for (auto& t : arithmeticOperations) {
		if (strncmp(input, t.c_str(), t.size()) == 0) {
			input += t.size();
			tokens.push_back(Token(t, TOKEN_ARITHMETIC_OPER));
			return true;
		}
	}

	static const string compaOperations[] =
	{ "==", "<", ">", "<=", ">=", "!=", "<>" };

	for (auto& t : compaOperations) {
		if (strncmp(input, t.c_str(), t.size()) == 0) {
			input += t.size();
			tokens.push_back(Token(t, TOKEN_COMPA_OPER));
			return true;
		}
	}

	if (strncmp(input, "=", 1) == 0) {
		input ++;
		tokens.push_back(Token("=", TOKEN_ASSIGN));
		return true;
	}

	return false;
};

bool Lexer::isKeyword() {

	static const string boolean[] =
	{ "True", "False" };

	for (auto& t : boolean) {
		if (strncmp(input, t.c_str(), t.size()) == 0 && !isalpha(input[t.size()])) {
			input += t.size();
			tokens.push_back(Token(t, KEYWORD_BOOLEAN));
			return true;
		}
	}

	static const string logicalOperations[] =
	{ "or", "not", "and" };

	for (auto& t : boolean) {
		if (strncmp(input, t.c_str(), t.size()) == 0 && !isalpha(input[t.size()])) {
			input += t.size();
			tokens.push_back(Token(t, KEYWORD_LOGICAL_OPERATION));
			return true;
		}
	}

	static const string otherValue[] =
	{	"break", 
		"continue",
		"class", 
		"def", 
		"if",
		"elif",
		"else",
		"while",
		"for",
		"in",
		"range",
		"pass",
		"return",
		"self",
		"__init__"
	};

	static const int otherType[] =
	{	KEYWORD_BREAK,
		KEYWORD_CONTINUE,
		KEYWORD_CLASS,
		KEYWORD_DEF,
		KEYWORD_IF,
		KEYWORD_ELIF,
		KEYWORD_ELSE,
		KEYWORD_WHILE,
		KEYWORD_FOR,
		KEYWORD_IN,
		KEYWORD_RANGE,
		KEYWORD_PASS,
		KEYWORD_RETURN,
		KEYWORD_SELF,
		KEYWORD___INIT__
	};

	for (int i = 0; i < 15; i++) {
		if (strncmp(input, otherValue[i].c_str(), otherValue[i].size()) == 0 && !isalpha(input[otherValue[i].size()])) {
			input += otherValue[i].size();
			tokens.push_back(Token(otherValue[i], otherType[i]));
			return true;
		}
	}

	return false;
};
