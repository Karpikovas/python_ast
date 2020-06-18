#pragma once
#include "stdafx.h"
#include <string>

using namespace std;

enum TokenType {
	/*--------------- Ключевые слова -----------------*/

	/*0*/KEYWORD_BOOLEAN, // False, True
	/*1*/KEYWORD_LOGICAL_OPERATION, // and, not, or
	/*2*/KEYWORD_BREAK,
	/*3*/KEYWORD_CONTINUE,
	/*4*/KEYWORD_CLASS, //-----------------------
	/*5*/KEYWORD_DEF,
	/*6*/KEYWORD_IF, 
	/*7*/KEYWORD_ELIF,
	/*8*/KEYWORD_ELSE,
	/*9*/KEYWORD_WHILE,
	/*10*/KEYWORD_FOR,
	/*11*/KEYWORD_IN,
	/*12*/KEYWORD_RANGE,
	/*13*/KEYWORD_PASS,//-----------------------
	/*14*/KEYWORD_RETURN,
	/*15*/KEYWORD_SELF,//-----------------------
	/*16*/KEYWORD___INIT__,

	/*----------------- Остальные токены ------------------*/

	/*17*/TOKEN_TAB,
	/*18*/TOKEN_IDENTIFIER,
	/*19*/TOKEN_ARITHMETIC_OPER, // + | - | * | / | %
	/*20*/TOKEN_COMPA_OPER, // ==| < | > | <= | >= | != | <>
	/*21*/TOKEN_COLON, // :
	/*22*/TOKEN_DOT, // .
	/*23*/TOKEN_COMMA, // ,
	/*24*/TOKEN_ASSIGN, // =
	/*25*/TOKEN_NUMBER,
	/*26*/TOKEN_RIGHT_BRACKET, // )
	/*27*/TOKEN_LEFT_BRACKET,// (
	/*28*/TOKEN_SQUARE_RIGHT_BRACKET, // ]
	/*29*/TOKEN_SQUARE_LEFT_BRACKET,// [
	/*30*/TOKEN_STRING_CONST,

	/*31*/TOKEN_PROGRAM
};

// Структура Токена
struct Token {
	Token(string value, int type) : value(value), type(type) {}

	string value; // значение токена
	int type; // тип токена
};