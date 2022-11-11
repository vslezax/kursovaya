#pragma once

#include <string>
#include <iostream>
#include <vector>

using std::string;
using std::pair;
using std::vector;

class Node{
private:
    int value;
    string bits;
    Node* left;
    Node* right;
public:
    Node();
    int returnValue() const;
    explicit Node(const string& str);
    void setValue(int value);
    Node* createLeft(const string& str);
    Node* createLeft();
    Node* createRight(const string& str);
    Node* createRight();
    Node* returnLeftNode();
    Node* returnRightNode();
    string returnStr();
    void setString(const string& str);
    ~Node();
};

string renameStr(string str);
void print(Node* node, int u);
void returnValues(Node* node, int u, vector< pair<char, string> >* code);
void makeTree(const int* array, int size, Node* node, bool v);