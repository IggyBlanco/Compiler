// Juan Blanco, ju589373
// Compiler Driver

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "PMachine.h"
#include "Lexical.h"
#include "Parser.h"

int main(int argc, char *argv[])
{
  int l = 0; // print lexeme list (lex output)
  int a = 0; // print assembly code (parser output)
  int v = 0; // print execution trace (vm output)

  int i;
  if(argc != 0)
  {
    for(i = 1; i < argc; i++)
    {
      if(strcmp(argv[i], "-l") == 0)
        l = 1; // lex output
      else if(strcmp(argv[i], "-a") == 0)
        a = 1; // parser output
      else if(strcmp(argv[i], "-v") == 0)
        v = 1; // vm output
    }
  }

  Lexical(l);
  Parser(a);
  PMachine(v);
  return 0;
}
