#include "executeFano.h"

#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>

#include "Fano.h"


void err(){
    std::cout << "Wrong arguments" << std::endl;
    std::cout << "Use: algorithmFano.exe [mode] [view] [path]" << std::endl;
    std::cout << "Example 1: algorithmFano.exe -c -v c:/file.txt" << std::endl;
    std::cout << "Example 2: algorithmFano.exe -d c:/file.txt.archive" << std::endl;
}

void startAlgorithm(const std::string& path, bool v){
    std::ios_base::sync_with_stdio(false);

    cout << endl << "Working with '" << path << "'" << endl;

    // Считывание потока данных с файла
    // Заполнение storedUsages, подсчет количества использований
    Fano main(path, v);

    if (v) cout << main << endl;

    std::cout << std::endl << "Generating keys..." << std::endl;

    // Генератор keys
    main.generateKeyFile(path + ".archive");

    std::cout << std::endl << "Generating data stream..." << std::endl;

    // Создание archive
    main.generateArchived(path, v);

    cout << endl << endl;
    cout << path + ".archive  -  created.";
    cout << endl << "File successfully archived!" << endl << endl;
}

void decompile(const std::string& path, bool v){
    std::ios_base::sync_with_stdio(false);

    cout << endl << "Working with '" << path << "'" << endl;

    std::ifstream streamFile(path, std::ios::binary | std::ios::in);
    if (streamFile.fail()){
        cout << "executeFano.cpp::decompile(const std::string& path)::92::9 | streamFile.fail() returns /* UNREACHABLE IMPORT FILE */";
        exit(2);
    }

    // Переопределение пути в вид \\root\\DECOMPILED[file]
    string newPath(path);
    int it = newPath.length();
    while (newPath[it] != '\\') --it;
    ++it;
    newPath.insert(it, "DECOMPILED");
    it = newPath.length();
    while (newPath[it-1] != '.'){
        --it;
        newPath.pop_back();
    }
    newPath.pop_back();

    std::ofstream decompiledFile(newPath, std::ios::binary | std::ios::out);
    if (!decompiledFile.fail()){
        decompiledFile.clear();
    }
    if (decompiledFile.fail()){
        cout << "executeFano.cpp::decompile(const std::string& path)::92::9 | decompiledFile.fail() returns /* UNREACHABLE IMPORT FILE */";
        exit(2);
    }

    std::cout << "Generating tree by stream data..." << std::endl;
    Node* head = new Node;
    char count;
    streamFile.read(&count, 1);

    while (count > 0) {
        // Считывание символа
        char value;
        streamFile.read(&value, 1);
        // Считывание нулей
        char nulls;
        streamFile.read(&nulls, 1);
        // Считывание значения
        char bit;
        streamFile.read(&bit, 1);

        string bits;
        while (bit){
            bits.push_back((bit & 1) + '0');
            bit >>= 1;
        }
        std::reverse(bits.begin(), bits.end());
        while (nulls > 0){
            bits.insert(0, "0");
            nulls--;
        }

        Node* thisNode = head;
        string str;
        for (int i = 0; i < bits.length(); i++){
            if (bits.at(i) == '0'){
                if (thisNode->returnLeftNode() == nullptr) thisNode = thisNode->createLeft(str);
                else thisNode = thisNode->returnLeftNode();
                str = thisNode->returnStr();
                continue;
            }
            if (bits.at(i) == '1'){
                if (thisNode->returnRightNode() == nullptr) thisNode = thisNode->createRight(str);
                else thisNode = thisNode->returnRightNode();
                str = thisNode->returnStr();
            }
        }
        thisNode->setValue(value);
        if (v) cout << "Add: " << thisNode->returnValue() << " = " << thisNode->returnStr() << endl;
        count--;
    }

    if (v) cout << endl;

    char endBits;
    streamFile.read(&endBits, 1);
    endBits = 8 - endBits;

    std::cout << "Generating bits stream from stream file..." << std::endl;

    string stream;

    int ch;
    while (streamFile.read((char*)&ch, 1)){
        int a = 0;
        string charToBin;
        while (ch){
            a++;
            if (ch & 1) charToBin += '1';
            else charToBin += '0';
            ch >>= 1;
        }
        std::reverse(charToBin.begin(), charToBin.end());
        while (a < 8){
            charToBin.insert(0, "0");
            a++;
        }
        stream += charToBin;
    }

    while (endBits > 0){
        stream.pop_back();
        endBits--;
    }

    std::cout << "Converting stream data to export file..." << std::endl;
    //std::cout << stream << std::endl;

    Node* thisNode = head;
    for (int i = 0; i < stream.length(); i++){
        if ((i % 200 == 0) && (i > 200) && (v)){
            std::cout << "Scanned " << i << " elements..." << std::endl;
        }
        if ((thisNode->returnLeftNode() == nullptr) && (thisNode->returnRightNode() == nullptr)){
            char forHex = (char)thisNode->returnValue();
            decompiledFile.write(&forHex, 1);
            thisNode = head;
        }
        if (stream.at(i) == '0') thisNode = thisNode->returnLeftNode();
        if (stream.at(i) == '1') thisNode = thisNode->returnRightNode();
        if (i == stream.length()){
            cout << "executeFano.cpp::decompile(const std::string& path)::185::5 | stream error -> not found relations for symbol";
            exit(3);
        }
    }
    if ((thisNode->returnLeftNode() == nullptr) && (thisNode->returnRightNode() == nullptr)){
        char forHex = (char)thisNode->returnValue();
        decompiledFile.write(&forHex, 1);
    }

    decompiledFile.close();
}