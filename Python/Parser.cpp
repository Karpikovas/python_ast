#include "stdafx.h"
#include "Parser.h"

AST Parser::parse() {
	Token token = Token("", TOKEN_PROGRAM);
	vector<AST> children;

	while (tokens.size() != 0) {
		AST item = parseClass(0);

		if (item.token.type != -1) {
			children.push_back(item);
			continue;
		}

		item = parseDef(0, false, true, false);

		if (item.token.type != -1) {
			children.push_back(item);
			continue;
		}

		item = parseBody(0, false, false, false);

		if (item.token.type != -1) {
			children.push_back(item);
			continue;
		}
		else {
			throw std::invalid_argument("Syntax error!");
		}
	}

	return AST(token, children);
};

Token Parser::nextToken() {
	if (tokens.size() != 0) {
		Token token = tokens.front();
		tokens.pop_front();
		return token;
	}
	return Token("", -1);
};

/*------------ Парсит присваивание ----------
	Например: a = 5 + 2
*/
AST Parser::parseAssignment(bool fromClass) {
	Token token = nextToken();

	if (token.type == TOKEN_IDENTIFIER || (token.type == KEYWORD_SELF && fromClass)) {

		/*if (token.type == KEYWORD_SELF && fromClass) {
			Token dot = nextToken();

			if (dot.type == TOKEN_DOT) {
				
			}

		}
		*/
		Token next = nextToken();

		if (next.type == TOKEN_ASSIGN) {
			AST left = parseArithmeticExpr(fromClass);

			if (left.token.type != -1) {
				return AST(token, left);
			}

			left = parseCondition(fromClass);

			if (left.token.type != -1) {
				return AST(token, left);
			}

			Token str = nextToken();

			if (str.type == TOKEN_STRING_CONST){
				return(AST(token, str));
			}

		}
		tokens.push_front(next);
	}

	return returnBackToken(token);
};

/*--------- Парсит простое выражение для вычислительных операций ------------*/
AST Parser::parseSimpleExpression(int tokenTypes[], bool fromClass){
	AST item = parseClassStatement(fromClass);

	if (item.token.type != -1) {
		return item;
	}
	item = parseDefCall();

	if (item.token.type != -1) {
		return item;
	}
	
	Token token = nextToken();

	if (token.type == TOKEN_LEFT_BRACKET) {
		auto result = parseBinaryExpression(0, tokenTypes, fromClass);

		token = nextToken();

		if (token.type != TOKEN_RIGHT_BRACKET) 
			throw std::runtime_error("Expected ')'!");
		return result;
	}

	if (token.type == TOKEN_NUMBER)
		return AST(token);


	if (token.type == TOKEN_IDENTIFIER) {
		return AST(token);
	}

	if (token.type == TOKEN_SQUARE_LEFT_BRACKET) {
		tokens.push_front(token);

		AST item = parseList();
		if (item.token.type != -1) {
			return item;
		}
		token = nextToken();
	}
		
	for (int i = 0; i < sizeof(tokenTypes) / sizeof(tokenTypes[0]); i++) {
		if (token.type == tokenTypes[i] && tokenTypes[i] != KEYWORD_BOOLEAN)
			return AST(token, parseSimpleExpression(tokenTypes, fromClass));
		if (token.type == tokenTypes[i] && tokenTypes[i] == KEYWORD_BOOLEAN)
			return AST(token);
	}

	return returnBackToken(token);
};

/*--------- Парсит лист дерева для вычислительных операций ------------

	Например: а + 2 => + a
						 2
*/
AST Parser::parseBinaryExpression(int minPriority, int tokenTypes[], bool fromClass) {
	auto left_expr = parseSimpleExpression(tokenTypes, fromClass);

	if (left_expr.token.type == -1) {
		return left_expr;
	}

	while (tokens.size() != 0) {
		Token token = tokens.front();

		auto priority = getPriority(token.value);
		if (priority <= minPriority) {
			return left_expr;
		}

		tokens.pop_front();
		auto right_expr = parseBinaryExpression(priority, tokenTypes, fromClass);
		left_expr = AST(token, left_expr, right_expr);
	}

	if (tokens.size() == 0) {
		return left_expr;
	}
};


AST Parser::returnBackToken(Token token) {
	if (token.type != -1)
		tokens.push_front(token);
	return AST(Token("", -1));
}



/*
------------ Парсит аргумент  ---------
	Аргументами могут быть:
		- выражение (1 + 2)
		- переменная (т.е. идентификатор)
		- число
		- строковая константа
		- булево выражение
*/

AST Parser::parseArgs(bool fromClass) {

	AST item = parseClassStatement(fromClass);

	if (item.token.type != -1) {
		return item;
	}

	item = parseList();

	if (item.token.type != -1) {
		return item;
	}

	item = parseArithmeticExpr(fromClass);

	if (item.token.type != -1) {
		return item;
	}

	item = parseCondition(fromClass);

	if (item.token.type != -1) {
		return item;
	}

	item = parseDefCall();

	if (item.token.type != -1) {
		return item;
	}

	Token token = nextToken();

	if (token.type == TOKEN_IDENTIFIER 
		|| token.type == TOKEN_NUMBER 
		|| token.type == TOKEN_STRING_CONST
		|| token.type == KEYWORD_BOOLEAN
	) {
		return(AST(token));
	}

	return returnBackToken(token);
}

/*
------------ Парсит вызов функции --------------
	Например: test(1, 2, 3)
*/
AST Parser::parseDefCall() {
	Token token = nextToken();

	// Название функции
	if (token.type == TOKEN_IDENTIFIER && tokens.size() != 0) {

		Token next = nextToken();

		// Скобки
		if (next.type == TOKEN_LEFT_BRACKET) {
			vector<AST> children = parseAllArgsChildren(false);

			next = nextToken();
			if (next.type != TOKEN_RIGHT_BRACKET)
				throw std::runtime_error("Expected ')'!");
			
			return AST(token, children);
		}
		returnBackToken(next);
	}
	
	return returnBackToken(token);
};

/*
------------ Парсит вызов метода--------------
*/
AST Parser::parseClassStatement(bool fromClass) {
	Token token = nextToken();

	// Название функции
	if (token.type == TOKEN_IDENTIFIER || (token.type == KEYWORD_SELF && fromClass) && tokens.size() != 0) {

		Token dot = nextToken();

		if (dot.type == TOKEN_DOT) {
			Token paramName = nextToken();

			if (paramName.type == TOKEN_IDENTIFIER) {

				Token next = nextToken();

				// Скобки
				if (next.type == TOKEN_LEFT_BRACKET) {
					vector<AST> children = parseAllArgsChildren(fromClass);

					next = nextToken();
					if (next.type != TOKEN_RIGHT_BRACKET)
						throw std::runtime_error("Expected ')'!");

					return AST(token, AST(paramName, children));
				}
				else {
					returnBackToken(next);
					return AST(token, paramName);
				}
			}

			returnBackToken(paramName);
		}

		returnBackToken(dot);
	}

	return returnBackToken(token);
};

AST Parser::parseCondition(bool fromClass) {
	int type[] = { KEYWORD_LOGICAL_OPERATION,  TOKEN_COMPA_OPER, KEYWORD_BOOLEAN };
	AST item = parseBinaryExpression(0, type, fromClass);
	return item;
}

AST Parser::parseArithmeticExpr(bool fromClass) {
	int type[] = { TOKEN_ARITHMETIC_OPER };
	AST item = parseBinaryExpression(0, type, fromClass);
	return item;
};



int Parser::getPriority(const string& binaryOp) {
	if (binaryOp == "or") return 1;
	if (binaryOp == "and") return 2;
	if (binaryOp == "not") return 3;

	if (binaryOp == "=="
		|| binaryOp == "<"
		|| binaryOp == ">"
		|| binaryOp == "<="
		|| binaryOp == ">="
		|| binaryOp == "!="
		|| binaryOp == "<>") return 4;


	if (binaryOp == "+" || binaryOp == "-") return 5;


	if (binaryOp == "*" 
		|| binaryOp == "%"
		|| binaryOp == "/") return 6;

	return 0;
}

AST Parser::parseWhile(int level, bool fromDef, bool fromClass) {
	Token token = nextToken();
	vector<AST> children{};

	if (token.type == KEYWORD_WHILE) {
		AST condition = parseArgs(fromClass);

		// Condition
		if (condition.token.type != -1) {
			children.push_back(condition);

			Token next = nextToken();

			if (next.type == TOKEN_COLON) {
				vector<AST> bodyChildren = parseAllBodyChildren(level, true, fromDef, fromClass);
				children.push_back(AST(next, bodyChildren));
				return AST(token, children);
			}
		}
	}

	return returnBackToken(token);
}

vector<AST> Parser::parseAllBodyChildren(int level, bool fromLoop, bool fromDef, bool fromClass) {
	vector<AST> bodyChildren{};

	AST child = parseBody(level + 1, fromLoop, fromDef, fromClass);

	while (child.token.type != -1) {
		bodyChildren.push_back(child);
		child = parseBody(level + 1, fromLoop, fromDef, fromClass);
	}

	return bodyChildren;
}

vector<AST> Parser::parseAllArgsChildren(bool fromClass) {
	vector<AST> argsChildren{};

	
	Token token = nextToken();

	if (token.type == KEYWORD_SELF) {
		if (fromClass) {
			argsChildren.push_back(token);

			token = nextToken();

			if (token.type != TOKEN_COMMA && token.type != TOKEN_RIGHT_BRACKET) {
				returnBackToken(token);
				throw std::invalid_argument("Syntax error!");
			}
		}
		else {
			returnBackToken(token);
			throw std::invalid_argument("Syntax error!");
		}		
	}
	else {
		returnBackToken(token);
	}
	
	
	AST child = parseArgs(fromClass);

	while (child.token.type != -1) {
		argsChildren.push_back(child);
		Token next = nextToken();

		if (next.type != TOKEN_COMMA) {
			returnBackToken(next);
			break;
		}

		child = parseArgs(fromClass);
	}

	return argsChildren;
};

AST Parser::parseFor(int level, bool fromDef, bool fromClass) {
	Token token = nextToken();
	vector<AST> children{};

	if (token.type == KEYWORD_FOR) {
		Token id = nextToken();

		if (id.type == TOKEN_IDENTIFIER) {
			children.push_back(id);
			Token in = nextToken();

			if (in.type == KEYWORD_IN) {
				Token next = nextToken();

				if (next.type == KEYWORD_RANGE) {
					Token br = nextToken();

					if (br.type == TOKEN_LEFT_BRACKET) {
						Token arg = nextToken();

						if (arg.type == TOKEN_NUMBER || TOKEN_IDENTIFIER) {

							if (nextToken().type != TOKEN_RIGHT_BRACKET) {
								throw std::runtime_error("Expected ')'!");
							}

							children.push_back(AST(next, arg));
						}
						else {
							returnBackToken(arg);
						}
					}
					else {
						returnBackToken(br);
					}
				}
				else {
					returnBackToken(next);
					AST item = parseList();

					if (item.token.type != -1) {
						children.push_back(item);
					}
					else {
						Token arg = nextToken();

						if (arg.type == TOKEN_STRING_CONST || TOKEN_IDENTIFIER) {
							children.push_back(arg);
						}
						else {
							returnBackToken(arg);
						}
						
					}
				}

				Token c = nextToken();

				if (c.type == TOKEN_COLON) {
					vector<AST> bodyChildren = parseAllBodyChildren(level, true, fromDef, fromClass);
					children.push_back(AST(c, bodyChildren));
					return AST(token, children);
				}

				returnBackToken(c);

			}
			returnBackToken(in);
		}
		returnBackToken(id);
	}

	return returnBackToken(token);
}

AST Parser::parseLoopOperators() {
	Token token = nextToken();
	if (token.type == KEYWORD_BREAK || token.type == KEYWORD_CONTINUE) {
		return AST(token);
	}
	return returnBackToken(token);
}
AST Parser::parseDefReturn(bool fromClass) {
	Token token = nextToken();
	if (token.type == KEYWORD_RETURN) {
		AST result = parseArgs(fromClass);
		return AST(token, result);
	}
	return returnBackToken(token);
};
AST Parser::parseList() {
	Token token = nextToken();

	if (token.type == TOKEN_SQUARE_LEFT_BRACKET) {
		vector<AST> children = parseAllArgsChildren(false);

		Token next = nextToken();
		if (next.type != TOKEN_SQUARE_RIGHT_BRACKET)
			throw std::runtime_error("Expected ']'!");

		return AST(Token("[]", TOKEN_SQUARE_LEFT_BRACKET), children);
	}
	return returnBackToken(token);
}

AST Parser::parseIfElifElse(int level, bool fromLoop, bool fromDef, bool fromClass) {
	vector<AST> ifChildren = parseIfElifElseSeg(level, KEYWORD_IF, fromLoop, fromDef, fromClass);

	if (ifChildren.empty()) {
		return AST(Token("", -1));
	}

	vector<AST> elifChildren = parseIfElifElseSeg(level, KEYWORD_ELIF, fromLoop, fromDef, fromClass);

	while (!elifChildren.empty()) {
		ifChildren.insert(ifChildren.end(), elifChildren.begin(), elifChildren.end());
		elifChildren = parseIfElifElseSeg(level, KEYWORD_ELIF, fromLoop, fromDef, fromClass);
	}

	vector<AST> elseChildren = parseIfElifElseSeg(level, KEYWORD_ELSE, fromLoop, fromDef, fromClass);

	if (!elseChildren.empty()) {
		ifChildren.insert(ifChildren.end(), elseChildren.begin(), elseChildren.end());
	}

	if (!ifChildren.empty()) {
		return AST(Token("if", KEYWORD_IF), ifChildren);
	}

	return AST(Token("", -1));
}

vector<AST> Parser::parseIfElifElseSeg(int level, int keyword, bool fromLoop, bool fromDef, bool fromClass) {
	bool canParseSeg = true;
	vector<AST> children{};

	if (keyword != KEYWORD_IF)
		canParseSeg = parseTab(level);

	if (canParseSeg) {
		Token token = nextToken();

		if (token.type == keyword) {

			if (keyword != KEYWORD_ELSE) {
				AST condition = parseArgs(fromClass);
				if (condition.token.type != -1) {
					children.push_back(condition);
				}
			}
					
			Token next = nextToken();

			if (next.type == TOKEN_COLON) {
				vector<AST> bodyChildren = parseAllBodyChildren(level, fromLoop, fromDef, fromClass);
				children.push_back(AST(next, bodyChildren));
				return children;
			}

			returnBackToken(next);
		}

		if (token.type != -1) {
			returnBackToken(token);
			if (keyword != KEYWORD_IF) {
				for (int i = 0; i < level; i++)
					returnBackToken(Token("<tab>", TOKEN_TAB));
			}
		}
			
	}

	return children;
};

AST Parser::parseDef(int level, bool fromLoop, bool fromDef, bool fromClass) {
	Token token = nextToken();

	if (token.type == KEYWORD_DEF) {
		vector<AST> children{};
		Token defName = nextToken();

		// Название функции
		if (defName.type == TOKEN_IDENTIFIER && tokens.size() != 0) {
			
			Token next = nextToken();

			// Скобки
			if (next.type == TOKEN_LEFT_BRACKET) {
				vector<AST> defArgs = parseAllArgsChildren(fromClass);

				next = nextToken();
				if (next.type != TOKEN_RIGHT_BRACKET)
					throw std::runtime_error("Expected ')'!");

				children.push_back(AST(defName, defArgs));

				Token colon = nextToken();

				if (colon.type == TOKEN_COLON) {
					vector<AST> bodyChildren = parseAllBodyChildren(level, fromLoop, fromDef, fromClass);
					children.push_back(AST(colon, bodyChildren));
					return AST(token, children);
				}

				tokens.push_front(colon);
			}
			tokens.push_front(next);
		}

		tokens.push_front(defName);

	}

	return returnBackToken(token);
}

bool Parser::parseTab(int level) {
	if (tokens.size() == 0) return false;

	while (tokens.size() != 0) {
		for (int i = 0; i < level; i++) {
			Token token = nextToken();
			if (token.type != TOKEN_TAB || tokens.size() == 0) {
				if (token.type != -1) tokens.push_front(token);
				for (int j = 0; j < i; j++)
					tokens.push_front(Token("<tab>", TOKEN_TAB));
				return false;
			}
		}
		break;
	}
	return true;
}

AST Parser::parseBody(int level, bool fromLoop, bool fromDef, bool fromClass) {

	if (!parseTab(level)) return AST(Token("", -1));

	AST item = parseAssignment(fromClass);

	if (item.token.type != -1) {
		return item;
	}

	item = parseDefCall();
	if (item.token.type != -1) {
		return item;
	}

	item = parseIfElifElse(level, fromLoop, fromDef, fromClass);
	if (item.token.type != -1) {
		return item;
	}

	item = parseWhile(level, fromDef, fromClass);
	if (item.token.type != -1) {
		return item;
	}

	item = parseFor(level, fromDef, fromClass);
	if (item.token.type != -1) {
		return item;
	}

	item = parseClassStatement(fromClass);

	if (item.token.type != -1) {
		return item;
	}

	if (fromLoop) {
		item = parseLoopOperators();
		if (item.token.type != -1) {
			return item;
		}
	}

	if (fromDef) {
		item = parseDef(level, fromLoop, fromDef, fromClass);
		if (item.token.type != -1) {
			return item;
		}

		item = parseDefReturn(fromClass);
		if (item.token.type != -1) {
			return item;
		}
	}

	return item;
}

AST Parser::parseClass(int level) {
	Token token = nextToken();

	if (token.type == KEYWORD_CLASS) {
		vector<AST> children{};
		Token className = nextToken();

		// Название функции
		if (className.type == TOKEN_IDENTIFIER && tokens.size() != 0) {
			children.push_back(className);

			Token colon = nextToken();

			if (colon.type == TOKEN_COLON) {
				vector<AST> methodChildren = parseAllClassBodyChildren(level + 1);
				children.insert(children.end(), methodChildren.begin(), methodChildren.end());
				return AST(token, children);
			}

			tokens.push_front(colon);
		}

		tokens.push_front(className);

	}

	return returnBackToken(token);
}
AST Parser::parseClassInit(int level) {
	if (!parseTab(level)) return AST(Token("", -1));

	Token token = nextToken();

	if (token.type == KEYWORD_DEF) {
		vector<AST> children{};
		Token defName = nextToken();

		// Название функции
		if (defName.type == KEYWORD___INIT__ && tokens.size() != 0) {

			Token next = nextToken();

			// Скобки
			if (next.type == TOKEN_LEFT_BRACKET) {
				vector<AST> defArgs = parseAllArgsChildren(true);

				next = nextToken();
				if (next.type != TOKEN_RIGHT_BRACKET)
					throw std::runtime_error("Expected ')'!");

				children.push_back(AST(defName, defArgs));

				Token colon = nextToken();

				if (colon.type == TOKEN_COLON) {
					vector<AST> bodyChildren = parseAllBodyChildren(level, 0, true, true);
					if (!bodyChildren.empty()) {
						children.push_back(AST(colon, bodyChildren));
						return AST(token, children);
					}
					
				}

				tokens.push_front(colon);
			}
			tokens.push_front(next);
		}

		tokens.push_front(defName);

	}

	return returnBackToken(token);
}
vector<AST> Parser::parseAllClassBodyChildren(int level) {
	vector<AST> bodyChildren{};
	AST child = parseClassInit(level);

	if (child.token.type == -1) {
		return bodyChildren;
	}

	bodyChildren.push_back(child);

	do {
		if (!parseTab(level)) return bodyChildren;

		child = parseDef(level, false, true, true);

		if (child.token.type != -1) {
			bodyChildren.push_back(child);
			continue;
		}

		child = parseClass(level);

		if (child.token.type != -1) {
			bodyChildren.push_back(child);
			continue;
		}

	} while (child.token.type != -1);

	return bodyChildren;
}