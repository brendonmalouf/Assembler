//
//  main.cpp
//  Project1
//
//  Created by Brendon Malouf on 10/16/18.
//  Copyright © 2018 Brendon Malouf. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
#define INF "P1.txt"

int Rtype(int op, int rs1, int rs2, int rd, int func);
int Jtype(int op, int offset);
int Itype(int op, int rs1, int rs2, int imm);

int main(int argc, const char * argv[]) {
    ifstream firstRead(INF);
    ifstream secondRead(INF);
    string line;
    int counter = 0;
    int tCounter = 0;
    vector<string> label;
    vector<int> labelByte; //used to find offset
    char* token;
    vector<string> tokens;
    vector<int> tBytes; //used to find offset
    
    string opNames[57] = {"J", "JAL", "BEQZ", "BNEZ", "JR", "JALR", "LB",
        "LH", "LW", "LBU", "LHU", "SB", "SH", "SW", "ADDI", "ADDUI", "SUBI",
        "SUBUI", "ANDI", "ORI", "XORI", "LHI", "SEQI", "SNEI", "SLTI", "SGTI",
        "SLEI", "SGEI", "SEQUI", "SNEUI", "SLTUI", "SGTUI", "SLEUI", "SGEUI",
        "SLLI", "SRLI", "SRAI", "SLL", "SRL", "SRA", "SLTU", "SGTU",
        "SLEU", "SGEU", "ADD", "ADDU", "SUB", "SUBU", "AND", "OR", "XOR",
        "SEQ", "SNE", "SLT", "SGT", "SLE", "SGE"};
    int opCodes[57] = {2, 3, 4, 5, 18, 19, 32, 33, 35, 36, 37, 40, 41, 43, 8,
        9, 10, 11, 12, 13, 14, 15, 24, 25, 26, 27, 28, 29, 48, 49, 50, 51, 52,
        53, 54, 55, 56, 4, 6, 7, 18, 19, 20, 21, 32, 33, 34, 35, 36, 37,
        38, 40, 41, 42, 43, 44, 45};
    
    
    cout << "TYPE:  INSTRUCTION(in hex):" << endl;
    //-------------------------------------------------------------------------
    // Identify labels and record offset.
    while (getline(firstRead, line))
    {
        string temp;
        int tempByte;
        
        counter++;
        for (int i = 0; i < line.size(); i++) line[i] = toupper(line[i]);
        
        for (int j = 0; j < line.size(); j++){
            if(line[j] == ':'){
                temp =  line.substr(0,line.find(":", 0));
                tempByte = (counter - 1) * 4;
                //cout << "LABEL: " << temp << " tempByte: " << tempByte << endl;
                label.push_back(temp);
                labelByte.push_back(tempByte);
            }
        }
    }
    
    //-------------------------------------------------------------------------
    // Ignore labels and parse out instructions into tokens.
    while(getline(secondRead, line)){
        tCounter++;
        
        for (int i = 0; i < line.size(); i++) line[i] = toupper(line[i]);
        
        for (int k = 0; k < line.size(); k++){
            if(line[k] == ':') line = line.substr(line.find(":") + 2 );
        }
        char* cLine = new char[line.length() + 1];
        strcpy(cLine, line.c_str());
        token = strtok(cLine, " ,()");
        while(token != NULL){
            string sToken(token);
            tokens.push_back(sToken);
            tBytes.push_back((tCounter * 4));
            token = strtok(NULL, " ,()");
        }
    }
    
    //-------------------------------------------------------------------------
    for(int i = 0; i < tokens.size(); i++){
        int rd = 0;
        int rs1 = 0;
        int rs2 = 0;
        int offset = 0;
        int imm = 0;
        int instr = 0;
        
        for(int j = 0; j < 2; j++){ //Checks for J-Type
            if(tokens[i] == opNames[j]){
                for (int m = 0; m < label.size(); m++){
                    if(tokens[i+1] == label[m])
                        offset = labelByte[m] - tBytes[i];
                }
                instr = Jtype(opCodes[j], offset);
                cout << hex << instr << endl;
            }
        }
        
        for(int k = 37; k < 57; k++){ //Checks for R-type
            if(tokens[i] == opNames[k]){
                tokens[i+1] = tokens[i+1].substr(1,2);
                rd = stoi(tokens[i+1]);
                
                tokens[i+2] = tokens[i+2].substr(1,2);
                rs1 = stoi(tokens[i+2]);
                
                tokens[i+3] = tokens[i+3].substr(1,2);
                rs2 = stoi(tokens[i+3]);
                
                instr = Rtype(0, rs1, rs2, rd, opCodes[k]);
                cout << hex << instr << endl;
            }
        }
        
        for(int m = 2; m < 4; m++){ //Handles Branches I-type
            if(tokens[i] == opNames[m]){
                tokens[i+1] = tokens[i+1].substr(1,2);
                rs1 = stoi(tokens[i+1]);
                
                for (int n = 0; n < label.size(); n++){
                    if(tokens[i+2] == label[n])
                        imm = labelByte[n] - tBytes[i];
                }
                instr = Itype(opCodes[m], rs1, 0, imm);
                cout << hex << instr << endl;
            }
        }
        
        for (int n = 4; n < 6; n++){ //Handles Jumps I-type
            if(tokens[i] == opNames[n]){
                tokens[i+1] = tokens[i+1].substr(1,2);
                rs1 = stoi(tokens[i+1]);
                
                instr = Itype(opCodes[n], rs1, 0, 0);
                cout << hex << instr << endl;
            }
        }
        
        for (int o = 6; o < 11; o++){ //Handles Loads I-type
            if(tokens[i] == opNames[o]){
                tokens[i+1] = tokens[i+1].substr(1,2);
                rs2 = stoi(tokens[i+1]);
                
                tokens[i+2] = tokens[i+2].substr(0,3);
                imm = stoi(tokens[i+2]);
                
                tokens[i+3] = tokens[i+3].substr(1,2);
                rs1 = stoi(tokens[i+3]);

                instr = Itype(opCodes[o], rs1, rs2, imm);
                cout << hex << instr << endl;
            }
        }
        
        for (int p = 11; p < 14; p++){ //Handles Stores I-type
            if(tokens[i] == opNames[p]){
                tokens[i+1] = tokens[i+1].substr(0,3);
                imm = stoi(tokens[i+1]);
                
                tokens[i+2] = tokens[i+2].substr(1,2);
                rs1 = stoi(tokens[i+2]);
                
                tokens[i+3] = tokens[i+3].substr(1,2);
                rs2 = stoi(tokens[i+3]);
                
                instr = Itype(opCodes[p], rs1, rs2, imm);
                cout << hex << instr << endl;
            }
        }
        
        for(int q = 14; q < 37; q++){ //Handles ALU I-type
            if(tokens[i] == opNames[q]){
                tokens[i+1] = tokens[i+1].substr(1,2);
                rs2 = stoi(tokens[i+1]);
                
                tokens[i+2] = tokens[i+2].substr(1,2);
                rs1 = stoi(tokens[i+2]);
                
                tokens[i+3] = tokens[i+3].substr(0,3);
                imm = stoi(tokens[i+3]);
                
                instr = Itype(opCodes[q], rs1, rs2, imm);
                cout << hex << instr << endl;
            }
        }
    }
    return 0;
}

int Rtype(int op, int rs1, int rs2, int rd, int func){
    int instr;
    op = op << 26;
    rs1 = rs1 << 21;
    rs2 = rs2 << 16;
    rd = rd << 11;
    cout << "RType: ";
    instr = op + rs1 + rs2 + rd + func;
    
    return instr;
}

int Jtype(int op, int offset){
    int instr;
    int mask = 0xFC000000;

    op = op << 26;
    if(offset < 0) offset = offset ^ mask; //Handles negative offset

    cout << "JType: ";
    instr = op + offset;
    return instr;
}

int Itype(int op, int rs1, int rs2, int imm){
    int instr;
    int mask = 0xFFFF0000;
    
    op = op << 26;
    rs1 = rs1 << 21;
    rs2 = rs2 << 16;
    if(imm < 0) imm = imm ^ mask; // Handles negative imm
    cout << "IType: ";
    instr = op + rs1 + rs2 + imm;
    
    return instr;
}





