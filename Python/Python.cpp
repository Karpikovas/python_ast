// Python.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "Lexer.h"
#include "Parser.h"

/*---------------- Вывод дерева ----------------------*/

void printTree(const AST& e, int level) {

	for (int i = 0; i < level; i++) {
		cout << "   ";
	}
	cout << e.token.value << std::endl;

	for (const auto& item : e.treeItems) {
		printTree(item, level + 1);
	}
}

int main()
{
	ifstream fin;
	fin.open("input.txt");
	Lexer l(fin);

	Parser p(l.tokens);
	printTree(p.parse(), 0);

	system("pause");


    return 0;
}

