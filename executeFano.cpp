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

    std::cout << std::endl << "Working with '" << path << "'" << std::endl;

    // Считывание потока данных с файла
    // Заполнение storedUsages, подсчет количества использований
    Fano main(path, v);

    if (v) std::cout << main << std::endl;

    std::cout << std::endl << "Generating keys..." << std::endl;

    if (!main.isEfficiency()){
        std::cout << "Attention: this file compressed inefficient." << std::endl;
        main.generateInefficient(path);
        std::cout << std::endl << std::endl;
        std::cout << path + ".archive  -  created.";
        std::cout << std::endl << "File successfully archived!" << std::endl << std::endl;
        return;
    }

    // Генератор keys
    main.generateKeyFile(path + ".archive");

    std::cout << std::endl << "Generating data stream..." << std::endl;

    // Создание archive
    main.generateArchived(path, v);

    std::cout << std::endl;
    std::cout << path + ".archive  -  created.";
    std::cout << std::endl << "File successfully archived!";
}

void readInt(std::ifstream &source, string& binary, int& valueExport){
    // Считывается значение
    char value;
    source.read(&value, 1);
    valueExport = value;
    // Считывание количества байт
    char bitsCount;
    source.read(&bitsCount, 1);

    while (bitsCount > 0){
        // Считывание нулей
        char nulls;
        source.read(&nulls, 1);
        // Считывание части строки
        char byte;
        source.read(&byte, 1);

        string bits;
        unsigned char bit = byte;
        while (bit){
            bits.push_back((bit & 1) + '0');
            bit >>= 1;
        }
        std::reverse(bits.begin(), bits.end());
        while (nulls > 0){
            bits.insert(0, "0");
            nulls--;
        }
        binary += bits;

        bitsCount--;
    }
}

Node* readTree(std::ifstream &source, bool debug){
    Node* head = new Node;
    char count;
    source.read(&count, 1);

    while (count > 0) {
        int value;
        string bits;

        readInt(source, bits, value);

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
        if (debug) std::cout << "Add: " << thisNode->returnValue() << " = " << thisNode->returnStr() << std::endl;
        count--;
    }
    if (debug) std::cout << std::endl;
    return head;
}

void decompress(std::ofstream& target, Node* treeHead, std::ifstream& sourceData, bool debug){
    string stream;

    char endBits;
    sourceData.read(&endBits, 1);
    endBits = 8 - endBits;

    char byte;
    Node* thisNode = treeHead;
    while (sourceData.read(&byte, 1)){
        int a = 0;
        string charToBin;
        unsigned char ch = byte;
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

        if (sourceData.peek() == EOF){
            while (endBits > 0){
                stream.pop_back();
                endBits--;
            }
        }
        while (true){
            if ((thisNode->returnLeftNode() == nullptr) && (thisNode->returnRightNode() == nullptr)){
                char forHex = (char)thisNode->returnValue();
                target.write(&forHex, 1);
                thisNode = treeHead;
            }
            if (stream.length() == 0) break;
            if (stream.at(0) == '0') thisNode = thisNode->returnLeftNode();
            if (stream.at(0) == '1') thisNode = thisNode->returnRightNode();
            string temp = stream.substr(1);
            stream = temp;
        }
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

    return fromPath;
}

void decompressInefficient(std::ifstream& streamFile, std::ofstream& decompiledFile){
    std::cout << "Copying data from stream file to decompiled file..." << std::endl;
    std::cout << "Converting stream data to export file..." << std::endl;
    char ch;
    while (streamFile.read(&ch, 1)) decompiledFile.write(&ch, 1);
}

bool isInefficient(std::ifstream& file){
    char check;
    file >> check;
    if (check == 0){
        std::cout << "Attention: this file was compressed inefficient." << std::endl;
        return true;
    }
    return false;
}

void decompile(const std::string& path, bool debug){
    std::ios_base::sync_with_stdio(false);

    std::cout << std::endl << "Working with '" << path << "'" << std::endl;

    std::ifstream streamFile(path, std::ios::binary | std::ios::in);
    if (streamFile.fail()){
        std::cout << "executeFano.cpp::decompile(const std::string& path)::92::9 | streamFile.fail() returns /* UNREACHABLE IMPORT FILE */";
        exit(2);
    }

    string newPath(resultFileName(path));

    std::ofstream decompiledFile(newPath, std::ios::binary | std::ios::out);
    if (!decompiledFile.fail()) decompiledFile.clear();
    if (decompiledFile.fail()){
        std::cout << "executeFano.cpp::decompile(const std::string& path)::92::9 | decompiledFile.fail() returns /* UNREACHABLE IMPORT FILE */";
        exit(2);
    }

    if (isInefficient(streamFile)){
        decompressInefficient(streamFile, decompiledFile);
        std::cout << std::endl << "" << newPath << " - created!" << std::endl;
        std::cout << "File successfully unpacked!" << std::endl;
        return;
    }

    std::cout << "Generating tree by stream data..." << std::endl;
    Node* treeHead = readTree(streamFile, debug);

    std::cout << "Converting stream data to export file..." << std::endl;
    decompress(decompiledFile, treeHead, streamFile, debug);
    std::cout << std::endl << "" << newPath << " - created!" << std::endl;
    std::cout << "File successfully unpacked!" << std::endl;
    decompiledFile.close();
}