#pragma once
#include "stdafx.h"
#include <list>
#include <fstream>
#include "Token.h"
#include <iostream>


using namespace std;

class Lexer
{
public:
	list<Token>tokens; // список токенов
	explicit Lexer(ifstream &fin) : fin(fin) { tokenize(); } //explicit - ключевое слово ставится перед объявлением конструктора с одним параметром и предотвращает неявное преобразование типов при инициализации.
	void tokenize();

private:
	const char* input; // входная строка
	ifstream &fin;

	bool isNumber();
	bool isOperation();
	bool isKeyword();
	bool isDelimiter();
	bool isIdentifier();
	bool isBracket();
	bool isStringConst();
};

