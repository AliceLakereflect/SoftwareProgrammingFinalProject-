#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <iterator>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
using namespace std;

struct Opcode {
    string key; 
    string value; 
};

struct Symbol {
    string label; 
    int address; 
};

struct Statement {
    int loc;
    string label; 
    string opcodeStr;
    string opcode;
    string operand; 
    string objCode;
};

Opcode opcodeRef[60];
vector<Symbol> symbolTable;
int currentLoc = 0;
int programLength = 0;
int objCodeLenCnt = 0;
vector<string> records;
vector<string> textRecords;
int textStartFrom = 0;
int programStartFrom = 0;

string replaceReturnChar(string& s) {
    size_t pos = s.find('\r');
    if (pos == string::npos) return s;
    s.replace(pos, 1, "");
    return s;
}

vector<string> splitString(string input, char delimiter){
    vector<string> v;
    stringstream ss(input);
    while (ss.good()) {
        string substr;
        getline(ss, substr, delimiter);
        v.push_back(substr);
    }
    return v;
}

bool ExistInVector(vector<Symbol>  &vec, string element)
{
    bool find = false;
    for (Symbol symbol: vec)
    {
        if(symbol.label == element) {
            find = true;
            break;
        }
    }

    return find;
}

Symbol FindInVector(vector<Symbol>  &vec, string element)
{
    Symbol find;
    for (Symbol symbol: vec)
    {
        if(symbol.label == element) {
            find = symbol;
            break;
        }
    }

    return find;
}

string toASCII(string input){
    ostringstream result;
    result << setw(2) << setfill('0') << hex << uppercase;
    copy(input.begin(), input.end(), ostream_iterator<unsigned int>(result, ""));
    return result.str();
}

string hexToString(int h, int length){
    string result = "";
    stringstream stream;
    stream << uppercase << hex << h;
    for (int i = 0; i < length - stream.str().length(); i++)
        result += "0";
    result += stream.str();
    return result;
}

void GenerateTextRecord(vector<string> textRecords){
    string record;
    string objCodeString;
    int textLen = 0;
    for (string text: textRecords)
    {
        textLen += text.length();
        objCodeString += text;
    }
    record = "T";
    record += hexToString(textStartFrom, 6);
    record += hexToString(textLen/2, 2);
    record += objCodeString;
    records.push_back(record);
}

void GetOpcodeRef(){
    Opcode opcode;
    ifstream ifs("opcode.txt", ios::in);
    if (!ifs.is_open()) {
        cout << "Failed to open file.\n";
    } else {
        string key;
        string value;
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
    st.opcodeStr = replaceReturnChar(v[1]);
    if(v.size() > 2) st.operand = replaceReturnChar(v[2]);

    return st;
}

void Pass1(Statement *st){
    if(st->opcodeStr == "START") {
        stringstream ss;
        ss << hex << st->operand;
        ss >> st->loc;
        currentLoc = st->loc;
        return;
    }
    st->loc = currentLoc;
    if(st->opcodeStr == "END" ) return;

    if(!st->label.empty()) {
        if(ExistInVector(symbolTable, st->label)){
            cout << "duplicate symbol in STMTAB" << endl;
        } else {
            Symbol symbol;
            symbol.label = st->label;
            symbol.address = currentLoc;
            symbolTable.push_back(symbol);
        }
    }

    for(int i=0; i < 60; i++){
        if(opcodeRef[i].key == st->opcodeStr){
            st->opcode = opcodeRef[i].value;
            break;
        }
    }
    
    // calaulate loc
    int loc = 0;
    bool foundOpcodeInOPTAB =  !st->opcode.empty();
    if(foundOpcodeInOPTAB || st->opcodeStr == "WORD") {
        loc = currentLoc + 3;
    } else if(st->opcodeStr == "RESW") {
        loc = currentLoc + stoi(st->operand) * 3;
    } else if(st->opcodeStr == "RESB") {
        loc = currentLoc + stoi(st->operand);
    } else if(st->opcodeStr == "BYTE") {
        vector<string> v = splitString(st->operand, '\''); 
        int b = 0;
        if(v[0] == "C"){
            b = v[1].length();
        } else if(v[0] == "X"){
            b = v[1].length()/2;
        }
        
        loc = currentLoc + b;
    } else {
        cout << "invalid operation code" << endl;
    }

    currentLoc = loc; 
}

Statement Pass2(Statement st){
    if(st.opcodeStr == "START") {
        string objectProgram = "H" + st.label + hexToString(st.loc, 6) + hexToString(programLength, 6);
        records.push_back(objectProgram);
        return st;
    }
    if(st.opcodeStr == "END" ) {
        GenerateTextRecord(textRecords);
        // Genetate end code
        string objectProgram = "E" + hexToString(programStartFrom, 6);
        records.push_back(objectProgram);
        return st;
    }

    bool foundOpcodeInOPTAB =  !st.opcode.empty();
    int incObjCodeNum = 0;
    if(foundOpcodeInOPTAB){
        if(!st.operand.empty()){
            string operandSymbol;
            int XEnable = 0;
            vector<string> v = splitString(st.operand, ',');
            if(v.size() > 1){
                // if x is enable, update the x bit the address code. (8*4096 in decimal)
                XEnable = 8*4096;
                operandSymbol = v[0];
            } else {
                operandSymbol = st.operand;
            }

            if(ExistInVector(symbolTable, operandSymbol)){
                Symbol s = FindInVector(symbolTable, operandSymbol);
                st.objCode = st.opcode + hexToString(s.address + XEnable, 4);
            } else {
                st.objCode = st.opcode + "0000";
                cout << "undefined symbol in operand" << endl;
            }
        } else {
            st.objCode = st.opcode + "0000";
        }
        incObjCodeNum =  3;
    } else if (st.opcodeStr == "BYTE") {
        vector<string> v = splitString(st.operand, '\'');

        int b = 0;
        if(v[0] == "C"){
            st.objCode = toASCII(v[1]);
            b = v[1].length();
        } else if(v[0] == "X"){
            st.objCode = v[1];
            b = v[1].length()/2;
        }
        incObjCodeNum = b;
    } else if(st.opcodeStr == "WORD"){
        st.objCode = st.objCode + hexToString(stoi(st.operand), 6);
        incObjCodeNum = 3;
    }

    objCodeLenCnt += incObjCodeNum;
    if (st.opcodeStr == "RESW" || st.opcodeStr == "RESB"){
        if(textRecords.size() > 0)
            GenerateTextRecord(textRecords);
        textRecords.clear();
        objCodeLenCnt = 0;
        return st;
    }
    
    if(objCodeLenCnt > 30){
        GenerateTextRecord(textRecords);
        textRecords.clear();
        objCodeLenCnt = incObjCodeNum;
    }

    // push new text record in textRecords
    if(textRecords.size() == 0) textStartFrom = st.loc;
    textRecords.push_back(st.objCode);

    return st;
}

void WriteOutput(string filename, vector<Statement> statementArray){
    ofstream myfile (filename);
    if (myfile.is_open())
    {
        myfile << "Loc" << "\t\t" << "Source statement" << "\t\t" << "Object Code" << endl ;
        for (Statement st: statementArray)
        {
            myfile << uppercase << hex << st.loc << "\t";
            myfile << st.label << "\t" << st.opcodeStr << "\t" << st.operand << "\t" << st.objCode << endl;
        }
        myfile.close();
    }
    else cout << "Unable to open and write file" << filename;
}

void WriteSYMTAB(){
    ofstream myfile ("SYMTAB.txt");
    if (myfile.is_open())
    {
        myfile << "Label Name" << "\t" << "Address" << endl ;
        for (Symbol symbol: symbolTable)
        {
            myfile << symbol.label << "\t";
            myfile << uppercase << hex << symbol.address << endl;
        }
        myfile.close();
    }
    else cout << "Unable to open and write file";
}

void WriteObjectProgram(string filename, vector<string> records){
    ofstream myfile (filename);
    if (myfile.is_open())
    {
        for (string r: records)
        {
            myfile << r << endl;
        }
        myfile.close();
    }
    else cout << "Unable to open and write file" << filename;
}

int main()
{
    vector<Statement> statementArray;
    GetOpcodeRef();
    ifstream ifs("source.txt", ios::in);
    if (!ifs.is_open()) {
        cout << "Failed to open file.\n";
    } else {
        string command;
        while (getline(ifs, command)) {
            Statement st = ScanStatement(command);
            Pass1(&st);
            statementArray.push_back(st);
        }
        ifs.close();
    }
    ifs.close();
    programLength = statementArray.back().loc - statementArray.front().loc;
    programStartFrom = statementArray.front().loc;
    WriteOutput("IntermediateFile.txt", statementArray);
    WriteSYMTAB();
    
    vector<Statement> sourceProgram;
    for (Statement st: statementArray)
    {
        Statement newSt = Pass2(st);
        sourceProgram.push_back(newSt);
    }
    
    WriteOutput("SourceProgram.txt", sourceProgram);
    WriteObjectProgram("FinalObjectProgram.txt", records);
    cout << "finish write FinalObjectProgram\n";
    return 0;
}