#include <fstream>
#include <iostream>
#include "Fano.h"
#include <algorithm>

using std::cout;
using std::vector;
using std::endl;

// https://iq.opengenus.org/sorting-vector-in-cpp/
bool compare(pair<char, int> a, pair<char, int> b){
    return a.second > b.second;
}

Fano::Fano(const string& pathToFile, char viewGeneration) {
    vector< pair<int, int> >::iterator iterator;

    std::ifstream file(pathToFile, std::ios::binary | std::ios::in);

    if (file.fail()){
        cout << "Fano.cpp::Fano(const string& pathToFile, char viewGeneration)::20::9 | Fano(const string& pathToFile, char viewGeneration) returns /* UNREACHABLE IMPORT FILE */";
        exit(2);
    }

    // Ввод символов
    int ch;
    while (file.read((char*)&ch, 1)){
        // Если такой символ уже есть
        iterator = findVectorIt(&storedUsages, ch);
        if (iterator == storedUsages.end()) storedUsages.emplace_back(ch, 1);
        else iterator->second++;
    }
    sort(storedUsages.begin(), storedUsages.end(), compare); // https://iq.opengenus.org/sorting-vector-in-cpp/

    Node* head = new Node;

    int* temporaryFreqArray = new int[storedUsages.size()];
    for (int i = 0; i < storedUsages.size(); i++){
        temporaryFreqArray[i] = storedUsages.at(i).second;
    }
    makeTree(temporaryFreqArray, storedUsages.size(), head, viewGeneration);

    // Сейчас обход по дереву, где char и binary string
    returnValues(head, 0, &storedCode);

    auto itUsages = storedUsages.begin();
    auto itCode = storedCode.begin();
    while (itUsages != storedUsages.end()){
        itCode->first = itUsages->first;
        ++itUsages;
        ++itCode;
    }


    // Исправление пропуска
    itCode = storedCode.begin();
    while (itCode != storedCode.end()){
        if (itCode->second.empty()) break;
        ++itCode;
    }
    if (itCode != storedCode.end()){
        itCode->second = (storedCode.end()-1)->second;
        storedCode.pop_back();
    }

    delete[] temporaryFreqArray;

    file.close();
}

Fano::~Fano() {
    storedUsages.clear();
    storedCode.clear();
}

vector< pair<int, int> >::iterator findVectorIt(vector<pair<int, int>>* vec, int value){
    vector< pair<int, int> >::iterator iterator;
    iterator = vec->begin();
    while (iterator != vec->end()){
        if (iterator->first == value) break;
        else iterator++;
    }
    return iterator;
}

void Fano::generateKeyFile(const string &path) {
    std::ofstream keyFile;
    keyFile.open(path);

    if (!keyFile.fail()){
        keyFile.clear();
    }

    for (int i = 0; i < storedCode.size(); i++){
        keyFile << (int)storedCode.at(i).first << " " << storedCode.at(i).second;
        if (i != storedCode.size() - 1) keyFile << endl;
    }

    keyFile.close();
}

void Fano::generateArchived(const string &pathToFile, char viewGeneration) {
    std::ofstream archiveFile(pathToFile + ".archive", std::ios::binary | std::ios::out);
    if (!archiveFile.fail()){
        archiveFile.clear();
    }

    std::ifstream streamFile(pathToFile, std::ios::binary | std::ios::in);
    if (streamFile.fail()){
        cout << endl << "Fano.cpp::108::9 -- generateArchived() returns /* UNREACHABLE IMPORT FILE */";
        exit(3);
    }

    string stream;
    char ch;

    while (streamFile.read(&ch, 1)){
        auto it = storedCode.begin();
        while ((it != storedCode.end()) && (it->first != ch)) ++it;

        if (it == storedCode.end()){
            cout << "Fano.cpp::generateArchived(const string &pathToFile)::127::19 | Not found relation between [streamFile >> ch] and [storedCode]";
            exit(4);
        }

        stream += it->second;

        if (viewGeneration == 'Y') cout << stream << endl;

        if (stream.length() >= 8){
            string outStr(stream, 0, 8);
            if (viewGeneration == 'Y'){
                string temp(stream, 8, stream.length());
                cout << "[" << outStr << "]" << temp << " = " << std::hex << std::uppercase << "[" << stoi(outStr, nullptr, 2) << "]" << endl;
            }
            char outCh = (char) stoi(outStr, nullptr, 2);
            archiveFile.write(&outCh, 1);
            stream.erase(0, 8);
        }
    }
    if (stream.length() >= 0){
        while (stream.length() < 8) stream.push_back('0');
        char outCh = (char) stoi(stream, nullptr, 2);
        if (viewGeneration == 'Y') cout << "[" << stream << "]" << " = " << std::hex << std::uppercase << "[" << (int)outCh << "]" << endl;
        archiveFile.write(&outCh, 1);
    }

    archiveFile.close();
    streamFile.close();
}