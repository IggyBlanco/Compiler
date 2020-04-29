// Juan Blanco, ju589373
// P-Machine

#include <stdio.h>
#include <stdlib.h>

#define MAX_STACK_HEIGHT 40
#define MAX_CODE_LENGTH 200
#define MAX_LEXI_LEVELS 3

struct instruction
{
  int OP; // OPERATION CODE
  int R; // REGISTER
  int L; // LEXICOGRAPHICAL LEVEL
  int M; // OPERATION DATA
};

int pc = 0; // PROGRAM COUNTER
int sp = 0; // STACK POINTER
int bp = 1; // BASE POINTER

int stack[MAX_STACK_HEIGHT] = {0}; // Initialize stack
int r[8] = {0}; // Declare CPU registers
int halt = 0; // Declare halt flag, init to 0(do not halt)

FILE *input; // Declare input files
FILE *output; // Output files

struct instruction IR; // Instruction Register
struct instruction code[MAX_CODE_LENGTH]; // Code

void fetch(); // Part 1 of cycle
void execute(); // Part 2 of cycle
void printStack(); // Prints contents of stack
void fprintStack(); // Prints contents of stack to output file
void printRegisters(); // Prints contents of register
void fprintRegisters(); // Prints contents of register to output file
void printAssembly(int count); // Prints the Interpreted Assembly Language
int base(int L, int base);

int PMachine(int flag)
{
  if(flag == 0)
    return 0;
  input = fopen("low_lvl.txt", "r"); // open input
  output = fopen("output.txt", "w"); // open output

  // In case the file can not be found
  if(input == NULL)
  {
    printf("Program failure: file could not open.\n");
    return 0;
  }

  int count = 0; // keep track of number of instructions the input file

  // Read in the lines from the input file
  while(!feof(input))
  {
    fscanf(input, "%d", &code[count].OP);
    fscanf(input, "%d", &code[count].R);
    fscanf(input, "%d", &code[count].L);
    fscanf(input, "%d", &code[count].M);
    count++;
  }


  printAssembly(count); // prints out interpreted assembly language
  count = 0; // count is reset to 0

  if(flag)
  {
    printf("\n\nExecution of Program:\n");
    printf("\t\t\tpc\tbp\tsp\tRegisters");
    printf("\nInitial Values\t\t0\t1\t0\t");
    printRegisters();
    printf("Stack: ");
    int j;
    for(j = 1; j < MAX_STACK_HEIGHT; j++)
    {
      printf("%d ", stack[j]);
    }
    printf("\n\t\t\tpc\tbp\tsp\tRegisters\n");
  }

  fprintf(output, "\n\nExecution of Program:\n");
  fprintf(output, "\t\t\t\t\tpc\tbp\tsp\tRegisters");
  fprintf(output, "\nInitial Values\t\t\t\t0\t1\t0\t");
  fprintRegisters();
  fprintf(output, "Stack: ");
  int j;
  for(j = 1; j < MAX_STACK_HEIGHT; j++)
  {
    fprintf(output, "%d ", stack[j]);
  }
  fprintf(output, "\n\t\t\t\t\tpc\tbp\tsp\tRegisters\n");

  // Loops fetch and execute cycle til halt is set to 0
  while(halt == 0)
  {
    fetch(); // Start the fetch part of the fetch exec cycle.
    execute(); // Start the execution process

    if(flag)
    {
      printf("%d\t%d\t%d\t", IR.R, IR.L, IR.M);
      printf("%d\t%d\t%d\t", pc, bp, sp);
      printRegisters();
      printf("Stack: ");
      printStack();
      printf("\n\n");
    }

    fprintf(output, "%d\t%d\t%d\t", IR.R, IR.L, IR.M);
    fprintf(output, "%d\t%d\t%d\t", pc, bp, sp);
    fprintRegisters();
    fprintf(output, "Stack: ");
    fprintStack();
    fprintf(output, "\n\n");

    if(( (pc == 0) && (bp == 0) & (sp == 0) ))
      halt = 1; // Sets program to stop
  }
  fclose(input); // close files
  fclose(output);
  return 0;
}

// Prints out the interpred assembly language
void printAssembly(int count)
{
  fprintf(output, "Interpreted Assembly Language\n");
  fprintf(output, "\nLine\tOP\tR\tL\tM");

  // Iterates through count number of switch cases
  int i;
  for (i = 0; i < count; i++)
  {
    int codeNumber = code[i].OP;
    switch(codeNumber)
    {
      case 1: fprintf(output, "\n%d\t", i);
      fprintf(output, "lit\t");
      break;
      case 2: fprintf(output, "\n%d\t", i);
      fprintf(output, "rtn\t");
      break;
      case 3: fprintf(output, "\n%d\t", i);
      fprintf(output, "lod\t");
      break;
      case 4: fprintf(output, "\n%d\t", i);
      fprintf(output, "sto\t");
      break;
      case 5: fprintf(output, "\n%d\t", i);
      fprintf(output, "cal\t");
      break;
      case 6: fprintf(output, "\n%d\t", i);
      fprintf(output, "inc\t");
      break;
      case 7: fprintf(output, "\n%d\t", i);
      fprintf(output, "jmp\t");
      break;
      case 8: fprintf(output, "\n%d\t", i);
      fprintf(output, "jpc\t");
      break;
      case 9: fprintf(output, "\n%d\t", i);
      fprintf(output, "sio\t");
      break;
      case 10: fprintf(output, "\n%d\t", i);
      fprintf(output, "sio\t");
      break;
      case 11: fprintf(output, "\n%d\t", i);
      fprintf(output, "sio\t");
      break;
      case 12: fprintf(output, "\n%d\t", i);
      fprintf(output, "neg\t");
      break;
      case 13: fprintf(output, "\n%d\t", i);
      fprintf(output, "add\t");
      break;
      case 14: fprintf(output, "\n%d\t", i);
      fprintf(output, "sub\t");
      break;
      case 15: fprintf(output, "\n%d\t", i);
      fprintf(output, "mul\t");
      break;
      case 16: fprintf(output, "\n%d\t", i);
      fprintf(output, "div\t");
      break;
      case 17: fprintf(output, "\n%d\t", i);
      fprintf(output, "odd\t");
      break;
      case 18: fprintf(output, "\n%d\t", i);
      fprintf(output, "mod\t");
      break;
      case 19: fprintf(output, "\n%d\t", i);
      fprintf(output, "eql\t");
      break;
      case 20: fprintf(output, "\n%d\t", i);
      fprintf(output, "neq\t");
      break;
      case 21: fprintf(output, "\n%d\t", i);
      fprintf(output, "iss\t");
      break;
      case 22: fprintf(output, "\n%d\t", i);
      fprintf(output, "leq\t");
      break;
      case 23: fprintf(output, "\n%d\t", i);
      fprintf(output, "gtr\t");
      break;
      case 24: fprintf(output, "\n%d\t", i);
      fprintf(output, "geq\t");
      break;
      default: return;
    }

    fprintf(output, "%d\t", code[i].R);
    fprintf(output, "%d\t", code[i].L);
    fprintf(output, "%d\t", code[i].M);
  }
  return;
}

void fetch()
{
  IR = code[pc]; // Fetch current instruction
  fprintf(output, "%d\t", pc); // print program counter before incrementation
  pc++; // Program counter points to next instruction
  return;
}

void execute()
{
  switch(IR.OP)
  {
    // LIT
    case 1:
    fprintf(output, "lit\t");
    r[IR.R] = IR.M;
    break;

    // RTN
    case 2:
    fprintf(output, "opr\t");
    sp = bp -1;
    bp = stack[sp + 3];
    pc = stack[sp + 4];
    break;

    // lOD
    case 3:
    fprintf(output, "lod\t");
    r[IR.R] = stack[base(IR.L, bp) + IR.M];
    break;

    // STO
    case 4:
    fprintf(output, "sto\t");
    stack[base(IR.L, bp) + IR.M] = r[IR.R];
    break;

    // CAL
    case 5:
    fprintf(output, "cal\t");
    stack[sp + 1] = 0;
    stack[sp + 2] = base(IR.L, bp);
    stack[sp + 3] = bp;
    stack[sp + 4] = pc;
    bp = sp + 1;
    pc = IR.M;
    break;

    // INC
    case 6:
    fprintf(output, "inc\t");
    sp = sp + IR.M;
    break;

    // JMP
    case 7:
    fprintf(output, "jmp\t");
    pc = IR.M;
    break;

    // JPC
    case 8:
    fprintf(output, "jpc\t");
    if(r[IR.R] == 0)
    {
      pc = IR.M;
    }
    break;

    // SIO - writes register to screen
    case 9:
    fprintf(output, "sio\t");
    printf("\nR[%d] = %d\n", IR.R, r[IR.R]);
    break;

    // SIO - read in value from screen to register
    case 10:
    fprintf(output, "sio\t");
    printf("\nEnter number into register: \n");
    scanf("%d", &r[IR.R]);
    break;

    // SIO - set halt flag to 1
    case 11:
    fprintf(output, "sio\t");
    halt = 1;
    break;

    // NEG
    case 12:
    fprintf(output, "neg\t");
    r[IR.R] = 0 - r[IR.L];
    break;

    // ADD
    case 13:
    fprintf(output, "add\t");
    r[IR.R] = r[IR.L] + r[IR.M];
    break;

    // SUB
    case 14:
    fprintf(output, "sub\t");
    r[IR.R] = r[IR.L] - r[IR.M];
    break;

    // MUL
    case 15:
    fprintf(output, "mul\t");
    r[IR.R] = r[IR.L] * r[IR.M];
    break;

    // DIV
    case 16:
    fprintf(output, "div\t");
    r[IR.R] = ((r[IR.L])/(r[IR.M]));
    break;

    // ODD
    case 17:
    fprintf(output, "odd\t");
    r[IR.R] = r[IR.R] % 2;
    break;

    // MOD
    case 18:
    fprintf(output, "mod\t");
    r[IR.R] = (r[IR.L])%(r[IR.M]);
    break;

    // EQL
    case 19:
    fprintf(output, "eql\t");
    if(r[IR.L] == r[IR.M])
    {
      r[IR.R] = 1;
    }
    else
    {
      r[IR.R] = 0;
    }
    break;

    // NEQ
    case 20:
    fprintf(output, "neq\t");
    if(r[IR.L] != r[IR.M])
    {
      r[IR.R] = 1;
    }
    else
    {
      r[IR.R] = 0;
    }
    break;

    // LSS
    case 21:
    fprintf(output, "lss\t");
    if(r[IR.L] < r[IR.M])
    {
      r[IR.R] = 1;
    }
    else
    {
      r[IR.R] = 0;
    }
    break;

    // LEQ
    case 22:
    fprintf(output, "leq\t");
    if(r[IR.L] <= r[IR.M])
    {
      r[IR.R] = 1;
    }
    else
    {
      r[IR.R] = 0;
    }
    break;

    // GTR
    case 23:
    fprintf(output, "gtr\t");
    if(r[IR.L] > r[IR.M])
    {
      r[IR.R] = 1;
    }
    else
    {
      r[IR.R] = 0;
    }
    break;

    // GEQ
    case 24:
    fprintf(output, "geq\t");
    if(r[IR.L] >= r[IR.M])
    {
      r[IR.R] = 1;
    }
    else
    {
      r[IR.R] = 0;
    }
    break;

    default:
    fprintf(output, "err\t");
    return;
  }
  return;
}

// Function from Appendix D
int base(int L, int base)
{
  int b1; // find base L levels down
  b1 = base;

  while(L > 0)
  {
    b1 = stack[b1 + 1];
    L--;
  }
  return b1;
}

// Prints registers
void printRegisters()
{
  int j;
  for(j = 0; j < 8; j++)
  {
    printf("%d ", r[j]);
  }
  printf("\n");
}

// Prints registers to output
void fprintRegisters()
{
  int j;
  for(j = 0; j < 8; j++)
  {
    fprintf(output, "%d ", r[j]);
  }
  fprintf(output, "\n");
}

// Prints stack
void printStack()
{
  int j;
  for(j = 1; j <= sp; j++)
  {
    printf("%d ", stack[j]);
  }
  return;
}

// Prints stack to output file
void fprintStack()
{
  int j;
  int cnt = 0;
  for(j = 1; j <= sp; j++)
  {
    fprintf(output, "%d ", stack[j]);
    cnt++;
    if (cnt == 6)
    {
      fprintf(output, "|");
    }
  }
  return;
}
