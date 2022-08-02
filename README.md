# Blua -- Basic Lua

Kind of a cross between my [tinybasic project](https://github.com/gavinbm/tinybasic) (shout out to Dr. Henley) and a subset of Lua. I think lua is a syntactically pretty language (I like the "end" keyword) and I like the simplicity of basic.

Hopefully I get this on a website of it's own so people can toy with it. Final goal is to target wasm as I've recently been enjoying reading about [this project](https://blog.scottlogic.com/2019/05/17/webassembly-compiler.html) and I think wasm is the future of web development.

# Grammar
Here is the full grammar, features and full specs coming soon
```
prog        ::= {stmnt}
stmnt       ::= comment
                | "VAR" ident {"=" expr}
                | "FUNC" ident "(" argv ")" {stmnt} "END" 
                | ident "=" expr
                | ident "(" argv ")"
                | "IF" comp "THEN" {stmnt} "END"
                | "IF" comp "THEN" {stmnt} "ELSE" {stmnt} "END"
                | "WHILE" comp "DO" {stmnt} "END"
                | "GOTO" expr
comment     ::= "#" a string
comp        ::= expr (("=" | "!" | ">" | "<") expr)+
expr        ::= term {( "-" | "+" ) term}
term        ::= unary {( "/" | "*" ) unary}
unary       ::= ["+" | "-"] primary
primary     ::= number | ident
argv        ::= ident {"," ident}
```