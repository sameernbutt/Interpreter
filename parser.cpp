/* Implementation of Recursive-Descent Parser
	for a Simple Pasacal-Like Language
 * parser.cpp
 * Programming Assignment 2
 * Fall 2023
*/

#include "parser.h"
#include "lex.cpp"
#include "lex.h"
#include "iostream"
#include "map"

map<string, bool> defVar;
map<string, Token> SymTable;

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if( pushed_back ) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem & t) {
		if( pushed_back ) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;	
	}

}

static int error_count = 0;

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

bool IdentList(istream& in, int& line);


//Program is: Prog ::= PROGRAM IDENT ; DeclPart CompoundStmt .
bool Prog(istream& in, int& line)
{
	bool f1, f2;
	LexItem tok = Parser::GetNextToken(in, line);
	
	
	if (tok.GetToken() == PROGRAM) {
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == IDENT) {
			string identstr = tok.GetLexeme();
			if (!(defVar.find(identstr)->second))
			{
				defVar[identstr] = true;
				
			}
			tok = Parser::GetNextToken(in, line);
			if (tok.GetToken() == SEMICOL) {
				f1 = DeclPart(in, line); 
			
				if(f1) {
					tok = Parser::GetNextToken(in, line);
					if(tok == BEGIN)
					{
						f2 = CompoundStmt(in, line);
						if(!f2)
						{
							ParseError(line, "Incorrect Program Body.");
							return false;
						}
						tok = Parser::GetNextToken(in, line);
						if (tok.GetToken() == DOT)
							return true;//Successful Parsing is completed
						else
						{
							ParseError(line, "Missing end of program dot.");
							return false;
						}
					}
					else
					{
						ParseError(line, "Missing program body.");
						return false;
					}
					
				}
				else
				{
					ParseError(line, "Incorrect Declaration Section.");
					return false;
				}
			}
			else
			{
				
				ParseError(line-1, "Missing Semicolon.");
				return false;
			}
		}
		else
		{
			ParseError(line, "Missing Program Name.");
			return false;
		}
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else if(tok.GetToken() == DONE && tok.GetLinenum() <= 1){
		ParseError(line, "Empty File");
		return true;
	}
	ParseError(line, "Missing PROGRAM Keyword.");
	return false;
}//End of Prog

//DeclPart ::= VAR DeclStmt { ; DeclStmt }
bool DeclPart(istream& in, int& line) {
	bool status = false;
	LexItem tok;

	LexItem t = Parser::GetNextToken(in, line);
	if(t == VAR)
	{
		status = DeclStmt(in, line);
		
		while(status)
		{
			tok = Parser::GetNextToken(in, line);
			if(tok != SEMICOL)
			{
				
				ParseError(line, "Missing semicolon in Declaration Statement.");
				return false;
			}
			status = DeclStmt(in, line);
		}
		
		tok = Parser::GetNextToken(in, line);
		if(tok == BEGIN )
		{
			Parser::PushBackToken(tok);
			return true;
		}
		else 
		{
			ParseError(line, "Syntactic error in Declaration Block.");
			return false;
		}
	}
	else
	{
		ParseError(line, "Non-recognizable Declaration Part.");
		return false;
	}
	
}//end of DeclBlock function

//DeclStmt ::= IDENT {, IDENT } : Type [:= Rxpr]
bool DeclStmt(istream& in, int& line)
{
	LexItem t;
	bool status = IdentList(in, line);
	bool flag;
	
	if (!status)
	{
		ParseError(line, "Incorrect identifiers list in Declaration Statement.");
		return status;
	}
	
	t = Parser::GetNextToken(in, line);
	if(t == COLON)
	{
		t = Parser::GetNextToken(in, line);
		if(t == INTEGER || t == REAL || t == STRING || t == BOOLEAN)
		{
			t = Parser::GetNextToken(in, line);
			if(t == ASSOP)
			{
				flag = Expr(in, line);
				if(!flag)
				{
					ParseError(line, "Incorrect initialization expression.");
					return false;
				}
			}
			else
			{
				Parser::PushBackToken(t);
			}
			return true;
		}
		else
		{
			ParseError(line, "Incorrect Declaration Type.");
			return false;
		}
	}
	else
	{
		Parser::PushBackToken(t);
		
		return false;
	}
	
}//End of DeclStmt

//IdList:= IDENT {,IDENT}
bool IdentList(istream& in, int& line) {
	bool status;
	string identstr;
	
	LexItem tok = Parser::GetNextToken(in, line);
	if(tok == IDENT)
	{
		//set IDENT lexeme to the type tok value
		identstr = tok.GetLexeme();
		if (!(defVar.find(identstr)->second))
		{
			defVar[identstr] = true;
			
		}	
		else
		{
			ParseError(line, "Variable Redefinition");
			return false;
		}
		
	}
	else
	{
		Parser::PushBackToken(tok);
		
		return true;
	}
	
	tok = Parser::GetNextToken(in, line);
	
	if (tok == COMMA) {
		status = IdentList(in, line);	
	}
	
	else if(tok == COLON)
	{
		Parser::PushBackToken(tok);
		return true;
	}
	else if(tok == IDENT)
	{
		ParseError(line, "Missing comma in declaration statement.");
		return false;
	}
	else {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	return status;
}//End of IdentList
	

//Stmt is either a simple statement or a structured one
//Stmt ::= SimpleStmt | StructuredStmt 
bool Stmt(istream& in, int& line) {
	bool status = false;
	//cout << "in ContrlStmt" << endl;
	LexItem t = Parser::GetNextToken(in, line);
	if(t == WRITELN || t == WRITE || t == IDENT)
	{
		Parser::PushBackToken(t);
		status = SimpleStmt(in, line);
		if (!status)
		{
			ParseError(line, "Incorrect Simple Statement.");
			return status;
		}
		return status;
	}
	else if( t == IF || t == BEGIN)
	{
		Parser::PushBackToken(t);
		status = StructuredStmt(in, line);
		if (!status)
		{
			ParseError(line, "Incorrect Structured Statement.");
			return status;
		}
		return status;
	}
	else if(t == END)
	{
		cout << "in stmt: " << t.GetLexeme() << endl;
		Parser::PushBackToken(t);
		return false;
	}
	return status;
}//End of Stmt
	
//StructuredStmt ::= IfStmt | CompoundStmt
bool StructuredStmt(istream& in, int& line) {
	bool status;
	
	LexItem t = Parser::GetNextToken(in, line);
	switch( t.GetToken() ) {

	case IF: //Keyword consumed
		status = IfStmt(in, line);
		
		break;

	case BEGIN: //Keyword consumed
		status = CompoundStmt(in, line);
		break;
		
	default:
		;
	}

	return status;
}//End of StructuredStmt


//CompoundStmt ::= BEGIN Stmt {; Stmt } END
bool CompoundStmt(istream& in, int& line) {
	bool status;
	LexItem tok;
	status = Stmt(in, line);
	tok = Parser::GetNextToken(in, line);
	while(status && tok == SEMICOL)
	{
		status = Stmt(in, line);
		tok = Parser::GetNextToken(in, line);
	}
	if(!status)
	{
		ParseError(line, "Syntactic error in the statement.");
		Parser::PushBackToken(tok);	
		return false;
	}
	if(tok == END)
		return true;
	else if(tok == ELSE)
	{
		Parser::PushBackToken(tok);
		return true;
	}
	else
	{
		
		ParseError(line, "Missing end of compound statement.");
		return false;
	}		
}//end of CompoundStmt



//SimpleStmt ::= AssignStmt | WriteLnStmt | WriteStmt
bool SimpleStmt(istream& in, int& line) {
	bool status;
	
	LexItem t = Parser::GetNextToken(in, line);
	
	switch( t.GetToken() ) {

	case WRITELN: //Keyword is consumed
		status = WriteLnStmt(in, line);
		
		break;

	case WRITE: //Keyword is consumed
		status = WriteStmt(in, line);
		break;

	case IDENT: //Keyword is not consumed
		Parser::PushBackToken(t);
        status = AssignStmt(in, line);
		
		break;
	default:
	;	
	}

	return status;
}//End of SimpleStmt


//WriteLnStmt ::= writeln (ExprList) 
bool WriteLnStmt(istream& in, int& line) {
	LexItem t;
	
	
	t = Parser::GetNextToken(in, line);
	if( t != LPAREN ) {
		
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}
	
	bool ex = ExprList(in, line);
	
	if( !ex ) {
		ParseError(line, "Missing expression list for WriteLn statement");
		return false;
	}
	
	t = Parser::GetNextToken(in, line);
	if(t != RPAREN ) {
		
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}
	
	return ex;
}//End of WriteLnStmt

//WriteStmt ::= write (ExprList) 
bool WriteStmt(istream& in, int& line) {
	LexItem t;
	
	t = Parser::GetNextToken(in, line);
	if( t != LPAREN ) {
		
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}
	
	bool ex = ExprList(in, line);
	
	if( !ex ) {
		ParseError(line, "Missing expression list for Write statement");
		return false;
	}
	
	t = Parser::GetNextToken(in, line);
	if(t != RPAREN ) {
		
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}
	
	return ex;
}//End of WriteStmt

//IfStmt ::= IF Expr THEN Stmt [ ELSE Stmt ] 
bool IfStmt(istream& in, int& line) {
	bool ex=false, status ; 
	LexItem t;
			
	ex = Expr(in, line);
	if( !ex ) {
		ParseError(line, "Missing if statement Logic Expression");
		return false;
	}
	
	t = Parser::GetNextToken(in, line);
	if(t != THEN)
	{
		ParseError(line, "If-Stmt Syntax Error");
		return false;
	}
	status = Stmt(in, line);
	if(!status)
	{
		ParseError(line, "Missing Statement for If-Stmt Then-Part");
		return false;
	}
	t = Parser::GetNextToken(in, line);
	if( t == ELSE ) {
		status = Stmt(in, line);
		if(!status)
		{
			ParseError(line, "Missing Statement for If-Stmt Else-Part");
			return false;
		}
		return true;
	}
		
	Parser::PushBackToken(t);// semicolon pushed back or anything else
	return true;
}//End of IfStmt function

//Var ::= ident
bool Var(istream& in, int& line)
{
	string identstr;
	
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == IDENT){
		identstr = tok.GetLexeme();
		
		if (!(defVar.find(identstr)->second))
		{
			ParseError(line, "Undeclared Variable");
			return false;
		}	
		return true;
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	return false;
}//End of Var

//AssignStmt:= Var := Expr
bool AssignStmt(istream& in, int& line) {
	bool varstatus = false, status = false;
	LexItem t;
	
	varstatus = Var( in, line);
		
	if (varstatus){
		t = Parser::GetNextToken(in, line);
		
		if (t == ASSOP){
			status = Expr(in, line);
			if(!status) {
				ParseError(line, "Missing Expression in Assignment Statement");
				return status;
			}
			
		}
		else if(t.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << t.GetLexeme() << ")" << endl;
			return false;
		}
		else {
			ParseError(line, "Missing Assignment Operator");
			return false;
		}
	}
	else {
		ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
		return false;
	}
	return status;	
}

//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	bool status = false;
	
	status = Expr(in, line);
	if(!status){
		ParseError(line, "Missing Expression");
		return false;
	}
	
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == COMMA) {
		
		status = ExprList(in, line);
		
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else{
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
}//ExprList

//Expr ::= LogORExpr ::= LogANDExpr { || LogANDRxpr }
bool Expr(istream& in, int& line) {
	//LogORExpr
	LexItem tok;
	bool t1 = LogANDExpr(in, line);
		
	if( !t1 ) {
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while ( tok == OR ) 
	{
		t1 = LogANDExpr(in, line);
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		
		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
		
	}
	Parser::PushBackToken(tok);
	return true;
}//End of Expr/LogORExpr

//LogAndExpr ::= RelExpr {AND RelExpr }
bool LogANDExpr(istream& in, int& line) {
	LexItem tok;
	bool t1 = RelExpr(in, line);
		
	if( !t1 ) {
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while ( tok == AND ) 
	{
		t1 = RelExpr(in, line);
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		
		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
		
	}
	
	Parser::PushBackToken(tok);
	return true;
}//End of LogANDExpr

//RelExpr ::= SimpleExpr  [ ( = | < | > ) SimpleExpr ]
bool RelExpr(istream& in, int& line) {
	LexItem tok;
	bool t1 = SimpleExpr(in, line);
		
	if( !t1 ) {
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	if ( tok == LTHAN || tok == GTHAN || tok == EQ) 
	{
		t1 = SimpleExpr(in, line);
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		
		tok = Parser::GetNextToken(in, line);
		
		if(tok == LTHAN || tok == GTHAN || tok == EQ)
		{
			ParseError(line, "Illegal Relational Expression.");
			return false;
		}
		else if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
		
	}
	Parser::PushBackToken(tok);
	return true;
}//End of RelExpr

//Expr:= Term {(+|-) Term}
bool SimpleExpr(istream& in, int& line) {
	
	bool t1 = Term(in, line);
	LexItem tok;
	
	if( !t1 ) {
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while ( tok == PLUS || tok == MINUS ) 
	{
		t1 = Term(in, line);
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		
		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
		
	}
	Parser::PushBackToken(tok);
	return true;
}//End of Expr

//Term:= SFactor {( * | / | DIV | MOD) SFactor}
bool Term(istream& in, int& line) {
	bool t1 = SFactor(in, line);
	LexItem tok;
	
	if( !t1 ) {
		return false;
	}
	
	tok	= Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
	}
	while ( tok == MULT || tok == DIV  || tok == IDIV || tok == MOD)
	{
		t1 = SFactor(in, line);
		
		if( !t1 ) {
			ParseError(line, "Missing operand after operator");
			return false;
		}
		
		tok	= Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
		
	}
	Parser::PushBackToken(tok);
	return true;
}//End of Term

//SFactor ::= [( - | + | NOT )] Factor
bool SFactor(istream& in, int& line)
{
	LexItem t = Parser::GetNextToken(in, line);
	bool status;
	int sign = 0;
	if(t == MINUS )
	{
		sign = -1;
	}
	else if(t == PLUS)
	{
		sign = 1;
	}
	else if(t == NOT)
	{
		sign = -2;//sign is a NOT op for logic operands
	}
	else
		Parser::PushBackToken(t);
		
	status = Factor(in, line, sign);
	return status;
}//End of SFactor

//Factor ::= IDENT | ICONST | RCONST | SCONST | BCONST | (Expr)
bool Factor(istream& in, int& line, int sign) {
	
	LexItem tok = Parser::GetNextToken(in, line);
	

	if( tok == IDENT ) {
		string lexeme = tok.GetLexeme();
		if (!(defVar.find(lexeme)->second))
		{
			ParseError(line, "Using Undefined Variable");
			return false;	
		}
		return true;
	}
	else if( tok == ICONST ) {
		
		return true;
	}
	else if( tok == SCONST ) {
		
		return true;
	}
	else if( tok == RCONST ) {
		
		return true;
	}
	else if( tok == BCONST ) {
		
		return true;
	}
	else if( tok == LPAREN ) {
		bool ex = Expr(in, line);
		if( !ex ) {
			ParseError(line, "Missing expression after (");
			return false;
		}
		if( Parser::GetNextToken(in, line) == RPAREN )
			return ex;
		else 
		{
			Parser::PushBackToken(tok);
			ParseError(line, "Missing ) after expression");
			return false;
		}
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	Parser::PushBackToken(tok);
	
	return false;
}



