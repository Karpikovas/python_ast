#pragma once
#include "stdafx.h"
#include <list>
#include "Token.h"
#include "AST.h"

class Parser
{
public:
	explicit Parser(list<Token>tokens) : tokens(tokens) {}
	AST parse();
private:
	list<Token>tokens; // ������ �������

	/*------ ��������������� ������� ----------*/
	/**/Token nextToken();
	/**/AST returnBackToken(Token token);
	/**/int getPriority(const string& binaryOp);

	/*------- ������� ����������� -----------*/
	/**/bool parseTab(int level);/*��� ��� ���� �������� ����� ������ �����, ��� ����� � ������ ����������� ������*/
	AST parseList();
	/**/AST parseLoopOperators();

	/**/AST parseDefCall();
	/**/AST parseClassStatement(bool fromClass);
	/**/AST parseDefReturn(bool fromClass);

	/**/AST parseCondition(bool fromClass);
	/**/AST parseArithmeticExpr(bool fromClass);

	/**/AST parseSimpleExpression(int tokenTypes[], bool fromClass);
	/**/AST parseBinaryExpression(int minPriority, int tokenTypes[], bool fromClass);

	/*------- �������� ������ ------*/
	/**/vector<AST> parseAllArgsChildren(bool isClass);
	/**/AST parseArgs(bool fromClass);

	vector<AST> parseAllBodyChildren(int level, bool fromLoop, bool fromDef, bool fromClass);
	AST parseBody(int level, bool fromLoop, bool fromDef, bool fromClass);

	/*-------- ��������� ����������� --------*/
	/**/AST parseAssignment(bool fromClass);

	/**/vector<AST> parseIfElifElseSeg(int level, int keyword, bool fromLoop, bool fromDef, bool fromClass);
	/**/AST parseIfElifElse(int level, bool fromLoop, bool fromDef, bool fromClass);

	/**/AST parseWhile(int level, bool fromDef, bool fromClass);
	/**/AST parseFor(int level, bool fromDef, bool fromClass);

	/**/AST parseDef(int level, bool fromLoop, bool fromDef, bool fromClass);

	/**/AST parseClass(int level);
	/**/AST parseClassInit(int level);

	vector<AST> parseAllClassBodyChildren(int level);

	
};

