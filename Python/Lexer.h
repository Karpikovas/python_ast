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
	list<Token>tokens; // ������ �������
	explicit Lexer(ifstream &fin) : fin(fin) { tokenize(); } //explicit - �������� ����� �������� ����� ����������� ������������ � ����� ���������� � ������������� ������� �������������� ����� ��� �������������.
	void tokenize();

private:
	const char* input; // ������� ������
	ifstream &fin;

	bool isNumber();
	bool isOperation();
	bool isKeyword();
	bool isDelimiter();
	bool isIdentifier();
	bool isBracket();
	bool isStringConst();
};

