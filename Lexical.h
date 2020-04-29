// Juan Blanco, ju589373
// LEXICAL ANALYZER

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Define global constants
#define MAXDIGIT 5
#define IDENTMAX 11
#define RESERVEDMAX 14
#define SYMBOLMAX 13

// Function prototypes
int wordHandler(char* code, int position, FILE *ofp);
int symHandler(char* code, int position, int cnt, FILE *ofp);
int numHandler(char* code, int position, FILE *ofp);
void lexemeNumHandler(char* code, int position, FILE *ofp);
void lexemeWordHandler(char* code, int position, FILE *ofp);
void lexemeSymHandler(char* code, int position, int cnt, FILE *ofp);
int reservedWord(char* str);
int reservedSymbol(char symbol);

// Define each token_type
typedef enum
{
  nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5, multsym = 6,  slashsym = 7,
  oddsym = 8,  eqlsym = 9, neqsym = 10, lessym = 11, leqsym = 12, gtrsym = 13, geqsym = 14,
  lparentsym = 15, rparentsym = 16, commasym = 17, semicolonsym = 18, periodsym = 19, becomessym = 20,
  beginsym = 21, endsym = 22, ifsym = 23, thensym = 24, whilesym = 25, dosym = 26, callsym = 27, constsym = 28,
  varsym = 29, procsym = 30, writesym = 31, readsym = 32, elsesym = 33
} token_type;

// String of strings for finding if a word is reserved
char *reserved_words[] = { "const", "var", "procedure", "call", "begin", "end", "if", "then", "else", "while", "do", "read", "write", "odd" };
// Get index of reserved_words to find the token_type for the reserved word
int word_token[] = {constsym, varsym, procsym, callsym, beginsym, endsym, ifsym, thensym, elsesym, whilesym, dosym, readsym, writesym, oddsym };

// String of reserved symbols
char symbols[] ={'+', '-', '*', '/', '(', ')', '=', ',', '.', '<', '>', ';', ':' };
// Using the index of symbols we find the token_type for the reserved symbols
int sym_token[] = {plussym, minussym, multsym, slashsym, lparentsym, rparentsym, eqlsym, commasym, periodsym, lessym, gtrsym, semicolonsym, becomessym};

// Global position variable
int position = 0;

int Lexical(int flag)
{
  if(flag == 0)
    return 0;

  FILE *ifp = fopen("input.txt", "r"); // Declare input files
  FILE *ofp = fopen("lexOut.txt", "w"); // Output files

  int cnt = 0;
  int current;
  char code[50000];

  // Error out if input is empty
  if (ifp == NULL)
  {
    printf("ERROR! file could not open.");
    fprintf(ofp, "ERROR! file could not open.");
    return 0;
  }

  // Insert .txt file into code string
  while((current = fgetc(ifp)) != EOF)
  {
    code[cnt] = current;
    cnt++;
  }

  int i;
  int hold;
  int error;

  // Reset position to re-traverse code string to print Lexemme List
  position = 0;

  // Resets loop to print Lexeme List using code string
  for (i = 0; i < cnt; i++)
  {
    // Check for letters, numbers and symbols
    hold = position;
    if (isalpha(code[position]))
      lexemeWordHandler(code, hold, ofp);
    else if (isdigit(code[position]))
      lexemeNumHandler(code, hold, ofp);
    else if (ispunct(code[position]))
      lexemeSymHandler(code, hold, cnt, ofp);

    position++;
  }

  printf("\n");

  // Close file pointers
  fclose(ifp);
  fclose(ofp);
  return 0;
}

// Once a letter is determined we search for reserved word or identifier
int wordHandler(char* code, int hold, FILE *ofp)
{
  position++;

  // Stop when char is not alphanumeric
  while(isalnum(code[position]))
    position++;

  // Get length of word and store in stringy
  int length_str = position - hold;

  // ERROR out if string is greater than max length
  if (length_str > IDENTMAX)
    return -1;

  char stringy[length_str-1];
  for (int j = 0; j < length_str; j++)
    stringy[j] = code[hold + j];

  stringy[length_str] = '\0';
  int tokenIndex;
  int token;
  tokenIndex = reservedWord(stringy);

  // Print out reserved word symbol or identifier symbol
  if (tokenIndex == -1)
  {
    token = identsym;
    printf("%s\t\t%d\n", stringy, token);
    fprintf(ofp, "%s\t\t%d\n", stringy, token);
  }
  else
  {
    token = word_token[tokenIndex];
    printf("%s\t\t%d\n", stringy, token);
    fprintf(ofp, "%s\t\t%d\n", stringy, token);
  }

  // Check if a symbol is follwed by word or letter
  if (ispunct(code[position]))
  {
    position--;
    return 1;
  }
  return 1;
}

// Return index for reserved word by traversing reserved_words
int reservedWord(char* str)
{
  int res;
  for (int i = 0; i < RESERVEDMAX; i++)
  {
    res = strcmp(str, reserved_words[i]);
    if (res == 0)
      return i;
  }
  return -1;
}

// Handles a char in code that starts with a number
int numHandler(char* code, int hold, FILE *ofp)
{
  position++;
  int digits = 1;

  // Checks in case a variable doesn't start with a letter.
  if (isalpha(code[position]))
  {
    printf("ERROR! Variable does not start with letter.\n");
    fprintf(ofp, "ERROR! Variable does not start with letter.\n");
    exit(1);
  }

  // Traverse code to keep finding digits
  // Error out if we reach max digits
  while(isdigit(code[position]))
  {
    position++;
    digits++;
    if (digits > MAXDIGIT)
      return -1;
  }

  // Print out number symbol and length of digits
  int length_num = position - hold;
  int token = numbersym;
  for (int j = 0; j < length_num; j++)
    {
      printf("%c", code[hold + j]);
      fprintf(ofp, "%c", code[hold + j]);
    }
  printf("\t\t%d\n", token);
  fprintf(ofp, "\t\t%d\n", token);

  // Check if symbol is followed by number
  if (ispunct(code[position]))
  {
    position--;
    return 1;
  }

  return 1;
}

// Handles a char in the code that is a symbol
int symHandler(char* code, int hold, int cnt, FILE *ofp)
{
  // Check for symbols with two chars
  if (code[position] == '>' && code[position+1] == '=')
  {
    int token = geqsym;
    printf(">=\t\t%d\n", token);
    fprintf(ofp, ">=\t\t%d\n", token);
    position++;
    return 1;
  }
  else if (code[position] == '<' && code[position+1] == '>')
  {
    int token = neqsym;
    printf("<>\t\t%d\n", token);
    fprintf(ofp, "<>\t\t%d\n", token);
    position++;
    return 1;
  }
  else if (code[position] == '<' && code[position+1] == '=')
  {
    int token = leqsym;
    printf("<=\t\t%d\n", token);
    fprintf(ofp, "<=\t\t%d\n", token);
    position++;
    return 1;
  }
  else if (code[position] == ':' && code[position+1] == '=')
  {
    int token = becomessym;
    printf(":=\t\t%d\n", token);
    fprintf(ofp, ":=\t\t%d\n", token);
    position++;
    return 1;
  }
  else if (code[position] == '/' && code[position+1] == '*')
  {  // checks for comments to ignore through iteration
    int j = position + 2;
    while (!(code[j] == '*' && code[j+1] == '/'))
      {
        j++;
        if (j >= cnt)
        {
          printf("ERROR! End of Comment '*/' not found.\n");
          fprintf(ofp, "ERROR! End of Comment '*/' not found.\n");
          exit(1);
        }
      }
      position = j + 2;
      return 1;
  }

  // Check for reserved symbols and print out token
  position++;
  int tokenIndex = reservedSymbol(code[hold]);

  // if tokenIndex is negative it is an invalid symbol
  if (tokenIndex == -1)
    return -1;

  int token = sym_token[tokenIndex];
  printf("%c\t\t%d\n", code[hold], token);
  fprintf(ofp, "%c\t\t%d\n", code[hold], token);

  return 1;
}

// Returns reserved symbol index in symbols
int reservedSymbol(char symbol)
{
  int res;
  for (int i = 0; i < SYMBOLMAX; i++)
  {
    if (symbol == symbols[i])
      return i;
  }
  return -1;
}

// Handles print statements for Lexemme List using same logic as symHandler()
void lexemeSymHandler(char* code, int hold, int cnt, FILE *ofp)
{
  if (code[position] == '>' && code[position+1] == '=')
  {
    int token = geqsym;
    printf("%d ", token);
    fprintf(ofp, "%d ", token);
    position++;
    return;
  }
  else if (code[position] == '<' && code[position+1] == '>')
  {
    int token = neqsym;
    printf("%d ", token);
    fprintf(ofp, "%d ", token);
    position++;
    return;
  }
  else if (code[position] == '<' && code[position+1] == '=')
  {
    int token = leqsym;
    printf("%d ", token);
    fprintf(ofp, "%d ", token);
    position++;
    return;
  }
  else if (code[position] == ':' && code[position+1] == '=')
  {
    int token = becomessym;
    printf("%d ", token);
    fprintf(ofp, "%d ", token);
    position++;
    return;
  }
  else if (code[position] == '/' && code[position+1] == '*')
  {  // checks for comments to ignore through iteration
    int j = position + 2;
    while (!(code[j] == '*' && code[j+1] == '/'))
      {
        j++;
        if (j >= cnt)
        {
          printf("ERROR! End of Comment '*/' not found.\n");
          fprintf(ofp, "ERROR! End of Comment '*/' not found.\n");
          exit(1);
        }
      }
      position = j + 2;
      return;
  }

  position++;
  int tokenIndex = reservedSymbol(code[hold]);
  int token = sym_token[tokenIndex];
  printf("%d ", token);
  fprintf(ofp, "%d ", token);

  return;
}

// Handles print statements for Lexemme List using same logic as numHandler()
void lexemeNumHandler(char* code, int hold, FILE *ofp)
{
  position++;
  int digits = 1;

  // stop when char is not alphanumeric
  while(isdigit(code[position]))
  {
    position++;
    digits++;
    if (digits > MAXDIGIT)
    {
      printf("ERROR! Invalid number length.\n");
      fprintf(ofp, "ERROR! Invalid number length.\n");
      break;
    }
  }

  int length_num = position - hold;
  int token = numbersym;
  printf("%d ", token);
  fprintf(ofp, "%d ", token);
  for (int j = 0; j < length_num; j++)
  {
    printf("%c", code[hold + j]);
    fprintf(ofp, "%c", code[hold + j]);
  }
  printf(" ");
  fprintf(ofp, " ");

  if (ispunct(code[position]))
  {
    position--;
    return;
  }

  return;
}

// Handles print statements for Lexemme List using same logic as wordHandler()
void lexemeWordHandler(char* code, int hold, FILE *ofp)
{
  position++;
  // stop when char is not alphanumeric
  while(isalnum(code[position]))
    position++;

  int length_str = position - hold;
  char stringy[length_str-1];
  for (int j = 0; j < length_str; j++)
    stringy[j] = code[hold + j];

  stringy[length_str] = '\0';
  int tokenIndex;
  int token;
  tokenIndex = reservedWord(stringy);
  if (tokenIndex == -1)
  {
    token = identsym;
    printf("%d %s ", token, stringy);
    fprintf(ofp, "%d %s ", token, stringy);
  }
  else
  {
    token = word_token[tokenIndex];
    printf("%d ", token);
    fprintf(ofp, "%d ", token);
  }

  if (ispunct(code[position]))
  {
    position--;
    return;
  }

  return;
}
