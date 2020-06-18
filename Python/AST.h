#pragma once
#include "stdafx.h"
#include <vector>
#include <iostream>
#include "Token.h"

using namespace std;

// Структура листа дерева токенов
struct AST {
	AST(Token token) : token(token) {}
	AST(Token token, AST left) : token(token), treeItems{ left } {}
	AST(Token token, AST left, AST right) : token(token), treeItems{ left, right } {}
	AST(Token token, vector<AST> treeItems) : token(token), treeItems(treeItems) {}

	Token token;
	vector<AST> treeItems; // поддеревья
};

