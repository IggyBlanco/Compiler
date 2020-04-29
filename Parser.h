// Juan Blanco, ju589373
// PARSER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define CODE_SIZE 500
#define MAX_SYMBOL_TABLE_SIZE 500

// Struct is from the pdf of the HW Parser assignment.
typedef struct
{
  int kind;
  char name[12];
  int val; // ascii value
  int level; // lexicographical lvl
  int addr; // M
} symbol;

// Token Structure
typedef struct
{
  char value[12]; // token value
  char type[3]; // token type
} token;

// This struct is also in p-Machine
// but for reasons i couldn't help to understand
// i had to utilize again, with a differnet name.
typedef struct
{
  int op; //opcode.
  int r;  //register.
  int l;  //lexicographical level.
  int m;  //modifier.
} codeStruct;

FILE *in;
FILE *out;
token curToken; //Current token to analyze.
int level; // lex level max
int curLvl; // current lex lvl
int sp; // stack pointer
int rp; // register pointer

symbol symbol_table[MAX_SYMBOL_TABLE_SIZE]; //Symbol table data structure.
codeStruct cs[CODE_SIZE]; // code struct
int symCount; // symbol counter
int codeIndex; // index of code

// Functions used in parser
void Expression();
int RelationalOperation();
void Block();
void Factor();
void Statement();
void Term();
void Condition();
void Program();
void Insert(int k, char name[], int val, int addr);
void Emit(int op, int r, int l, int m);
void ErrorType(int n);
int GetToken();

// Parser is run throught the driver with the -a command.
// Flag is determined in the compiler driver.
int Parser(int flag)
{
  // if flag is not activated, terminate early.
  if (flag == 0)
    return 0;

  int i;
  in = fopen("lexOut.txt", "r");

  curLvl = 1; // current level
  level = curLvl; // max level
  symCount = 0; // symbol counter
  codeIndex = 0; // code index
  sp = 1; // stack pointer
  rp = 0; // register pointer

  for( i = 0; i < CODE_SIZE; i++ )
  {
    cs[i].op = 0;
    cs[i].r = 0;
    cs[i].l = 0;
    cs[i].m = 0;
  }

  Program(); // Will run to check for errors
  printf("No errors, program is syntactically correct.\n");
  out = fopen("low_lvl.txt", "w");

  // Gets assembly code
  int j = 0;
  while(!(cs[j].op == 0 && cs[j].r == 0 && cs[j].l == 0 && cs[j].m == 0))
  {
    fprintf(out, "%d %d %d %d\n", cs[j].op, cs[j].r, cs[j].l, cs[j].m);
    j++;
  }

  fclose(in); // close files
  fclose(out);
  return 0;
}

// Contains erros from assigment pdf
void ErrorType(int caseNum)
{
  // Switch statement contains the 25 errors from the assignment pdf.
  // Error 16 is now functional for hw 4.
  switch(caseNum)
  {
  case 1: printf("Error 1: Use = instead of :=. \n");
    break;
  case 2: printf("Error 2: = must be followed by a number. \n");
    break;
  case 3: printf("Error 3: Identifier must be followed by =. \n");
    break;
  case 4: printf("Error 4: Const, int, procedure must be followed by identifier. \n");
    break;
  case 5: printf("Error 5: Semicolon or comma missing. \n");
    break;
  case 6: printf("Error 6: Incorrect symbol after procedure declaration. \n");
    break;
  case 7: printf("Error 7: Statement expected. \n");
    break;
  case 8: printf("Error 8: Incorrect symbol after statement part in block. \n");
    break;
  case 9: printf("Error 9: Period expected. \n");
    break;
  case 10: printf("Error 10: Semicolon between statements missing. \n");
    break;
  case 11: printf("Error 11: Undeclared identifier. \n");
    break;
  case 12: printf("Error 12: Assignment to constant or procedure is not allowed. \n");
    break;
  case 13: printf("Error 13: Assignment operator expected. \n");
    break;
  case 14: printf("Error 14: Call must be followed by an identifier. \n");
    break;
  case 15: printf("Error 15: Call of a constant or variable is meaningless. \n");
    break;
  case 16: printf("Error 16: Then expected. \n");
    break;
  case 17: printf("Error 17: Semicolon or } expected. \n");
    break;
  case 18: printf("Error 18: Do expected. \n");
    break;
  case 19: printf("Error 19: Incorrect symbol following statement. \n");
    break;
  case 20: printf("Error 20: Relational opertor expected. \n");
    break;
  case 21: printf("Error 21: Expression must not contain a procedure identifier. \n");
    break;
  case 22: printf("Error 22: Right parenthesis missing. \n");
    break;
  case 23: printf("Error 23: The preceding factor cannot begin with this symbol. \n");
    break;
  case 24: printf("Error 24: An Expression cannot begin with this symbol. \n");
    break;
  case 25: printf("Error 25: This number is too large. \n");
    break;
  }
  fclose(in);
  exit(0);
}

void Program()
{
  GetToken();
  Block();
  if(strcmp(curToken.type, "19") != 0)
    ErrorType(9); // Period is missing
}

void Factor()
{
  int IDX; // index for identifier
  int declared = 0; // boolean value
  int i;

  if(!strcmp(curToken.type, "2"))
  {
    //Check for undeclared.
    for(i = symCount - 1; i >= 0; i--)
      if(!strcmp(curToken.value, symbol_table[i].name ))
      {
        declared = 1; //Declared!
        IDX = i; //Save index.
      }

    //Undeclared identifier
    if(declared == 0)
      ErrorType(11);

    // if var
    if(symbol_table[IDX].kind == 2)
        Emit( 3, rp, curLvl - symbol_table[IDX].level, symbol_table[IDX].addr - 1);

    //If const
    else if(symbol_table[IDX].kind == 1)
        Emit(1, rp, 0, symbol_table[IDX].val);
    rp++; //Increment RP.
    GetToken();
  }

  // if num
  else if(!strcmp(curToken.type, "3"))
  {
    Emit(1, rp, 0, atoi(curToken.value));
    rp++; // register pointer is incremented
    GetToken();
  }

  // if parenthesis
  else if(!strcmp(curToken.type, "15"))
  {
    GetToken();
    Expression();
    if(strcmp(curToken.type, "16") != 0)
      ErrorType(22);
    GetToken();
  }
  else
    ErrorType(23);
}

void Block()
{
  // table index, value of symbol, procedure area, location to jump to
  int index, val, room = 4, jmpTo = codeIndex;
  sp = 5; //Stack pointer.
  char name[12]; //identifier
  Emit(7, 0, 0, 0); // 7 is for jmp

  if(!strcmp(curToken.type, "28"))
  {
    int x = 0; // used for comparisons
    do
    {
      GetToken();

      if(strcmp(curToken.type, "2") != x)
        ErrorType(4);

      strcpy(name, curToken.value);
      GetToken();

      if(strcmp(curToken.type, "9") != x)
        ErrorType(3);
      GetToken();

      if(strcmp(curToken.type, "3") != x)
        ErrorType(2);
      val = atoi(curToken.value);

      // insert symbol
	    Insert(1, name, val, x);
      GetToken();

    } while(!strcmp(curToken.type, "17"));

    if(strcmp(curToken.type, "18") != x)
      ErrorType(5); // missing punctuation

    GetToken();
  }

  // for declaration of var
  if(!strcmp(curToken.type, "29"))
  {
    do
    {
      GetToken();
      if(strcmp(curToken.type, "2") != 0)
        ErrorType(4);
      strcpy(name, curToken.value);

      // insert symbol
      Insert(2, name, 0, sp);
      symbol_table[symCount - 1].level = level;
      sp++;
	    room++;
      GetToken();

    } while(!strcmp(curToken.type, "17"));

    if(strcmp(curToken.type, "18") != 0)
      ErrorType(5);
    GetToken();
  }

  // declaration of procedure
  while(!strcmp(curToken.type, "30"))
  {
    GetToken();

    // is procedure followed by an identifier?
    if(strcmp(curToken.type, "2") != 0)
      ErrorType(4);
    strcpy(name, curToken.value); // store value as name

    // insert a symbol
    Insert(3, name, 0, 0);
    index = symCount - 1; // update index
    symbol_table[index].level = level;
    symbol_table[index].addr = jmpTo + 1;
    GetToken();

    if(strcmp(curToken.type, "18") != 0)
      ErrorType(17); // check if missing semicolon

    GetToken();
    level++;
    Block();

    if(strcmp(curToken.type, "18") != 0)
      ErrorType(17); // check if missing semicolon
    GetToken();
  }

  cs[jmpTo].m = codeIndex; // stores
  Emit(6, 0, 0, room);
  Statement();
  Emit(2, 0, 0, 0);
  curLvl--; // decrement current level
}

// Function to insert symbol into symbol table
void Insert(int kind, char name[], int val, int addr)
{
  symbol_table[symCount].kind = kind;
  strcpy(symbol_table[symCount].name, name);
  symbol_table[symCount].val = val;
  symbol_table[symCount].addr = addr;
  symCount++; // symbol count is incremented
}

void Statement()
{
  int i;
  int declared = 0; // true, false
  int IDX; // store

  // :=
  if(!strcmp(curToken.type, "2"))
  {
    // check for declaration
    for(i = symCount - 1; i >= 0; i--)
        if(!strcmp(curToken.value, symbol_table[i].name))
        {
            if(symbol_table[i].kind == 1)
              ErrorType(12);
            else if(symbol_table[i].kind == 2)
            {
                declared = 1; // true
                IDX = i; // store
            }
        }

    if(declared == 0) // check if declared
      ErrorType(11);
    GetToken();
    if(strcmp(curToken.type, "20") != 0)
      ErrorType(3);
    GetToken();
    Expression();
    Emit(4, rp - 1, curLvl - symbol_table[IDX].level, symbol_table[IDX].addr - 1);
    rp--;
  }

    // call
    else if(!strcmp(curToken.type, "27"))
    {
        int declared = 0; // true, false
        GetToken();
        if(strcmp(curToken.type, "2"))
          ErrorType(14); // check if identifier is missing

        // check for declaration
        for(i = symCount - 1; i >= 0; i--)
            if(!strcmp(curToken.value, symbol_table[i].name))
            {
                IDX = i; // store
                declared = 1;
            }

        // check to see if identifier is undeclared
        if(declared == 0)
          ErrorType(11);

        if(symbol_table[IDX].kind == 3)
        {
            Emit(5, 0, level, symbol_table[IDX].addr);
            curLvl ++;
        }
        else
            ErrorType(14);
        GetToken();
    }

  else if(!strcmp(curToken.type, "21"))
  {
    GetToken();
    Statement();

    while(!strcmp(curToken.type, "18"))
    {
      GetToken();
      Statement();
    }

    if(strcmp(curToken.type, "22") != 0)
      ErrorType(26);
    GetToken();
  }

  // if, then, else
  // These lines are new and have been implemented for hw 4.
  // I modified some of these lines from hw 3 as i wasn't sure
  // my if, then, else implementation worked properly when i turned
  // it in. I had if, else, then but never bothered to actually test
  // them before submitting my work. i Merely implemented them on a whim.
  else if(!strcmp(curToken.type, "23"))
  {
    GetToken();
    Condition();


	// if statements consists of a condition, a substatement
	// that executes if the condition is true or false.
    if(strcmp(curToken.type, "24") != 0)
      ErrorType(16);
    GetToken();

    int ctemp = codeIndex; // store

    Emit(8, rp - 1, 0, 0);
    Statement();
    GetToken();

    // 'else'
	// else carries a second sub-statement that can be provided.
	// This second statment will be executed instead of the first
	// one if the condition was false.
	// One of two statements will be executed.
    if( !strcmp(curToken.type, "33"))
    {
        int ctemp2 = codeIndex; // store
        Emit(7, 0, 0, 0);
        cs[ctemp].m = codeIndex;
        GetToken();
        Statement();
        cs[ctemp2].m = codeIndex;
        rp--; // decrement register pointer
    }
    else
    {
        cs[ctemp].m = codeIndex;
        rp--; // decrement register pointer
    }
  }

  // while and do loops
  else if(!strcmp(curToken.type, "25"))
  {
    int codeIndexA = codeIndex;
    GetToken();
    Condition();
    int codeIndexB = codeIndex; // jpc
    Emit(8, rp - 1, 0, 0);
    if(strcmp(curToken.type, "26") != 0)
      ErrorType(18);

    GetToken();
    Statement();

    Emit(7, 0, 0, codeIndexA);
    cs[codeIndexB].m = codeIndex;
    rp--; // decrement register pointer
  }

  // read ident
  else if(!strcmp(curToken.type, "32"))
  {
    GetToken();
    if(strcmp(curToken.type, "2") != 0)
      ErrorType(29);
    // checks if declared
    for(i = symCount - 1; i >= 0; i--)
        if(!strcmp(curToken.value, symbol_table[i].name))
        {
            declared = 1; // true
            IDX = i; // store
        }
     if(declared == 0) // checks if declared
      ErrorType(11);
     Emit(10, rp, 0, 2); // uses input to reg
     if(symbol_table[IDX].kind == 2)
        Emit(4, rp, curLvl - symbol_table[IDX].level, symbol_table[IDX].addr - 1);
     else if(symbol_table[IDX].kind == 1)
        ErrorType( 12 ); // unallowed assignment
     GetToken();
  }

  // write ident
  else if(!strcmp(curToken.type, "31"))
  {
    GetToken();
    if(strcmp(curToken.type, "2") != 0 )
      ErrorType(29);

    // check for declaration
    for(i = symCount - 1; i >= 0; i--)
        if(!strcmp(curToken.value, symbol_table[i].name))
        {
            if(symbol_table[i].kind == 1 || symbol_table[i].kind == 2)
            {
                declared = 1; // true
                IDX = i; // store
            }
        }
     if(declared == 0) // checks if declared
      ErrorType(11);
     // pull var
     if(symbol_table[IDX].kind == 2)
     {
        Emit(3, rp, curLvl - symbol_table[IDX].level, symbol_table[IDX].addr - 1); // from mem
        Emit(9, rp, 0, 1); // to output
     }

     // we pull const
     else if(symbol_table[IDX].kind == 1)
     {
        Emit(1, rp, 0, symbol_table[IDX].val); // r to table
        Emit(9, rp, 0, 1); // r to out
     }
     GetToken();
  }
}

void Expression()
{
  char plusOperation[3];
  if(!strcmp(curToken.type, "4") || !strcmp(curToken.type, "5"))
  {
    strcpy(plusOperation, curToken.type);
    GetToken();
    Term();
    // if previous is minussym
    if(!strcmp(plusOperation, "5"))
        Emit(11, rp - 1, rp - 1, 0);
  }
  else Term();
  while(!strcmp(curToken.type, "4") || !strcmp(curToken.type, "5"))
  {
    strcpy(plusOperation, curToken.type);
    GetToken();
    Term();
    // if plus
    if(!strcmp(plusOperation, "4"))
    {
        Emit(12, rp - 2, rp - 2, rp - 1);
        rp--;
    }
    // if min
    else
    {
        Emit(13, rp - 2, rp - 2, rp - 1);
        rp--;
    }
  }
}

void Term()
{
  char mulop[3];
  Factor();

  while(!strcmp(curToken.type, "6") || !strcmp(curToken.type, "7"))
  {
    strcpy( mulop, curToken.type );
    GetToken();
    Factor();

    // if mult
    if(!strcmp(mulop, "6"))
    {
        Emit(14, rp - 2, rp - 2, rp - 1);
        rp--;
    }
    // if div
    else
    {
        Emit(15, rp - 2, rp - 2, rp - 1);
        rp--;
    }
  }
}

int RelationalOperation()
{
  // if rel OP
  if(!strcmp(curToken.type, "9")) // eqlsym
    return 18;
  else if(!strcmp(curToken.type, "10")) // neqsym
    return 19;
  else if(!strcmp(curToken.type, "11")) // lessym
    return 20;
  else if(!strcmp(curToken.type, "12")) // leqsym
    return 21;
  else if(!strcmp(curToken.type, "13")) // gtrsym
    return 22;
  else if(!strcmp(curToken.type, "14")) // geqsym
    return 23;
  else
    return 0; // false
}

void Emit(int op, int r, int l, int m)
{
  if(codeIndex > CODE_SIZE)
    ErrorType(30); // codes too large
  else
  {
    cs[codeIndex].op = op; // op
    cs[codeIndex].r = r; // register
    cs[codeIndex].l = l; // lex
    cs[codeIndex].m = m; // m
    codeIndex++; // increment code index
  }
}

void Condition()
{
  // if 'odd'
  if(!strcmp(curToken.type, "8"))
  {
    GetToken();
    Expression();
    Emit(16, rp - 1, rp - 1, 0);
  }
  // else its rel op
  else
  {
    Expression();
    int op = RelationalOperation();
    if(!op)
      ErrorType(20);
    GetToken();
    Expression();
    Emit(op, rp - 2, rp - 2, rp - 1 );
    rp--; // regiester pointer is incremented
  }
}

// Gets next token from list
int GetToken()
{
  if(fscanf(in, "%s", curToken.type ) != EOF)
  {
    // num or ident
    if(!strcmp(curToken.type, "2"))
      fscanf( in, "%s", curToken.value );
    else if(!strcmp(curToken.type, "3"))
      fscanf( in, "%s", curToken.value );
    else
      curToken.value[0] = '\0';
    return 1;
  }
  // no more available tokens
  else
  {
    curToken.type[0] = '\0';
    curToken.value[0] = '\0';
    return 0;
  }
}
