Grammar and Description for Simple Pascal-Like Language

Prog ::= PROGRAM IDENT ; DeclPart CompoundStmt .
DeclPart ::= VAR DeclStmt { ; DeclStmt }
DeclStmt ::= IDENT {, IDENT } : Type [:= Expr]
Type ::= INTEGER | REAL | BOOLEAN | STRING
Stmt ::= SimpleStmt | StructuredStmt
SimpleStmt ::= AssignStmt | WriteLnStmt | WriteStmt
StructuredStmt ::= IfStmt | CompoundStmt
CompoundStmt ::= BEGIN Stmt {; Stmt } END
WriteLnStmt ::= WRITELN (ExprList)
WriteStmt ::= WRITE (ExprList)
IfStmt ::= IF Expr THEN Stmt [ ELSE Stmt ]
AssignStmt ::= Var := Expr
Var ::= IDENT
ExprList ::= Expr { , Expr }
Expr ::= LogOrExpr ::= LogAndExpr { OR LogAndExpr }
LogAndExpr ::= RelExpr {AND RelExpr }
RelExpr ::= SimpleExpr [ ( = | < | > ) SimpleExpr ]
SimpleExpr ::= Term { ( + | - ) Term }
Term ::= SFactor { ( * | / | DIV | MOD ) SFactor }
SFactor ::= [( - | + | NOT )] Factor
Factor ::= IDENT | ICONST | RCONST | SCONST | BCONST | (Expr)
IDENT ::= Letter {( Letter | Digit | _ | $ )}
Letter ::= [a-z A-Z]
Digit ::= [0-9]
BCONST ::= (TRUE | FALSE )
RCONST ::= ([0-9]+)\.([0-9]*)

| Reserved Word | Token |
|---|---|
| `and` | `AND` |
| `begin` | `BEGIN` |
| `boolean` | `BOOLEAN` |
| `div` | `IDIV` |
| `end` | `END` |
| `else` | `ELSE` |
| `false` | `FALSE` |
| `if` | `IF` |
| `integer` | `INTEGER` |
| `mod` | `MOD` |
| `not` | `NOT` |
| `or` | `OR` |
| `program` | `PROGRAM` |
| `real` | `REAL` |
| `string` | `STRING` |
| `write` | `WRITE` |
| `writeln` | `WRITELN` |
| `var` | `VAR` |