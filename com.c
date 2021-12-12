#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
    variables: we're using the set keyword
        set x = y
    
    functions: we're using def like this
        def myfunc(argv) {
            code...
        }
    
    conditionals: basic if else structure
        if (condition) then {
            code a...
        } else {
            code b...
        }

    call funcs: standard, nothing changes
        myfunc(argv)

    iteration: we're using do loops of the following syntax:
        do {
            code...
        } until (COND)
    
*/

struct lex {
    char *src;
    int type;
} typedef lex;

enum {SET, IF, ELSE, DO, UNTIL, DEF, RET, LBR, RBR, LPA, RPA,
      PLS, MIN, STR, SLH, LES, GRT, EQL, NLN, IDE, NUM, EOI};

char *tok;
int vars[26] = {0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0};

int iskey(char *src) {
    char keywords[8][7] = {"set", "if", "else", "do", 
                        "until", "def", "return", NULL};

    for(int i = 0; i < 8; i++) {
        if(strcmp(keywords[i], src) == 0)
            return i;
    }

    return IDE;
}

void setlex(lex **lexer, char *tok, int type, int size) {
    (*lexer) = malloc(sizeof(lex));
    (*lexer)->src = malloc(size + 1);
    memcpy((*lexer)->src, tok, size);
    (*lexer)->src[size] = '\0';
    (*lexer)->type = type;
}

lex *next() {

    lex *imlex = NULL;

    char *peek = tok;
    int len = 0;

    switch (*tok) {
        case EOF: break;
        case ' ': tok++; break;
        case '\t': tok++; break;
        case '\n': setlex(&imlex, "\n", NLN, 1); tok++; break;
        case '{': setlex(&imlex, tok, LBR, 1); tok++; break;
        case '}': setlex(&imlex, tok, RBR, 1); tok++; break;
        case '(': setlex(&imlex, tok, LPA, 1); tok++; break;
        case ')': setlex(&imlex, tok, RPA, 1); tok++; break;
        case '+': setlex(&imlex, tok, PLS, 1); tok++; break;
        case '-': setlex(&imlex, tok, MIN, 1); tok++; break;
        case '*': setlex(&imlex, tok, STR, 1); tok++; break;
        case '/': setlex(&imlex, tok, SLH, 1); tok++; break;
        case '<': setlex(&imlex, tok, LES, 1); tok++; break;
        case '>': setlex(&imlex, tok, GRT, 1); tok++; break;
        case '=': setlex(&imlex, tok, EQL, 1); tok++; break;
        default:
            // if it's a letter
            if(*tok >= 'a' && *tok <= 'z') {
                while(*peek >= 'a' && *peek <= 'z') {
                    peek++; len++;
                }
                tok[len] = '\0';
         
                setlex(&imlex, tok, iskey(tok), len + 1);
            }
            // if it's a decimal number
            else if(*tok >= '0' && *tok <= '9') {
                while(*peek >= '0' && *peek <= '9') {
                    peek++; len++;
                }

                // floating points aren't supported
                if(*peek == '.') {
                    puts("No floating points...");
                    exit(4);
                }

                setlex(&imlex, tok, NUM, len + 1);
            }
            // invalid character encounter
            else {
                printf("invalid char [%c]\n", *tok);
                exit(1);
            }

            tok = peek + 1;

            break;
    }

    return imlex;
}

int main(int argc, char **argv) {

    FILE *fp = fopen(argv[1], "r");
    char buffer[128];
    lex *lexer;

    while(fgets(buffer, 128, fp) != NULL) {
        buffer[strlen(buffer)] = '\0';
        tok = buffer;
        
        while(*tok != '\0') {
            lexer = next();
            if(lexer) {
                printf("[%s] -- [%d]\n", lexer->src, lexer->type);
                free(lexer->src);
                free(lexer);
            }
            while(*tok == ' ')
                tok++;
        }
    }

    fclose(fp);

    return 0;
}

