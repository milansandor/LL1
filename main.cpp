#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <fstream>


using namespace std;


map<char, vector<string>> GRAMMAR;          
map<char, set<char>> FIRST;                 
map<char, set<char>> FOLLOW;                
set<char> nonTerminalSymbol;                
set<char> terminalSymbol;                   
map<char, bool> toEpsilon;                  
map<string, vector<string>> analysisTable;                 
bool judge_LL1_third = false;               
char startNonTerminalChar;                  
string expressionStr;                      
bool analysisResult = true;                 

void getFIRST() {   
    bool update = true;
    while (update) {
        update = false;
        for (char nonTerminalChar : nonTerminalSymbol) { 
            int firstSize = FIRST[nonTerminalChar].size();
            for (string rightSide : GRAMMAR[nonTerminalChar]) {  
                for (char ch : rightSide) {     
                    if (!isupper(ch)) {     
                        FIRST[nonTerminalChar].insert(ch);  
                        break;      
                    } else {
                        bool flag = false;      
                        for (char temp : FIRST[ch]) {   
                            if (temp == '@') {
                                flag = true;
                            }
                            FIRST[nonTerminalChar].insert(temp);
                        }
                        if (!flag) {        
                            break;
                        }
                    }
                }
            }
            if (firstSize != FIRST[nonTerminalChar].size()) {
                update = true;
            }
        }
    }
    for (char nonTerminalChar: nonTerminalSymbol) {
        toEpsilon[nonTerminalChar] = FIRST[nonTerminalChar].find('@') != FIRST[nonTerminalChar].end();
    }
}

void getFOLLOW() {  
    bool update = true;
    while (update) {
        update = false;
        for (char nonTerminalChar : nonTerminalSymbol) { 
            int followSize = FOLLOW[nonTerminalChar].size();    
            for (auto iter = GRAMMAR.begin(); iter != GRAMMAR.end(); iter++) {      
                for (string rightSide : iter->second) {      
                    int i = 0;
                    while (i < rightSide.length()) {    
                        for (; i < rightSide.length(); i++) {
                            if (nonTerminalChar == rightSide[i]) {  
                                if (i == rightSide.length() - 1) { 
                                    for (char ch : FOLLOW[iter->first]) {   
                                        FOLLOW[nonTerminalChar].insert(ch);
                                    }
                                }
                                i++;
                                break;  
                            }
                        }
                        for (; i < rightSide.length(); i++) {   
                            if (!isupper(rightSide[i])) {   
                                FOLLOW[nonTerminalChar].insert(
                                        rightSide[i]);   
                                break;      
                            } else {        
                                for (char ch : FIRST[rightSide[i]]) {   
                                    if (ch != '@') {
                                        FOLLOW[nonTerminalChar].insert(ch);
                                    }
                                }
                                if (!toEpsilon[rightSide[i]]) {     
                                    break;
                                } else if (i == rightSide.length() - 1) {        
                                    for (char ch : FOLLOW[iter->first]) {
                                        FOLLOW[nonTerminalChar].insert(
                                                ch);     
                                    }
                                }
                            }
                            if (i == rightSide.length() - 1 &&
                                rightSide[i] == nonTerminalChar) { 
                                for (char ch : FOLLOW[iter->first]) {
                                    FOLLOW[nonTerminalChar].insert(
                                            ch); 
                                }
                            }
                        }
                    }
                }
            }
            if (followSize != FOLLOW[nonTerminalChar].size()) {
                update = true;
            }
        }
    }
}

void printFIRST() {     
    cout << "First: " << endl;
    for (char chr : nonTerminalSymbol) {
        cout << chr << ": ";
        set<char> first;
        first = FIRST[chr];
        for (char ch : first) {
            cout << ch << " ";
        }
        cout << endl;
    }
    cout << endl;
}

void printFOLLOW() {    
    cout << "Follow: " << endl;
    for (char chr : nonTerminalSymbol) {
        cout << chr << ": ";
        set<char> follow;
        follow = FOLLOW[chr];
        for (char ch : follow) {
            cout << ch << " ";
        }
        cout << endl;
    }
    cout << endl;
}

void init() {   
    ifstream stream;
    stream.open("grammar.txt");
    if (!stream.is_open()) {
        cout << "Nem lehet megnyitni: grammar.txt" << endl;
        exit(EXIT_FAILURE);
    }
    string temp;
    bool isFirstNonTerminalChar = true;
    cout << "Szabályok:" << endl;
    while (getline(stream, temp)) {
        int nPos = 0;
        while ((nPos = temp.find(" ", nPos)) != temp.npos) {
            temp.replace(nPos, 1, "");
        }
        GRAMMAR[temp[0]].push_back(temp.substr(3));
        if (isFirstNonTerminalChar) {
            FOLLOW[temp[0]].insert('$');
            startNonTerminalChar = temp[0];
            isFirstNonTerminalChar = false;
        }
        nonTerminalSymbol.insert(temp[0]);      
        for (int i = 3; i < temp.length(); i++) {
            if (!isupper(temp[i]) && temp[i] != '@') {
                terminalSymbol.insert(temp[i]);     
            }
        }
        cout << temp << endl;
    }
    cout << endl;
    cout << "Nem terminális: " << endl;
    for (char nonTerminalChar:nonTerminalSymbol) {
        cout << nonTerminalChar << " ";
    }
    cout << endl << endl;
    cout << "Terminális: " << endl;
    for (char terminalChar : terminalSymbol) {
        cout << terminalChar << " ";
    }
    cout << endl << endl;
    stream.close();
    stream.open("expression.txt");
    if (!stream.is_open()) {
        cout << "Nem lehet megnyitni: expression.txt" << endl;
        exit(EXIT_FAILURE);
    }
    stream >> expressionStr;
    stream.close();
}

set<char> getRightFirst(string ss) { 
    set<char> s;
    bool flag = false;
    if (ss == "@") {
        judge_LL1_third = true;
    }
    for (char c : ss) {
        flag = false;
        if (!isupper(c)) {
            s.insert(c);
            break;
        } else {
            set<char> first = FIRST[c];
            for (auto j : first) {
                if (j == '@') {
                    flag = true;
                } else {
                    s.insert(j);
                }
            }
            if (!flag) {
                break;
            }
        }
    }
    if (flag) {
        judge_LL1_third = true;
    }
    return s;
}

void createAnalysisTable() {
    for (auto iter = GRAMMAR.begin(); iter != GRAMMAR.end(); iter++) {      
        for (string rightSide : iter->second) {     
            set<char> rightSideFirst = getRightFirst(rightSide);    
            for (char first : rightSideFirst) {     
                if (first == '@') {     
                    for (char follow : FOLLOW[iter->first]) {   
                        string temp;
                        temp.push_back(iter->first);
                        temp.push_back(follow);
                        if (analysisTable.find(temp) == analysisTable.end()) {   
                            string temp2;
                            temp2.push_back(iter->first);
                            analysisTable[temp].push_back(temp2);
                            for (char ch : rightSide) {
                                temp2.clear();
                                temp2.push_back(ch);
                                analysisTable[temp].push_back(temp2);
                            }
                        } else {    
                            cout << "Analizált tábla készítése sikertelen!" << endl;
                            exit(EXIT_FAILURE);
                        }
                    }
                    continue;
                }
                string temp;
                temp.push_back(iter->first);
                temp.push_back(first);
                if (analysisTable.find(temp) == analysisTable.end()) {   
                    string temp2;
                    temp2.push_back(iter->first);
                    analysisTable[temp].push_back(temp2);
                    for (char ch : rightSide) {
                        temp2.clear();
                        temp2.push_back(ch);
                        analysisTable[temp].push_back(temp2);
                    }
                } else {    
                    cout << "Analizált tábla készítése sikertelen!" << endl;
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
    for (char nonTerminalChar : nonTerminalSymbol) {        
        for (char follow : FOLLOW[nonTerminalChar]) {
            string temp;
            temp.push_back(nonTerminalChar);
            temp.push_back(follow);
            if (analysisTable.find(temp) == analysisTable.end()) {
                analysisTable[temp].push_back("synch");
            }
        }
    }
}

void printAnalysisTable() {
    set<char> terminalSymbolWithDollor = terminalSymbol;
    terminalSymbolWithDollor.insert('$');
    cout << " ";
    for (char terminalChar : terminalSymbolWithDollor) {
        string temp = "";
        temp += terminalChar;
        cout << terminalChar << " ";
    }
    cout << "\n";
    
    for (char nonTerminalChar :nonTerminalSymbol) {
        cout << nonTerminalChar << ":" << " ";
        for (char terminalChar : terminalSymbolWithDollor) {
            string temp = "";
            temp += nonTerminalChar;
            temp += terminalChar;
            auto iter = analysisTable.find(temp);
            if (iter != analysisTable.end()) {
                string expressionStr = "";
                if (iter->second.size() == 1 && *iter->second.begin() == "synch") {
                    expressionStr = "synch";
                } else {
                    expressionStr += nonTerminalChar;
                    expressionStr += "->";
                    auto iter2 = iter->second.begin();
                    iter2++;
                    for (; iter2 != iter->second.end(); iter2++) {
                        expressionStr += *iter2;
                    }
                }
                cout << expressionStr << " | ";
            } else {
                cout << " - " << " | ";
            }
        }
        cout << "\n";
    }
}

void analysis() {
    
    vector<string> analysisStack;
    
    analysisStack.push_back("$");
    string temp;
    temp.push_back(startNonTerminalChar);
    analysisStack.push_back(temp);
    int index = 0;  
    string printStack = "";
    string printInput = "";
    string printAction = "";
    while (analysisStack.back() != "$") {
        for (string stackStr : analysisStack) {     
            printStack += stackStr;
        }
        for (int i = index; i < expressionStr.size(); i++) {
            printInput += expressionStr[i];
        }
        
        //cout << "stack: " << printStack << " input: " << printInput << " ";
        printStack = "";
        printInput = "";
        printAction = "";
        string a;
        a.push_back(expressionStr[index]);
        temp.clear();
        temp = analysisStack.back() + a;
        auto iter = analysisTable.find(temp);
        if (analysisStack.back() == a) {   
            printAction += ("Match " + a);
            analysisStack.pop_back();   
            index++;       
        } else if (!isupper(analysisStack.back()[0])) {     
            printAction =
                    "Hiba az indexen " + to_string(index) + ", Stack top != " + a + ", pop " + analysisStack.back();
            analysisResult = false;
            analysisStack.pop_back();
        } else if (iter == analysisTable.end()) {   
            index++;
            printAction = "Hiba az indexen " + to_string(index) + ", ugrás " + a;
            analysisResult = false;
        } else if (iter != analysisTable.end()) {   
            vector<string> tempVector = iter->second;
            if (tempVector.size() == 1 && tempVector[0] == "synch") {   
                printAction = "Hiba az indexen " + to_string(index) + ", " + a + " ebben van benne " + analysisStack.back() +
                              " synch, pop " + analysisStack.back();
                analysisResult = false;
                analysisStack.pop_back();
            } else {
                
                printAction += "output: ";
                printAction += (tempVector[0] + " -> ");
                for (int i = 1; i < tempVector.size(); i++) {
                    printAction += tempVector[i];
                }
                analysisStack.pop_back();       
                for (int i = tempVector.size() - 1; i >= 1; i--) {
                    if (tempVector[i] != "@") {
                        analysisStack.push_back(tempVector[i]);     
                    }
                }
            }
        }
        
        //cout << printAction << "\n";
    }
    for (int i = index; i < expressionStr.length() - 1; i++) {  
        //cout << "$";
        string temp;
        for (int j = i; j < expressionStr.length(); j++) {
            temp.push_back(expressionStr[j]);
        }
        //cout << temp;
        temp.clear();
        temp.append("Hiba az indexen " + to_string(i) + ", ugrás ");
        temp.push_back(expressionStr[i]);
        //cout << temp;
        analysisResult = false;
    }
    //cout << "$" << "$" << "\n";
}

int main(int argc, char **argv) {
    init();
    getFIRST();     
    getFOLLOW();    
    printFIRST();
    printFOLLOW();
    createAnalysisTable();
    printAnalysisTable();
    analysis();
    if (analysisResult) {
        cout << expressionStr << " jó a string." << endl;
    } else {
        cout << expressionStr << " nem jó a string." << endl;
    }
    return 0;
    
}
