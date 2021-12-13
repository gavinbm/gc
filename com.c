#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct lexer {
    char *tok;
    char *pos;
    int type;
} typedef lexer;

enum {SET, IF, ELSE, DO, UNTIL, DEF, RET, LBR, RBR, LPA, RPA,
      PLS, MIN, STR, SLH, LES, GRT, EQL, NLN, IDE, NUM, EOI};

int vars[26] = {0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0};

int iskey(char *src) {
    char keywords[7][7] = {"set", "if", "else", "do", 
                        "until", "def", "return"};

    for(int i = 0; i < 7; i++) {
        if(strcmp(keywords[i], src) == 0)
            return i;
    }

    return IDE;
}

char *readfile(char *filename, int *len) {
    FILE *fp = fopen(filename, "r");
    char *buffer = NULL;
    long length;

    if(fp) {
        fseek(fp, 0, SEEK_END);
        length = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        buffer = malloc(length + 1);
        if(buffer) {
            fread(buffer, 1, length, fp);
        }
        fclose(fp);
    }
    *len = length;
    return buffer;
} 

void initlexer(lexer **lexer, char *tok, int type, int size) {
    if(*lexer == NULL)
        (*lexer) = malloc(sizeof(lexer));

    (*lexer)->tok = malloc(size + 1);
    memcpy((*lexer)->tok, tok, size);
    (*lexer)->tok[size] = '\0';
    (*lexer)->type = type;
}

lexer *next(lexer *old) {

    lexer *new = malloc(sizeof(lexer));
    new->tok = NULL;

    char *peek = old->pos, *substr;
    int len = 0;

    switch (*peek) {
        case EOF: break;
        case ' ': peek++; break;
        case '\t': peek++; break;
        case '\n': initlexer(&new, "\\n", NLN, 2); peek++; break;
        case '{': initlexer(&new, peek, LBR, 1); peek++; break;
        case '}': initlexer(&new, peek, RBR, 1); peek++; break;
        case '(': initlexer(&new, peek, LPA, 1); peek++; break;
        case ')': initlexer(&new, peek, RPA, 1); peek++; break;
        case '+': initlexer(&new, peek, PLS, 1); peek++; break;
        case '-': initlexer(&new, peek, MIN, 1); peek++; break;
        case '*': initlexer(&new, peek, STR, 1); peek++; break;
        case '<': initlexer(&new, peek, LES, 1); peek++; break;
        case '>': initlexer(&new, peek, GRT, 1); peek++; break;
        case '=': initlexer(&new, peek, EQL, 1); peek++; break;
        default:
            // if it's a letter
            if(*peek >= 'a' && *peek <= 'z') {
                while(peek[len] >= 'a' && peek[len] <= 'z') {
                    len++;
                }
         
                substr = malloc(len + 1);
                memcpy(substr, peek, len);
                substr[len] = '\0';
                initlexer(&new, peek, iskey(substr), len);
                free(substr);
            }
            // if it's a decimal number
            else if(*peek >= '0' && *peek <= '9') {
                while(peek[len] >= '0' && peek[len] <= '9') {
                    len++;
                }

                // floating points aren't supported
                if(peek[len] == '.') {
                    puts("No floating points...");
                    exit(4);
                }

                initlexer(&new, peek, NUM, len);
            }
            // invalid character encounter
            else {
                printf("invalid char [%c]\n", *peek);
                exit(1);
            }

            peek = peek + len + 1;
            break;
    }

    free(old->tok);
    free(old);
    new->pos = peek;
    return new;
}

int main(int argc, char **argv) {

    int length;
    char *input = readfile(argv[1], &length);

    if(input) {
        input[length] = '\0';
        lexer *lex = malloc(sizeof(lexer));
        lex->tok = NULL;
        lex->pos = input;

        while(1) {
            lex = next(lex);
            if(lex->pos[0] != '\0') {
                if(lex->tok)
                    printf("[%s] -- [%d]\n", lex->tok, lex->type);
            } else {
                break;
            }
        }

        free(lex->tok);
        free(lex);
        free(input);
    }

    return 0;
}
