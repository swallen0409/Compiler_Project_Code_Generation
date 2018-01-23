
%{
 #include <stdio.h>
 #include <stdlib.h>
 #include "node-type.h"
 #include "symtab.h"
 #include "code_gen.h"
    extern struct SymTable SymbolTable;
    struct node *  ASTRoot;
    struct node * newOpNode(int op , int lineCount);
    extern int lineCount;
    extern int check;
    FILE * fp;
    /* Called by yyparse on error.  */
     void
     yyerror (char const *s)
     {
        extern char *yytext;
        //extern int lineCount;
        fprintf (stderr, "%s: at line %d symbol'%s'\n", s,lineCount,yytext);
     }
%}

%union { 
    struct node * node_t ;
}

%token <node_t> ARRAY DO ELSE END FUNCTION IF NOT OF PROCEDURE  
%token <node_t> PROGRAM THEN VAR WHILE 
%token <node_t> IDENTIFIER 
%token <node_t> ASSIGNMENT COLON 
%token <node_t> COMMA 
%token <node_t> DIGSEQ 
%token <node_t> DOT DOTDOT EQUAL GE GT LBRAC LE LPAREN LT 
%token <node_t> MINUS PLUS RBRAC 
%token <node_t> REALNUMBER  
%token <node_t> RPAREN SEMICOLON SLASH  STAR  
%token <node_t> notEQUAL 
%token <node_t> STRING  
%token <node_t> real INTEGER  begin string_v
%token <node_t> READ WRITELN

%type <node_t> prog identifier_list standard_type optional_var 
%type <node_t> procedure_statement relop factor
%type <node_t> subprogram_declaration subprogram_declarations 
%type <node_t> subprogram_head parameter_list expression
%type <node_t> expression_list   arguments   declarations type
%type <node_t> variable tail addop minusop mulop compound_statement optional_statements simple_expression 
%type <node_t> statement statement_list term 
%start prog
%%


prog : PROGRAM IDENTIFIER LPAREN identifier_list RPAREN SEMICOLON
     declarations
     subprogram_declarations
     compound_statement
     DOT {  //create symbol table
            $$ = newNode(NODE_PROGRAM , lineCount);
            addChild($$ , $2);
            deleteNode($4);
            deleteNode($1); 
            deleteNode($3); 
            deleteNode($5); 
            deleteNode($6); 
            addChild($$ , $7); 
            addChild($$ , $8);
            addChild($$ , $9);
            deleteNode($10); 
            ASTRoot = $$;};

identifier_list : IDENTIFIER {
                        $$ = newNode(NODE_LIST, lineCount);
                        addChild($$ , $1);};
        | identifier_list COMMA IDENTIFIER {  
                            $$ = $1;
                            addChild($$ , $3);
                            deleteNode($2); };

declarations : declarations VAR identifier_list COLON type SEMICOLON {
                        $$ = $1;
                        addChild($$ , $3);
                        addChild($$ , $5);
                        deleteNode($2); 
                        deleteNode($4); 
                        deleteNode($6);}; 
                | lambda {
                        $$ = newNode(NODE_DECL, lineCount); //handle declaration problem ex: array int real string_v
                    };

type : standard_type {
            $$ = $1;
        };
        | ARRAY LBRAC DIGSEQ DOTDOT DIGSEQ RBRAC OF type {
            $$ = newNode(NODE_TYPE_ARRAY, lineCount);
            addChild($$ , $3);
            addChild($$ , $5);
            addChild($$ , $8);
            deleteNode($1);
            deleteNode($2);
            deleteNode($4);
            deleteNode($6);
            deleteNode($7);
        };

standard_type : INTEGER {
                $$ = $1;
                $$->nodeType = NODE_TYPE_INT;};
        | real   {
            $$ = $1;
            $$->nodeType = NODE_TYPE_REAL;};
        | STRING {
            $$ = $1;
            $$->nodeType = NODE_TYPE_STRING; };


subprogram_declarations :
	subprogram_declarations subprogram_declaration SEMICOLON {
                                    $$ = $1;
                                    addChild($$ , $2);
                                    deleteNode($3);
                                    };
	| lambda {
            $$ = newNode(NODE_SUB_DECLS, lineCount);
        };

subprogram_declaration : subprogram_head
                             declarations
                            compound_statement {
                $$ = newNode(NODE_LIST, lineCount);
                addChild($$ , $1);
                addChild($$ , $2);
                addChild($$ , $3);};

subprogram_head : FUNCTION IDENTIFIER arguments COLON standard_type SEMICOLON { //create symbol table
                    $$ = newNode(NODE_FUN_HEAD, lineCount);
                    deleteNode($1);
                    addChild($$ , $2);
                    addChild($$ , $3);
                    addChild($$ , $5);
                    deleteNode($4);
                    deleteNode($6);};
	| PROCEDURE IDENTIFIER arguments SEMICOLON { //create symbol table
                    $$ =  newNode(NODE_PRO_HEAD, lineCount);
                    deleteNode($1);
                    addChild($$ , $2);
                    addChild($$ , $3);
                    deleteNode($4);};

arguments : LPAREN parameter_list RPAREN {
                    $$ = $2;
                    deleteNode($1);
                    deleteNode($3);};
	| lambda {
            $$ = newNode(NODE_lambda, lineCount);};


parameter_list : optional_var identifier_list COLON type {
                        $$ = $1;
                        addChild($$ , $2);
                        addChild($$ , $4);
                        deleteNode($3);};
	| optional_var identifier_list COLON type SEMICOLON parameter_list {
            $$ = $6;
            addChild($$ , $2);
            addChild($$ , $4);
            //addChild($$ , $6);
            deleteNode($3);
            deleteNode($5);};

optional_var   : VAR {
                        $$ = newNode(NODE_DECL, lineCount);
                        deleteNode($1);
                     };
        | lambda {$$ = newNode(NODE_DECL, lineCount);};


compound_statement : begin
		       optional_statements
		       END {    // create symbol table
                        $$ = newNode(NODE_BEGIN, lineCount);
                        addChild($$ , $2);
                        struct node * temp = newNode(NODE_END, lineCount);
                        addChild($$ , temp);
                        deleteNode($1);
                        deleteNode($3);
                    };


optional_statements : statement_list {
                            $$ = $1;};


statement_list : statement {
                    $$ = newNode(NODE_LIST, lineCount);
                    addChild($$ , $1);};
	| statement_list SEMICOLON statement {
                    $$ = $1;
                    addChild($$ , $3);
                    deleteNode($2);};

statement : variable ASSIGNMENT expression {
                $$ = newNode(NODE_ASSIGN_STMT, lineCount);
                addChild($$ , $1);
                addChild($$ , $3);
                deleteNode($2);};
	| procedure_statement {
                $$ = $1;
                          };
	| compound_statement {
                $$ = $1;
                         };
	| IF expression THEN statement ELSE statement { 
                        //semantic if here
                        $$ = newNode(NODE_IF, lineCount);
                        addChild($$ , $2);
                        addChild($$ , $4);
                        addChild($$ , $6);
                        deleteNode($1);
                        //deleteNode($3);
                        //deleteNode($5);
                        };
	| WHILE expression DO statement { 
            $$ = newNode(NODE_WHILE, lineCount);
            addChild($$ , $2);
            addChild($$ , $4);
            deleteNode($1);
            deleteNode($3);};
    | READ LPAREN factor RPAREN{
        $$ = newNode(NODE_READ , lineCount);
        addChild($$ , $3);
        deleteNode($1);
        deleteNode($2);
        deleteNode($4);
    };
    |WRITELN LPAREN factor RPAREN {
        $$ = newNode(NODE_WRITELN , lineCount);
        addChild($$ , $3);
        deleteNode($1);
        deleteNode($2);
        deleteNode($4);
    };
	| lambda {$$ = newNode(NODE_LIST, lineCount);};


variable : IDENTIFIER tail {
                $$ = newNode(NODE_SYM_REF, lineCount);
                $$->string =  $1->string;
                addChild($$ , $2);
                };

tail     : LBRAC expression RBRAC tail { 
                    $$ = $4;
                    addChild($$ , newNode(TOKEN_LBRAC, lineCount));
                    addChild($$ , $2);
                    addChild($$ , newNode(TOKEN_RBRAC, lineCount));
                    deleteNode($1);
                    deleteNode($3);
                    };

	| lambda {$$ = newNode(NODE_LIST, lineCount);};


procedure_statement : IDENTIFIER {
                            $$ = newNode(NODE_SYM_REF, lineCount);//without parameter
                            $$->string = $1->string;
                                 };
	| IDENTIFIER LPAREN expression_list RPAREN {
                $$ = newNode(NODE_SYM_REF, lineCount);
                $$->string = $1->string;
                addChild($$ , $3);
                deleteNode($2);
                deleteNode($4);};

expression_list : expression {
                    $$ = newNode(NODE_LIST, lineCount);
                    addChild($$ , $1);
                             };
	| expression_list COMMA expression {
            $$ = $1;
            addChild($$ , $3);
            deleteNode($2);};

expression : simple_expression {
                        $$ = $1;
                               };
	| simple_expression relop simple_expression {
                $$ = $2;
                addChild($$ , $1);
                addChild($$ , $3);};


simple_expression : term {
                        $$ = $1;
                         };
	| simple_expression addop term {
            $$ = $2;
            addChild($$ , $1);
            addChild($$ , $3);};
    | simple_expression minusop term{
            $$ = newOpNode(OP_SUB, lineCount);
            addChild($$ , $1);
            addChild($$ , $3);
    };


term : factor {
            $$ = $1;
              };
	| term mulop factor {
            $$ = $2;
            addChild($$ , $1);
            addChild($$ , $3);};


factor : IDENTIFIER tail { //call declared variable , function , procedure 
            $$ = newNode(NODE_SYM_REF, lineCount);
            $$->string = $1->string;
            addChild($$ , $2);
                         };
	| IDENTIFIER LPAREN expression_list RPAREN {
            $$ = newNode(NODE_SYM_REF, lineCount);
            $$->string = $1->string;
            addChild($$ , $3);
            deleteNode($2);
            deleteNode($4);};
	| DIGSEQ {
        $$ = $1;
        $$->nodeType = NODE_INT;
             };
    | minusop DIGSEQ {
            $2->nodeType = NODE_INT;
            $2->iValue=-$2->iValue;
            $$=$2;
            deleteNode($1);         
                   };
    | REALNUMBER {
        $$ = $1;
        $$->nodeType = NODE_REAL;
                 };
    | minusop REALNUMBER {
            $2->nodeType = NODE_REAL; 
            $2->rValue=-$2->rValue;
            $$=$2;
            deleteNode($1);
                       };
	| LPAREN expression RPAREN {
                $$ = $2;
                deleteNode($1);
                deleteNode($3);};
	| NOT factor {
                    $$ = $2;
                    addChild($$ , $1);};
    | string_v {
            $$ = $1;
            $$->nodeType = NODE_STRING_v;};

addop : PLUS {
                $$ = newOpNode(OP_ADD, lineCount);
                deleteNode($1);};
minusop : MINUS {
                };


mulop : STAR {
                $$ = newOpNode(OP_MUL, lineCount);
                deleteNode($1);};
        | SLASH {
                $$ = newOpNode(OP_DIV, lineCount);
                deleteNode($1);};

relop : LT {
            $$ = newOpNode(OP_LT, lineCount);
            deleteNode($1);};
	| GT {
            $$ = newOpNode(OP_GT, lineCount);
            deleteNode($1);};
	| EQUAL {
            $$ = newOpNode(OP_EQ, lineCount);
            deleteNode($1);};
	| LE {
            $$ = newOpNode(OP_LE, lineCount);
            deleteNode($1);};
	| GE {
            $$ = newOpNode(OP_GE, lineCount);
            deleteNode($1);};
	| notEQUAL {
            $$ = newOpNode(OP_NE, lineCount);
            deleteNode($1);};

lambda : ;

%%

struct node * newOpNode(int op , int lineCount) {
            struct node * node = newNode(NODE_OP, lineCount);
            node->op = op;
            node->lineCount = lineCount;
            return node;
    }

int main(int argc, char** argv) {
    int res;
    
    fprintf(stderr, "open file.\n");
    if(argc>1 && freopen(argv[1],"r",stdin)==NULL){
        printf("fuckleo\n");
        fprintf(stderr , "no file\n");
        exit(1);
    }
    
    fprintf(stderr, "call yyparse\n");
    res = yyparse();
    fprintf(stderr, "after call yyparse, res = %d.\n", res);
    
    if (res==0)
        fprintf(stderr, "SUCCESS\n NO syntax error !\n");
    else
        fprintf(stderr, "ERROR\n");
    
    printf("-----------------------------------------------\n");
    printTree(ASTRoot, 0);

    SymbolTable.size = 0;
    SymbolTable.current_level = 0;
    printf("New scope created at line 1\n");
    semanticCheck(ASTRoot);
    printf("Symbol table created\n");
    printf_symbol_table();
    if(check){
        printf("********************************\n"
               "*      No semantic error!      *\n"
               "********************************\n");
        printf("here\n");
        fp = fopen("foo.j" , "w");
        gen_program_start();
        travel_node(ASTRoot);
        fclose(fp);
    }
    else 
        printf("Semantic error\n");
    
    return 0;
}