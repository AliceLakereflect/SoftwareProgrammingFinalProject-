# SoftwareProgrammingFinalProject-

This is a 2-pass assembler compose by C++ which reads source.txt and produce the object code.

### Environment
- IDE: VS code
- Compiler version: gcc9
- C language version: C++11

#### Steps
1. Install C++ language Extansion in VSCode - https://code.visualstudio.com/docs/languages/cpp
2. Reopen VS Code window
3. Open Terminal and run git clone https://github.com/AliceLakereflect/SoftwareProgrammingFinalProject-.git
4. Open 5110056001.cpp file and press Command + Shift + B to build it
5. Run with command ./5110056001 to execute the program

#### Output
**The following files will be generated by executing the cpp program:**
Pass1:
- IntermediateFile.txt - The intermediate file after Pass 1. This include the location counter value and the error flags.
- SYMTAB.txt - The symbol table after Pass 1
Pass2:
- SourceProgram.txt - The source program after Pass 2. This include all the information for producing an object program.
- FinalObjectProgram.txt - The object program for machine.



