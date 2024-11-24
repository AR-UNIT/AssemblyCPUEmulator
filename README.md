# CS 5600 Lab 1 - emulate the 5600 CPU

See [CPU Description](cpu-description.pdf) for a detailed description of the instruction set to emulate.

You should modify the following files:
- `emulate.c` - this is your CPU emulator
- `test.c` - this is the start of a test skeleton for your code

Additional files provided:
- `Makefile` - so the `make` command will work
- `asm.py` - assembler, translates assembly language to binary
- `hello.asm`, `hello.bin` - "Hello world!" in 6500 assembler
- `runsim.c` - runs your simulation
- `disasm.c` - disassembler

### Deliverable

We will grade the most recent version of your code in your repository as of the assignment due date\*. Remember to run `git push` to copy your commits to the server.

\* actually there's a process for requesting and using slip hours, but we'll discuss that in lecture and post to Piazza.

### Git commits / pushes

You are expected to run `git commit` frequently, i.e. at least every few hours. This is not only good practice, but also serves as a record that you developed the code yourself rather than copying it from a classmate.

The command `git commit -a` will commit all modified files, starting  up an editor for you to record a commit note. It's good practice to  provide a note which will remind you what you changed, in case you  need to look back through versions; if you don't write anything, it  won't let you commit.

### How to compile things

You can use the `make` command to compile the three binaries:
```
~/base-lab1$ make runsim disasm test
cc -g   -c -o emulate.o emulate.c
cc -g   -c -o runsim.o runsim.c
cc -g   -c -o disasm.o disasm.c
cc -g -o runsim emulate.o runsim.o disasm.o
cc -g -o disasm -DSTANDALONE disasm.c
cc -g   -c -o test.o test.c
cc -g -o test test.o emulate.o
```

You can delete the object files and executables with the `make clean` command. (e.g. before uploading to gradescope)

### `runsim`
The `runsim` command takes a binary file as an argument, and two optional parameters:
```
    runsim [-v] [-r] filename.bin
```

If given the `-v` option ("verbose") it will disassemble each instruction and print it to the terminal before executing it.

The `-r` ("registers") option causes it to print all registers after the instruction runs. 

### testing

You are expected to test your own code to make sure it works on all instructions, not just the ones in `hello.asm`. 

The file `test.c` shows how you might be able to do this. Note that you may find it helpful to use the assembler and disassembler to create these examples - e.g. here's how I created the first test:
```
~/base-lab1$ echo 'ADD R3 + R6 -> R1' > t.asm
~/base-lab1$ python3 asm.py t.asm
bytes: 2
5073
~/base-lab1$ ./disasm t.bin
0000 : 5073      : ADD R3 + R6 -> R1
```

