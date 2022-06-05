#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <iterator>
#include <bits/stdc++.h>
using namespace std;


struct Opcode {
     string key; 
     string value; 
};


struct Statement {
     string label; 
     string opcode; 
     string operand; 
};

Opcode opcodeRef[60];

void GetOpcodeRef(){
    Opcode opcode;
    ifstream ifs("opcode.txt", ios::in);
    if (!ifs.is_open()) {
        cout << "Failed to open file.\n";
    } else {
        string key;
        string value;
        int score;
        int i = 0;
        while (ifs >> key >> value) {
            opcode.key = key;
            opcode.value = value; 
            opcodeRef[i] = opcode;
            i++;
        }
        ifs.close();
    }
    ifs.close();
}

Statement ScanStatement(string command)
{
    Statement st;
    vector<string> v;
    stringstream ss(command);

    while (ss.good()) {
        string substr;
        getline(ss, substr, '\t');
        v.push_back(substr);
    }
    st.label = v[0];
    st.opcode = v[1];
    if(v.size() > 2) st.operand = v[2].erase(v[2].size() - 1);

    cout << st.label << "\t" << st.opcode << "\t" << st.operand << "\n";
    return st;
}

void WriteOutput(char filename[], int objectCode[]){
    
    FILE* file;
    file = fopen (filename, "w");
    if (file == NULL)
    {
        fprintf(stderr, "\nCouldn't Open File'\n");
        exit (1);
    }
    
    // writing all the nodes of the linked list to the file
    // for (int i = 0; i < objectCode.length; i++)
    // {
    //     fwrite(temp, sizeof(char), 1, file);
    // }
    
    if(&fwrite != 0)
    {
        printf("Linked List stored in the file successfully\n");
    }
       else
    {
           printf("Error While Writing\n");
    }
    
    fclose(file);
    
}

int main()
{
    GetOpcodeRef();
    ifstream ifs("source.txt", ios::in);
    if (!ifs.is_open()) {
        cout << "Failed to open file.\n";
    } else {
        string command;
        while (getline(ifs, command)) {
            Statement st = ScanStatement(command);
        }
        ifs.close();
    }
    ifs.close();
    return 0;
}
