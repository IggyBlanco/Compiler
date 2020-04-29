How to run Driver(UNIX-based GUI):

1) Enter Compiler Directory

2) If you wish to change input file, edit contents within "input.txt"
	or overwrite the file with a new "input.txt".

3) Compile Driver and assign executable <gcc Driver.c -o Driver>

4) Print Lexeme List (Lex output) <./Driver -l>
	- This takes in the file "input.txt" and spits out "lexOut.txt"

5) Run error handler and parser <./Driver -a>
	- This takes in the file "lexOut.txt" and spits out "low_lvl.txt"

6) Print Virtual Machine (P-Machine output) <./Driver -v>
	- This takes in the file "low_lvl.txt" and spits out "output.txt"


