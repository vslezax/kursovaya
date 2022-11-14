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

Node* readTree(std::ifstream &source, bool debug){
    Node* head = new Node;
    char count;
    source.read(&count, 1);

    while (count > 0) {
        // Считывание символа
        char value;
        source.read(&value, 1);
        // Считывание нулей
        char nulls;
        source.read(&nulls, 1);
        // Считывание значения
        char bit;
        source.read(&bit, 1);

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
        if (debug) cout << "Add: " << thisNode->returnValue() << " = " << thisNode->returnStr() << endl;
        count--;
    }
    if (debug) cout << endl;
    return head;
}

string readCompressedData(std::ifstream &source){
    char endBits;
    source.read(&endBits, 1);
    endBits = 8 - endBits;

    string stream;

    int ch;
    while (source.read((char*)&ch, 1)){
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
}

void decompress(std::ofstream& target, Node* treeHead, string& sourceData, bool debug){
    Node* head = treeHead;
    for (int i = 0; i < sourceData.length(); i++){
        if ((i % 200 == 0) && (i > 200) && (debug)){
            std::cout << "Scanned " << i << " elements..." << std::endl;
        }
        if ((treeHead->returnLeftNode() == nullptr) && (treeHead->returnRightNode() == nullptr)){
            char forHex = (char)treeHead->returnValue();
            target.write(&forHex, 1);
            treeHead = head;
        }
        if (sourceData.at(i) == '0') treeHead = treeHead->returnLeftNode();
        if (sourceData.at(i) == '1') treeHead = treeHead->returnRightNode();
        if (i == sourceData.length()){
            cout << "executeFano.cpp::decompile(const std::string& path)::185::5 | stream error -> not found relations for symbol";
            exit(3);
        }
    }
    if ((treeHead->returnLeftNode() == nullptr) && (treeHead->returnRightNode() == nullptr)){
        char forHex = (char)treeHead->returnValue();
        target.write(&forHex, 1);
    }
}

// Переопределение пути в вид \\root\\DECOMPILED[file]
string resultFileName(string fromPath){
    int it = fromPath.length();
    while (fromPath[it] != '\\') --it;
    ++it;
    fromPath.insert(it, "DECOMPILED");
    it = fromPath.length();
    while (fromPath[it-1] != '.'){
        --it;
        fromPath.pop_back();
    }
    fromPath.pop_back();
}

void decompile(const std::string& path, bool debug){
    std::ios_base::sync_with_stdio(false);

    cout << endl << "Working with '" << path << "'" << endl;

    std::ifstream streamFile(path, std::ios::binary | std::ios::in);
    if (streamFile.fail()){
        cout << "executeFano.cpp::decompile(const std::string& path)::92::9 | streamFile.fail() returns /* UNREACHABLE IMPORT FILE */";
        exit(2);
    }

    string newPath = resultFileName(path);

    std::ofstream decompiledFile(newPath, std::ios::binary | std::ios::out);
    if (!decompiledFile.fail()){
        decompiledFile.clear();
    }
    if (decompiledFile.fail()){
        cout << "executeFano.cpp::decompile(const std::string& path)::92::9 | decompiledFile.fail() returns /* UNREACHABLE IMPORT FILE */";
        exit(2);
    }

    std::cout << "Generating tree by stream data..." << std::endl;
    Node* treeHead = readTree(streamFile, debug);

    std::cout << "Generating bits stream from stream file..." << std::endl;
    string stream = readCompressedData(streamFile);


    std::cout << "Converting stream data to export file..." << std::endl;
    //std::cout << stream << std::endl;
    decompress(decompiledFile, treeHead, stream, debug);
    decompiledFile.close();
}