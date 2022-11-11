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

Fano::Fano(const string& pathToFile, bool v) {
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
    makeTree(temporaryFreqArray, storedUsages.size(), head, v);

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
    keyFile.open(path, std::ios::binary);

    if (!keyFile.fail()){
        keyFile.clear();
    }

    char size = storedCode.size();
    keyFile.write(&size, 1);

    // Данные записываются: [символ][количество нулей в начале][HEX значения]
    // Если значение начинается с 0, то количество нулей до первой единицы записывается во вторые []
    //
    // '0100', HEX представление 4 = '100', но из этого не построится дерево.
    // То есть запишется: [символ][01][04]
    //
    // Если значение начинается с 1, то ничего не трубется.
    // То есть запишется: [символ][00][значение]

    for (int i = 0; i < storedCode.size(); i++){
        char first = storedCode.at(i).first;
        char second = stoi(storedCode.at(i).second, nullptr, 2);

        char null = 0;
        while ((storedCode.at(i).second[(int)null] == '0') && (null < storedCode.at(i).second.size())){
            null++;
        }

        keyFile.write(&first, 1);
        keyFile.write(&null, 1);
        keyFile.write(&second, 1);
        std::cout << std::dec << i << " ";
        if ((i % 100 == 0) && (i > 100)){
            std::cout << std::endl;
        }
    }

    keyFile.close();
}

void Fano::generateArchived(const string &pathToFile, bool v) {
    std::ofstream archiveFile(pathToFile + ".archive", std::ios::binary | std::ios::out | std::ios::app);
//    if (archiveFile.fail()){
//        cout << endl << "Fano.cpp::126::9 -- generateArchived() returns /* UNREACHABLE IMPORT FILE */";
//    }

    std::ifstream streamFile(pathToFile, std::ios::binary | std::ios::in);
    if (streamFile.fail()){
        cout << endl << "Fano.cpp::132::9 -- generateArchived() returns /* UNREACHABLE IMPORT FILE */";
        exit(3);
    }

    string stream;
    char ch;

    if (v) cout << endl << "Scanning... ";
    unsigned long i = 0;
    while(streamFile.read(&ch, 1)){
        if ((i % 1000000 == 0) && (i > 0) && (v)){
            std::cout << "Scanned from imported file " << std::dec << i << " bytes." << std::endl;
        }
        auto it = storedCode.begin();
        while ((it != storedCode.end()) && (it->first != ch)) ++it;

        if (it == storedCode.end()){
            cout << "Fano.cpp::generateArchived(const string &pathToFile)::127::19 | Not found relation between [streamFile >> ch] and [storedCode]";
            exit(4);
        }

        stream += it->second;
        i++;
    }

    char size = stream.size() % 8;
    std::cout << "size: " << (int)size;
    archiveFile.write(&size, 1);

    i = 0;
    while (stream.length() > 0){
        if ((i % 1000000 == 0) && (i > 0) && (v)){
            std::cout << "Writed to export file " << std::dec << i << " bytes." << std::endl;
        }
        i++;
        if (stream.length() > 8){
            string k(stream, 0, 8);
            char outCh = stoi(k, nullptr, 2);
            archiveFile.write(&outCh, 1);
            stream.erase(0, 8);
            continue;
        }

        while (stream.length() != 8) stream.push_back('0');
        char outCh = stoi(stream, nullptr, 2);
        archiveFile.write(&outCh, 1);
        break;
    }

    archiveFile.close();
    streamFile.close();
}