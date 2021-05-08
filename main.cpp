#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <fstream>


using namespace std;


map<char, vector<string>> GRAMMAR;          
map<char, set<char>> FIRST;                 
map<char, set<char>> FOLLOW;                
set<char> nemTerminalisSzimbolum;                
set<char> terminalisSzimbolum;                   
map<char, bool> toEpsilon;                  
map<string, vector<string>> analizisTabla;                 
bool LL1_harmadik_vizsgalata = false;               
char kezdoNemTerminalisChar;                  
string kifejezesString;                      
bool analizisEredmenye = true;                 

void getFIRST() {   
    bool update = true;
    while (update) {
        update = false;
        for (char nemTerminalisChar : nemTerminalisSzimbolum) { 
            int firstSize = FIRST[nemTerminalisChar].size();
            for (string jobbOldal : GRAMMAR[nemTerminalisChar]) {  
                for (char ch : jobbOldal) {     
                    if (!isupper(ch)) {     
                        FIRST[nemTerminalisChar].insert(ch);  
                        break;      
                    } else {
                        bool flag = false;      
                        for (char temp : FIRST[ch]) {   
                            if (temp == '@') {
                                flag = true;
                            }
                            FIRST[nemTerminalisChar].insert(temp);
                        }
                        if (!flag) {        
                            break;
                        }
                    }
                }
            }
            if (firstSize != FIRST[nemTerminalisChar].size()) {
                update = true;
            }
        }
    }
    for (char nemTerminalisChar: nemTerminalisSzimbolum) {
        toEpsilon[nemTerminalisChar] = FIRST[nemTerminalisChar].find('@') != FIRST[nemTerminalisChar].end();
    }
}

void getFOLLOW() {  
    bool update = true;
    while (update) {
        update = false;
        for (char nemTerminalisChar : nemTerminalisSzimbolum) { 
            int followSize = FOLLOW[nemTerminalisChar].size();    
            for (auto iter = GRAMMAR.begin(); iter != GRAMMAR.end(); iter++) {      
                for (string jobbOldal : iter->second) {      
                    int i = 0;
                    while (i < jobbOldal.length()) {    
                        for (; i < jobbOldal.length(); i++) {
                            if (nemTerminalisChar == jobbOldal[i]) {  
                                if (i == jobbOldal.length() - 1) { 
                                    for (char ch : FOLLOW[iter->first]) {   
                                        FOLLOW[nemTerminalisChar].insert(ch);
                                    }
                                }
                                i++;
                                break;  
                            }
                        }
                        for (; i < jobbOldal.length(); i++) {   
                            if (!isupper(jobbOldal[i])) {   
                                FOLLOW[nemTerminalisChar].insert(
                                        jobbOldal[i]);   
                                break;      
                            } else {        
                                for (char ch : FIRST[jobbOldal[i]]) {   
                                    if (ch != '@') {
                                        FOLLOW[nemTerminalisChar].insert(ch);
                                    }
                                }
                                if (!toEpsilon[jobbOldal[i]]) {     
                                    break;
                                } else if (i == jobbOldal.length() - 1) {        
                                    for (char ch : FOLLOW[iter->first]) {
                                        FOLLOW[nemTerminalisChar].insert(
                                                ch);     
                                    }
                                }
                            }
                            if (i == jobbOldal.length() - 1 &&
                                jobbOldal[i] == nemTerminalisChar) { 
                                for (char ch : FOLLOW[iter->first]) {
                                    FOLLOW[nemTerminalisChar].insert(
                                            ch); 
                                }
                            }
                        }
                    }
                }
            }
            if (followSize != FOLLOW[nemTerminalisChar].size()) {
                update = true;
            }
        }
    }
}

void printFIRST() {     
    cout << "First: " << endl;
    for (char chr : nemTerminalisSzimbolum) {
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
    for (char chr : nemTerminalisSzimbolum) {
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

void beolvasas() {   
    ifstream stream;
    stream.open("grammar.txt");
    if (!stream.is_open()) {
        cout << "Nem lehet megnyitni: grammar.txt" << endl;
        exit(EXIT_FAILURE);
    }
    string temp;
    bool FirstNemTerminalisChar = true;
    cout << "Szabályok:" << endl;
    while (getline(stream, temp)) {
        int nPos = 0;
        while ((nPos = temp.find(" ", nPos)) != temp.npos) {
            temp.replace(nPos, 1, "");
        }
        GRAMMAR[temp[0]].push_back(temp.substr(3));
        if (FirstNemTerminalisChar) {
            FOLLOW[temp[0]].insert('$');
            kezdoNemTerminalisChar = temp[0];
            FirstNemTerminalisChar = false;
        }
        nemTerminalisSzimbolum.insert(temp[0]);      
        for (int i = 3; i < temp.length(); i++) {
            if (!isupper(temp[i]) && temp[i] != '@') {
                terminalisSzimbolum.insert(temp[i]);     
            }
        }
        cout << temp << endl;
    }
    cout << endl;
    cout << "Nem terminális: " << endl;
    for (char nemTerminalisChar:nemTerminalisSzimbolum) {
        cout << nemTerminalisChar << " ";
    }
    cout << endl << endl;
    cout << "Terminális: " << endl;
    for (char terminalisChar : terminalisSzimbolum) {
        cout << terminalisChar << " ";
    }
    cout << endl << endl;
    stream.close();
    stream.open("expression.txt");
    if (!stream.is_open()) {
        cout << "Nem lehet megnyitni: expression.txt" << endl;
        exit(EXIT_FAILURE);
    }
    stream >> kifejezesString;
    stream.close();
}

set<char> getRightFirst(string ss) { 
    set<char> s;
    bool flag = false;
    if (ss == "@") {
        LL1_harmadik_vizsgalata = true;
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
        LL1_harmadik_vizsgalata = true;
    }
    return s;
}

void createAnalizisTabla() {
    for (auto iter = GRAMMAR.begin(); iter != GRAMMAR.end(); iter++) {      
        for (string jobbOldal : iter->second) {     
            set<char> jobbOldalFirst = getRightFirst(jobbOldal);    
            for (char first : jobbOldalFirst) {     
                if (first == '@') {     
                    for (char follow : FOLLOW[iter->first]) {   
                        string temp;
                        temp.push_back(iter->first);
                        temp.push_back(follow);
                        if (analizisTabla.find(temp) == analizisTabla.end()) {   
                            string temp2;
                            temp2.push_back(iter->first);
                            analizisTabla[temp].push_back(temp2);
                            for (char ch : jobbOldal) {
                                temp2.clear();
                                temp2.push_back(ch);
                                analizisTabla[temp].push_back(temp2);
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
                if (analizisTabla.find(temp) == analizisTabla.end()) {   
                    string temp2;
                    temp2.push_back(iter->first);
                    analizisTabla[temp].push_back(temp2);
                    for (char ch : jobbOldal) {
                        temp2.clear();
                        temp2.push_back(ch);
                        analizisTabla[temp].push_back(temp2);
                    }
                } else {    
                    cout << "Analizált tábla készítése sikertelen!" << endl;
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
    for (char nemTerminalisChar : nemTerminalisSzimbolum) {        
        for (char follow : FOLLOW[nemTerminalisChar]) {
            string temp;
            temp.push_back(nemTerminalisChar);
            temp.push_back(follow);
            if (analizisTabla.find(temp) == analizisTabla.end()) {
                analizisTabla[temp].push_back("synch");
            }
        }
    }
}

void printAnalizisTabla() {
    set<char> terminalisSzimbolumDollar = terminalisSzimbolum;
    terminalisSzimbolumDollar.insert('$');
    cout << " ";
    for (char terminalisChar : terminalisSzimbolumDollar) {
        string temp = "";
        temp += terminalisChar;
        cout << terminalisChar << " ";
    }
    cout << "\n";
    
    for (char nemTerminalisChar :nemTerminalisSzimbolum) {
        cout << nemTerminalisChar << ":" << " ";
        for (char terminalisChar : terminalisSzimbolumDollar) {
            string temp = "";
            temp += nemTerminalisChar;
            temp += terminalisChar;
            auto iter = analizisTabla.find(temp);
            if (iter != analizisTabla.end()) {
                string kifejezesString = "";
                if (iter->second.size() == 1 && *iter->second.begin() == "synch") {
                    kifejezesString = "synch";
                } else {
                    kifejezesString += nemTerminalisChar;
                    kifejezesString += "->";
                    auto iter2 = iter->second.begin();
                    iter2++;
                    for (; iter2 != iter->second.end(); iter2++) {
                        kifejezesString += *iter2;
                    }
                }
                cout << kifejezesString << " | ";
            } else {
                cout << " - " << " | ";
            }
        }
        cout << "\n";
    }
}

void analysis() {
    
    vector<string> analizisStack;
    
    analizisStack.push_back("$");
    string temp;
    temp.push_back(kezdoNemTerminalisChar);
    analizisStack.push_back(temp);
    int index = 0;  
    string StackKiirasa = "";
    string InputKiirasa = "";
    string ActionKiirasa = "";
    while (analizisStack.back() != "$") {
        for (string stackStr : analizisStack) {     
            StackKiirasa += stackStr;
        }
        for (int i = index; i < kifejezesString.size(); i++) {
            InputKiirasa += kifejezesString[i];
        }
        
        //cout << "stack: " << StackKiirasa << " input: " << InputKiirasa << " ";
        StackKiirasa = "";
        InputKiirasa = "";
        ActionKiirasa = "";
        string a;
        a.push_back(kifejezesString[index]);
        temp.clear();
        temp = analizisStack.back() + a;
        auto iter = analizisTabla.find(temp);
        if (analizisStack.back() == a) {   
            ActionKiirasa += ("Match " + a);
            analizisStack.pop_back();   
            index++;       
        } else if (!isupper(analizisStack.back()[0])) {     
            ActionKiirasa =
                    "Hiba az indexen " + to_string(index) + ", Stack top != " + a + ", pop " + analizisStack.back();
            analizisEredmenye = false;
            analizisStack.pop_back();
        } else if (iter == analizisTabla.end()) {   
            index++;
            ActionKiirasa = "Hiba az indexen " + to_string(index) + ", ugrás " + a;
            analizisEredmenye = false;
        } else if (iter != analizisTabla.end()) {   
            vector<string> tempVector = iter->second;
            if (tempVector.size() == 1 && tempVector[0] == "synch") {   
                ActionKiirasa = "Hiba az indexen " + to_string(index) + ", " + a + " ebben van benne " + analizisStack.back() +
                              " synch, pop " + analizisStack.back();
                analizisEredmenye = false;
                analizisStack.pop_back();
            } else {
                
                ActionKiirasa += "output: ";
                ActionKiirasa += (tempVector[0] + " -> ");
                for (int i = 1; i < tempVector.size(); i++) {
                    ActionKiirasa += tempVector[i];
                }
                analizisStack.pop_back();       
                for (int i = tempVector.size() - 1; i >= 1; i--) {
                    if (tempVector[i] != "@") {
                        analizisStack.push_back(tempVector[i]);     
                    }
                }
            }
        }
        
        //cout << ActionKiirasa << "\n";
    }
    for (int i = index; i < kifejezesString.length() - 1; i++) {  
        //cout << "$";
        string temp;
        for (int j = i; j < kifejezesString.length(); j++) {
            temp.push_back(kifejezesString[j]);
        }
        //cout << temp;
        temp.clear();
        temp.append("Hiba az indexen " + to_string(i) + ", ugrás ");
        temp.push_back(kifejezesString[i]);
        //cout << temp;
        analizisEredmenye = false;
    }
    //cout << "$" << "$" << "\n";
}

int main(int argc, char **argv) {
    beolvasas();
    getFIRST();     
    getFOLLOW();    
    printFIRST();
    printFOLLOW();
    createAnalizisTabla();
    printAnalizisTabla();
    analysis();
    if (analizisEredmenye) {
        cout << kifejezesString << " jó a string." << endl;
    } else {
        cout << kifejezesString << " nem jó a string." << endl;
    }
    return 0;
    
}
