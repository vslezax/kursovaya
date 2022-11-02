#include "executeFano.h"

#include <iostream>
#include <string>
#include <dirent.h>
#include <fstream>
#include <algorithm>

#include "Fano.h"

// https://www.tutorialspoint.com/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-cplusplus
void viewDirectory(const std::string& path){
    DIR *dir;
    struct dirent *files;
    dir = opendir(path.c_str()); //open all or present directory

    if (!dir){
        cout << "executeFano.cpp::viewDirectory(const std::string& path)::15::9 | dir returned FALSE, no directory in this path.";
        exit(1);
    }

    while ((files = readdir(dir)) != nullptr) {
        cout << path << '\\' << files->d_name << endl;
    }
    closedir(dir);
}

void startAlgorithm(const std::string& path){
    std::ios_base::sync_with_stdio(false);

    // Просмотр директории
    viewDirectory(path);
    string pathToImportFile;

    cout << endl << "Print name of file with extension (e.g. 'file.txt'):" << endl;
    std::cin >> pathToImportFile;
    cout << endl << "Working with '" << path + "\\" + pathToImportFile << "'" << endl;

    // Для просмотра построения дерева
    char viewGeneration;
    while ((viewGeneration != 'Y') && (viewGeneration != 'N')){
        cout << "View tree generation? Y/N ";
        std::cin >> viewGeneration;
        cout << endl;
    }

    // Считывание потока данных с файла
    // Заполнение storedUsages, подсчет количества использований
    Fano main(path + "\\" + pathToImportFile, viewGeneration);

    viewGeneration = 0;
    while ((viewGeneration != 'Y') && (viewGeneration != 'N')){
        cout << "View Fano data class? Y/N ";
        std::cin >> viewGeneration;
    }
    if (viewGeneration == 'Y') cout << main << endl;

    // Генератор keys
    main.generateKeys(path + "\\" + pathToImportFile + ".archive");

    viewGeneration = 0;
    while ((viewGeneration != 'Y') && (viewGeneration != 'N')){
        cout << "View keys table? Y/N ";
        std::cin >> viewGeneration;
        cout << endl;
    }
    if (viewGeneration == 'Y') std::cout << main;

    // Заполнение archive
    viewGeneration = 0;
    while ((viewGeneration != 'Y') && (viewGeneration != 'N')){
        cout << "View processing? Y/N ";
        std::cin >> viewGeneration;
        cout << endl;
    }
    main.generateArchived(path + "\\" + pathToImportFile + ".archive", viewGeneration);

    cout << endl << endl;
    cout << path + "\\" + pathToImportFile + ".archive  -  created.";
    cout << endl << "File successfully archived!" << endl << endl;
}

void decompile(const std::string& path){
    std::ios_base::sync_with_stdio(false);

    // Просмотр директории
    viewDirectory(path);
    string pathToImportFile;

    cout << endl << "Print name of file with extension (e.g. 'file.txt.archive'):" << endl;
    std::cin >> pathToImportFile;
    cout << endl << "Working with '" << path + "\\" + pathToImportFile << "'" << endl;
    std::ifstream streamFile(path + "\\" + pathToImportFile, std::ios::binary | std::ios::in);

    while (pathToImportFile.back() != '.') pathToImportFile.pop_back();

    std::ifstream keyFile(path + "\\" + pathToImportFile + "key");
    pathToImportFile.pop_back();
    std::ofstream decompiledFile(path + "\\" + "DECOMPILED" + pathToImportFile, std::ios::binary | std::ios::out);
    if (!decompiledFile.fail()){
        decompiledFile.clear();
    }

    if (decompiledFile.fail()){
        cout << "executeFano.cpp::decompile(const std::string& path)::92::9 | decompiledFile.fail() returns /* UNREACHABLE IMPORT FILE */";
        exit(2);
    }

    if (keyFile.fail()){
        cout << "executeFano.cpp::decompile(const std::string& path)::92::9 | keyFile.fail() returns /* UNREACHABLE IMPORT FILE */";
        exit(2);
    }

    char view;
    while ((view != 'Y') && (view != 'N')){
        cout << endl << "View tree generation? Y/N ";
        std::cin >> view;
    }

    Node* head = new Node;
    string line;
    /*while (getline(keyFile, line)) {
        if ((line.empty()) || (line.at(1) != ' ')) continue;

        Node* thisNode = head;
        string str;
        for (int i = 2; i < line.length(); i++){
            string k(line, 2, i);
            if (line.at(i) == '0'){
                if (thisNode->returnLeftNode() == nullptr) thisNode = thisNode->createLeft(str);
                else thisNode = thisNode->returnLeftNode();
                str = thisNode->returnStr();
                continue;
            }
            if (line.at(i) == '1'){
                if (thisNode->returnRightNode() == nullptr) thisNode = thisNode->createRight(str);
                else thisNode = thisNode->returnRightNode();
                str = thisNode->returnStr();
            }
        }
        string valueInLine;
        for (int i = 0; i < line.length(); i++){
            if (line[i] == ' ') break;
            cout << line[i];
        }
        cout << endl;
        //thisNode->setValue(stoi(valueInLine, nullptr, 10));
        if (view == 'Y') cout << "Add: " << thisNode->returnValue() << "[" << (char)thisNode->returnValue() << "] = " << thisNode->returnStr() << endl;
    }
    */
    string bits;
    int value;
    while (keyFile >> value >> bits) {

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
        if (view == 'Y') cout << "Add: " << thisNode->returnValue() << " = " << thisNode->returnStr() << endl;
    }

    keyFile.close();

    if (streamFile.fail()){
        cout << "executeFano.cpp::decompile(const std::string& path)::106::9 | streamFile.fail() returns /* UNREACHABLE IMPORT FILE */";
        exit(2);
    }

    view = 0;
    while ((view != 'Y') && (view != 'N')){
        cout << endl << "View archive file hex? Y/N ";
        std::cin >> view;
    }

    if (view == 'Y') cout << endl;

    string stream;

    int ch;
    while (streamFile.read((char*)&ch, 1)){
        int a = 0;
        string charTo8Bin;
        if (view == 'Y') cout << std::hex << std::uppercase << ch;
        while (ch){
            a++;
            if (ch & 1) charTo8Bin += '1';
            else charTo8Bin += '0';
            ch >>= 1;
        }
        std::reverse(charTo8Bin.begin(), charTo8Bin.end());
        if (view == 'Y') cout << " = " << "[" << charTo8Bin << "]";
        while (a < 8){
            charTo8Bin.insert(0, "0");
            a++;
        }
        if (view == 'Y') cout << " = " << "[" << charTo8Bin << "]" << endl;
        stream += charTo8Bin;
    }

    view = 0;
    while ((view != 'Y') && (view != 'N')){
        cout << endl << "View archive file binary stream? Y/N ";
        std::cin >> view;
    }

    if (view == 'Y') cout << endl << "Stream: " << stream << endl << endl;

    Node* thisNode = head;
    for (int i = 0; i < stream.length(); i++){
        if ((thisNode->returnLeftNode() == nullptr) && (thisNode->returnRightNode() == nullptr)){
            char forHex = (char)thisNode->returnValue();
            decompiledFile.write(&forHex, 1);
            if (view == 'Y') cout << "Founded: '" << (char)thisNode->returnValue() << "'" << ", return " << thisNode->returnValue() << endl;
            thisNode = head;
        }
        if (stream.at(i) == '0') thisNode = thisNode->returnLeftNode();
        if (stream.at(i) == '1') thisNode = thisNode->returnRightNode();
        if (i == stream.length()){
            cout << "executeFano.cpp::decompile(const std::string& path)::185::5 | stream error -> not found relations for symbol";
            exit(3);
        }
    }

    decompiledFile.close();
}