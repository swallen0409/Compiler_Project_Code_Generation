#include "code_gen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern int lineCount;
extern FILE * fp;
extern struct SymTable SymbolTable;
int scope = 0;

int function_scope = 0;
int procedure_scope = 0;
int main_scope = 0;
int while_scope = 0;
int if_scope = 0;


/********** generate start of xxx.j *********/
void gen_program_start(){
    fprintf(fp , ".class public foo\n");
    fprintf(fp , ".super java/lang/object\n\n");
    fprintf(fp, ".field public static _sc Ljava/util/Scanner;\n ");
    return;
}
/********** generate end of xxx.j ***********/
void gen_program_end(){
    fprintf(fp, "return\n");
	fprintf(fp, ".end method\n");
    return;
}
/********** DFS to create an java bytecode ***********/
void traverl_node(struct node * node){
    if(node->lsibling->nodeType == NODE_SUB_DECLS)
        scope=1;
    switch(node->nodeType) {
        case NODE_BEGIN:{

        }
        case NODE_FUN_HEAD:{

        }
        case NODE_PRO_HEAD:{

        }
        case NODE_END:{

        }
        case NODE_DECL:{

        }
        case NODE_SYM_REF:{

        }
        case NODE_OP:{

        }
        case NODE_IF:{

        }
        case NODE_WHILE:{

        }
        case NODE_INT:{

        }
        case NODE_REAL:{

        }
        case NODE_STRING_v:{

        }
        case NODE_ASSIGN_STMT:{

        }
    }
    /* Default action for other nodes not listed in the switch-case */
    struct node *child = node->child;
    if(child != NULL) {
        do {
            semanticCheck(child);
            child = child->rsibling;
        } while(child != node->child);
    }
}