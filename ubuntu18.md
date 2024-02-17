# Linux Kernel Programming (Ubuntu 18 & Linux Kernel 5.4)

## Ubuntu 18.04

### Build

#### If meet lex bug

```sh
# scripts/dtc/dtc-lexer.l:36
+extern YYLTYPE yylloc;
```