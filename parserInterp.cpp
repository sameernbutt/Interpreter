#include "parserInterp.h"
#include "val.h"
//#include "val.cpp"
#include "lex.h"
//#include "lex.cpp"


map<string, bool> defVar;
map<string, Token> SymTable;

map<string, Value> TempsResults; //Container of temporary locations of Value objects for results of expressions, variables values and constants 
queue <Value> * ValQue; //declare a pointer variable to a queue of Value objects

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
							//cout<<tok<<endl;
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

//IdList:= IDENT {,IDENT}
bool Check(istream& in, int& line, map<string,bool>& varList) {
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
			varList[identstr] = true;
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
		status = Check(in, line,varList);	
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

//DeclStmt ::= IDENT {, IDENT } : Type [:= Rxpr]
bool DeclStmt(istream& in, int& line)
{
	map<string,bool> varList;
	LexItem t;
	bool status = Check(in, line,varList);
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
		bool isInt = false;
		bool isReal = false;
		bool isString = false;
		bool isBoolean = false;
		if(t == INTEGER || t == REAL || t == STRING || t == BOOLEAN)
		{
			for (const auto& variable : varList) {
				if(t==INTEGER){
					SymTable[variable.first]=INTEGER;
					isInt = true;
				}
				else if(t==REAL){
					SymTable[variable.first]=REAL;
					isReal = true;
				}
				else if(t==STRING){
					SymTable[variable.first]=STRING;
					isString = true;
				}
				else if(t==BOOLEAN){
					SymTable[variable.first]=BOOLEAN;
					isBoolean = true;
				}
    		}
			t = Parser::GetNextToken(in, line);
			if(t == ASSOP)
			{
				Value val;
				flag = Expr(in, line, val);
				if(!flag)
				{
					ParseError(line, "Incorrect initialization expression.");
					return false;
				}
				
				if(val.IsInt() && (isInt || isReal)){
					for (const auto& variable : varList) {
						TempsResults[variable.first]=val;
					}
				}
				else if(val.IsReal() && isReal){
					for (const auto& variable : varList) {
						TempsResults[variable.first]=val;
					}
				}
				else if(val.IsString() && isString){
					for (const auto& variable : varList) {
						TempsResults[variable.first]=val;
					}
				}
				else if(val.IsBool() && isBoolean){
					for (const auto& variable : varList) {
						TempsResults[variable.first]=val;
					}
				}
				else{
					//cout<<val<<" and "<<isInt<<isReal<<isString<<isBoolean<<endl;
					ParseError(line,"Mismatched types in declaration");
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

//WriteLnStmt ::= writeln (ExprList) done
bool WriteLnStmt(istream& in, int& line) {
	LexItem t;
	//cout << "in WriteStmt" << endl;
	ValQue = new queue<Value>;
	
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
	
	//Evaluate: print out the list of expressions' values
	while (!(*ValQue).empty())
	{
		Value nextVal = (*ValQue).front();
		cout << nextVal;
		ValQue->pop();
	}
	cout << endl;
	return ex;
}//End of WriteLnStmt

//WriteStmt ::= write (ExprList) 
bool WriteStmt(istream& in, int& line){
	LexItem t;
	//cout << "in WriteStmt" << endl;
	ValQue = new queue<Value>;
	
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
	
	//Evaluate: print out the list of expressions' values
	while (!(*ValQue).empty())
	{
		Value nextVal = (*ValQue).front();
		cout << nextVal;
		ValQue->pop();
	}
	return ex;
}//End of WriteStmt

//SimpleExpr:= Term {(+|-) Term} done
bool SimpleExpr(istream& in, int& line, Value & retVal){
	
	Value val1;
	bool t1 = Term(in, line,val1);
	LexItem tok;
	//cout<<"in simple expr"<<val1<<endl;

	if( !t1 ) {return false;}
	retVal = val1;
	
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while ( tok == PLUS || tok == MINUS ) {
		Token op = tok.GetToken();
		Value otherVal;
		t1 = Term(in, line, otherVal);
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}

		if(op==PLUS){ // add them
			retVal = retVal + otherVal;
		}
		else if(op==MINUS){ // subtract them
			retVal = retVal - otherVal;
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

//RelExpr ::= SimpleExpr  [ ( = | < | > ) SimpleExpr ] done
bool RelExpr(istream& in, int& line, Value & retVal){
	LexItem tok;
	Value val1;
	bool t1 = SimpleExpr(in, line,val1);
		
	if( !t1 ) {
		return false;
	}
	retVal = val1;
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	if ( tok == LTHAN || tok == GTHAN || tok == EQ) 
	{
		Value val2;
		t1 = SimpleExpr(in, line,val2);
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}

		//cout<<"this is retval"<<retVal<<endl;
		if(tok==LTHAN){
			retVal = Value((retVal<val2));
		}
		else if(tok==GTHAN){
			retVal = Value((retVal>val2));
		}
		else if(tok==EQ){
			retVal = Value((retVal==val2));
		}
		if(retVal.IsErr()){
			ParseError(line,"Something went wrong in the inequality");
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

//AssignStmt:= Var := Expr
bool AssignStmt(istream& in, int& line) {
	bool varstatus = false, status = false;
	LexItem t;
	Value val;
	
	varstatus = Var( in, line, t);
	string name = t.GetLexeme();
	//cout<<"THIS VAR IS "<<name<<endl;
		
	if (varstatus){
		t = Parser::GetNextToken(in, line);
		
		if (t == ASSOP){
			status = Expr(in, line, val);
			if(val.IsErr()){
				ParseError(line,"Invalid assignment");
				return false;
			}

			// check to make sure the value type is correct
			Token type = SymTable[name];
			bool invalid = false;
			switch(type){
				case INTEGER:
					if(!val.IsInt()) invalid = true;
					if(val.IsReal()){ // change to int if need be
						invalid = false;
						double oldNum = val.GetReal();
						int newInt = static_cast<int>(oldNum);
						val = Value(newInt);
					}
					break;
				case STRING:
					if(!val.IsString()) invalid = true;
					break;
				case BOOLEAN:
					if(!val.IsBool()) invalid = true;
					break;
				case REAL:
					if(!val.IsReal() && !val.IsInt()) invalid = true;
					if(val.IsInt()){ // change to real if need be
						int oldNum = val.GetInt();
						double newNum = static_cast<double>(oldNum);
						val = Value(newNum);
					}
					break;
				default:
					break;
			}
			if(invalid){
				ParseError(line, "Mismatched value types");
				return false;
			}

			//cout<<"Variable "<<name<<" assigned value "<<val<<endl;
			TempsResults[name] = val;
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

//Var ::= ident done?
bool Var(istream& in, int& line, LexItem & idtok)
{
	string identstr;
	
	idtok = Parser::GetNextToken(in, line);
	
	if (idtok == IDENT){
		identstr = idtok.GetLexeme();
		
		if (!(defVar.find(identstr)->second))
		{
			ParseError(line, "Undeclared Variable");
			return false;
		}	
		return true;
	}
	else if(idtok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << idtok.GetLexeme() << ")" << endl;
		return false;
	}
	return false;
}//End of Var

//ExprList:= Expr {,Expr} done
bool ExprList(istream& in, int& line) {
	bool status = false;
	Value retVal;
	//cout << "in ExprList and before calling Expr" << endl;
	status = Expr(in, line, retVal);
	if(!status){
		ParseError(line, "Missing Expression");
		return false;
	}
	ValQue->push(retVal);
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == COMMA) {
		//cout << "before calling ExprList" << endl;
		status = ExprList(in, line);
		//cout << "after calling ExprList" << endl;
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

// LogAndExpr ::= RelExpr {AND RelExpr } done
bool LogANDExpr (istream& in, int& line, Value & retVal) { 
	Value val1, val2;
	bool t1 = RelExpr(in,line,val1);
	LexItem lex;
	if(!t1){
		return false;
	}
	retVal = val1;
	lex = Parser::GetNextToken(in,line);
	if(lex.GetToken()==ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << lex.GetLexeme() << ")" << endl;
		return false;
	}
	while(lex==AND){
		t1 = RelExpr(in,line,val2);
		if(!t1){
			ParseError(line, "Missing operand after operator");
			return false;
		}
		retVal = retVal && val2;
		if(retVal.IsErr()){
			ParseError(line,"Weird semantics in LogANDExpr");
			return false;
		}
		lex = Parser::GetNextToken(in,line);
		if(lex.GetToken()==ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << lex.GetLexeme() << ")" << endl;
			return false;
		}
	}
	Parser::PushBackToken(lex);
	return true;
}

//Factor ::= IDENT | ICONST | RCONST | SCONST | BCONST | (Expr) done
bool Factor(istream& in, int& line, Value & retVal, int sign) {
	
	LexItem tok = Parser::GetNextToken(in, line);
	//cout<<"Factor token "<<tok<<endl;
	//cout<<"sign "<<sign<< endl;
	

	if( tok == IDENT ) {
		string lexeme = tok.GetLexeme();
		if (!(defVar.find(lexeme)->second))
		{
			ParseError(line, "Using Undefined Variable");
			return false;	
		}
		else{
			retVal = Value(TempsResults[lexeme]);
		}
		if(retVal.IsString() || retVal.IsBool()){
			if(sign==1 || sign==-1){
				ParseError(line, "cannot use signs with strings");
				return false;
			}
		}
		else if(retVal.IsInt() || retVal.IsReal()){
			if(sign==-2){
				ParseError(line, "cannot use NOT with numbers");
				return false;
			}
		}
		return true;
	}
	else if( tok == ICONST ) {
		int num = stoi(tok.GetLexeme());
		if(sign==1 || sign==-1) num = num*sign;
		else if(sign==-2){
			ParseError(line,"cannot use NOT with numbers");
			return false;
		}
		//cout<<"num is "<<num<<endl;
		retVal = Value(num);
		return true;
	}
	else if( tok == SCONST ) {
		if(sign==1 || sign==-1){
			ParseError(line, "cannot use signs with strings");
			return false;
		}
		retVal = Value(tok.GetLexeme());
		return true;
	}
	else if( tok == RCONST ) {
		double num = stod(tok.GetLexeme());
		if(sign==1 || sign==-1) num = num*sign;
		else if(sign==-2){
			ParseError(line, "cannot use NOT with numbers");
			return false;
		}
		retVal = Value(num);
		return true;
	}
	else if( tok == BCONST ) {
		bool negate = false;
		if(sign==-2) negate = true; // negate the boolean if -2 is passed as the sign
		string lexeme = tok.GetLexeme();
		if(lexeme=="true") {
			if(negate) retVal = Value(false);
			else retVal = Value(true);
		}
		else if(lexeme=="false"){ 
			if(negate) retVal = Value(true);
			else retVal = Value(false);
		}
		return true;
	}
	else if( tok == LPAREN ) {
		bool ex = Expr(in, line, retVal);
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

//SFactor ::= [( - | + | NOT )] Factor
bool SFactor(istream& in, int& line, Value & retVal){
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
	else Parser::PushBackToken(t);
	
	Value val1;
	status = Factor(in, line, val1,sign);
	retVal = val1;
	return status;
}//End of SFactor

//Term:= SFactor {( * | / | DIV | MOD) SFactor}
bool Term(istream& in, int& line, Value & retVal) {
	Value val1;
	bool t1 = SFactor(in, line, val1);
	LexItem tok;
	//cout<<"term "<< val1<<endl;
	if( !t1 ) {
		return false;
	}
	
	retVal = val1;
	
	tok	= Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
	}
	while ( tok == MULT || tok == DIV  || tok == IDIV || tok == MOD)
	{
		Token op = tok.GetToken();
		Value val2;
		t1 = SFactor(in, line,val2);
		
		if( !t1 ) {
			ParseError(line, "Missing operand after operator");
			return false;
		}

		// do the operation
		if(op==MULT){
			retVal = retVal*val2;
		}
		else if(op==DIV){
			retVal = retVal.div(val2);
		}
		else if(op==IDIV){
			retVal = retVal.idiv(val2);
		}
		else if(op==MOD){
			retVal = retVal%val2;
		}
		else cout<<"something went wrong here in Term"<<endl;
		if(retVal.IsErr()){
			ParseError(line, "strange math");
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

void skipStatement(istream& in, int& line){
	LexItem lex = Parser::GetNextToken(in,line);
	while(lex!=END){
		lex = Parser::GetNextToken(in,line);
		if(lex==BEGIN){
			return skipStatement(in,line);
		}
		if(lex==END){
			return;
		}
	}
	return;
}

//IfStmt ::= IF Expr THEN Stmt [ ELSE Stmt ] done
bool IfStmt(istream& in, int& line){
	bool ex=false, status ; 
	LexItem t;
	Value permission; // this is for the condition of the if statement. its gonna be either true or false
			
	ex = Expr(in, line, permission);
	if( !ex ) {
		ParseError(line, "Missing if statement Logic Expression");
		return false;
	}
	
	if(!permission.IsBool()){
		ParseError(line,"Illegal condition for the if statement");
		return false;
	}

	t = Parser::GetNextToken(in, line);
	if(t != THEN)
	{
		ParseError(line, "If-Stmt Syntax Error");
		return false;
	}
	//cout<<"permission "<<permission<<endl;
	if(permission.GetBool()==true){ 
		status = Stmt(in, line);
		if(!status)
			{
				ParseError(line, "Missing Statement for If-Stmt Then-Part");
				return false;
			}
		//cout<<"we good thus far"<<endl;
	}
	else{
		while(t != SEMICOL && t!=ELSE){
			if(t==BEGIN) skipStatement(in,line);
			t=Parser::GetNextToken(in,line);
			if(t==ELSE){
				break;
			}
		}
		Parser::PushBackToken(t);
	}
	t = Parser::GetNextToken(in, line);
	if( t == ELSE ) {
		if(permission.GetBool() == false){
			status = Stmt(in, line);
			if(!status)
			{
				ParseError(line, "Missing Statement for If-Stmt Else-Part");
				return false;
			}
			return true;
		}
		else{
			while(t != SEMICOL){
				if(t==BEGIN) skipStatement(in,line);
				t=Parser::GetNextToken(in,line);
			}
		}
	}
	//cout<<t<<endl;
	Parser::PushBackToken(t);// semicolon pushed back or anything else
	return true;
}//End of IfStmt function



//CompoundStmt ::= BEGIN Stmt {; Stmt } END done
bool CompoundStmt(istream& in, int& line) {
	//cout<<"began compound statement"<<endl;
	bool status;
	LexItem tok;
	status = Stmt(in, line);
	tok = Parser::GetNextToken(in, line);
	while(status && tok == SEMICOL)
	{
		status = Stmt(in, line);
		//cout<<tok<<endl;
		tok = Parser::GetNextToken(in, line);
	}
	if(!status)
	{
		ParseError(line, "Syntactic error in the statement.");
		Parser::PushBackToken(tok);	
		return false;
	}
	//cout<<tok<<endl;
	if(tok == END){
		//cout<<"ended compound statement"<<endl;
		return true;
	}
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

//Expr ::= LogORExpr ::= LogANDExpr { || LogANDRxpr }
bool Expr(istream& in, int& line, Value & retVal){
	//LogORExpr
	LexItem tok;
	Value val1;
	bool t1 = LogANDExpr(in, line, val1);
		
	if( !t1 ) {
		return false;
	}
	
	retVal = val1;
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while ( tok == OR ) 
	{
		Value otherVal;
		t1 = LogANDExpr(in, line,otherVal);
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		if(otherVal.GetBool()==true){ // true or statement makes the whole thing true
			Value temp = Value(true);
			retVal = temp;
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