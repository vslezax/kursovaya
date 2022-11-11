#include "tree.h"

using std::cout;
using std::endl;

Node::Node(){
    right = nullptr;
    left = nullptr;
    value = 0;
    bits.clear();
}

int Node::returnValue() const{
    return value;
}

Node::Node(const string& str){
    left = nullptr;
    right = nullptr;
    value = 0;
    bits = str;
}

// Очищение строки от лишних нулей в начале
string renameStr(string str){
    while (str[0] == '0'){
        str.erase(str.begin());
    }
    if (str.empty()) str = "0";
    return str;
}

void Node::setString(const string& str){
    bits = str;
}

string Node::returnStr(){
    return bits;
}

// Создание левого нода.
// Добавление '0' в конец, так как это левый нод.
// Возвращение ссылки на созданный нод.
Node* Node::createLeft(const string& str){
    left = new Node(str + "0");
    return this->left;
}

Node *Node::createLeft() {
    this->left = new Node;
    return this->left;
}

// Создание правого нода.
// Добавление '1' в конец, так как это левый нод.
// Возвращение ссылки на созданный нод.
Node* Node::createRight(const string& str){
    right = new Node(str + "1");
    return this->right;
}

Node *Node::createRight() {
    this->right = new Node;
    return this->right;
}

void Node::setValue(int value) {
    this->value = value;
}

Node* Node::returnLeftNode(){
    return this->left;
}
Node* Node::returnRightNode(){
    return this->right;
}

Node::~Node() {
    value = 0;
    bits.clear();
}

void print(Node* node, int u){
    if (node == nullptr) return; //Если дерево пустое, то отображать нечего, выходим
    else
    {
        print(node->returnLeftNode(), ++u);//С помощью рекурсивного посещаем левое поддерево
        node->setString(node->returnStr()); // Если есть, убираем лишние нули в начале
        if (node->returnValue() != 0){
            cout << "'" << node->returnValue() << "' = "<< node->returnStr() << endl;
            //cout << "'" << node->returnValue() << "' = "<< renameStr(node->returnStr()) << endl;
        }
        u--;
    }
    print(node->returnRightNode(), ++u); //С помощью рекурсии посещаем правое поддерево
}

void returnValues(Node* node, int u, vector< pair<char, string> >* code){
    if (node == nullptr) return; //Если дерево пустое, то отображать нечего, выходим
    else
    {
        returnValues(node->returnLeftNode(), ++u, code);//С помощью рекурсивного посещаем левое поддерево
        node->setString(node->returnStr()); // Если есть, убираем лишние нули в начале
        if (node->returnValue() != 0){
            code->emplace_back(node->returnValue(), node->returnStr());
        }
        u--;
    }
    returnValues(node->returnRightNode(), ++u, code); //С помощью рекурсии посещаем правое поддерево
}

// [Создание дерева]
// Функция с рекурсией, что разделяет массив на две части так, что разность сумм двух частей была минимальна.
// Рекурсия состоит в том, что необходимо разделить массив неизвестного размера, поэтому функция разделяет массив и подмассивы
// до того момента, как размер новой части массива не станет равна единице (всего один элемент в массиве).
//
// Для перехода на следующий часть в конце алгоритма вызывается эта же функция, но для левого подмассива вызывается прошлый
// указатель на начало массива-родителя, а для правого вызывается указатель на место разрыва массива-родителя.
// Для каждого случая так же вызывается размер массива.
void makeTree(const int* array, int arraySize, Node* node, bool v){
    if (v) cout << endl;

    // [Конец рекурсии]
    // Создание листьев, если у части массива остался 1 элемент
    if (arraySize == 1){
        if (v) cout << "[" << array[0] << "] " << node->returnStr() << endl; // Вывод для наглядности
        node->setValue(array[0]); // Ввод получившегося символа в лист дерева
        return;
    }

    // [Основной алгоритм]
    // Нахождение середины массива так, что, разделив его, сумма элементов из частей массива имеет минимальную разницу.
    // 1. Создание дополнительного массива для запоминания разности сумм при различных местах разрыва.
    // 2.1 Цикличный проход по всем элементам от 0 до size-1.
    // 2.2 Если предыдущая разность была меньше чем нынешняя, то возврат места разрыва на прошлый элемент.
    //
    // differenceHolder[] - массив для сохранения результатов подсчета разностей на каждом шаге.
    // index - значение, показывающее где необходим разрыв.
    int* differenceHolder = new int[arraySize];
    int index;

    // Шаг 2.1
    for (index = 0; index < arraySize; index++){

        int leftSum = 0; // Подсчет левой части
        int rightSum = 0; // Подсчет правой части

        // Заполнение left и right циклом, что проходит от 0 до size-1
        // Если мы на элементе левее нынешнего index, то увеличивается leftSum
        // Если мы на элементе правее нынешнего index, то увеличивается rightSum
        for (int j = 0; j < arraySize; j++){
            if (j < index) leftSum = leftSum + array[j];
            else rightSum =  rightSum + array[j];
        }
        differenceHolder[index] = abs(leftSum - rightSum); // Запоминание разницы

        // Шаг 2.2
        if ((index != 0) && (differenceHolder[index] > differenceHolder[index - 1])) {
            index--;
            break;
        }
    }

    // Если середина в конце, то справа будет массив без элементов, возврат на элемент назад
    if (index == arraySize) index--;

    // Удаление differenceHolder за ненадобностью далее
    delete[] differenceHolder;

    // Показательный вывод разделения
    if (v){
        for (int j = 0; j < arraySize; j++){
            cout << array[j] << " ";
            if (j == index - 1) cout << "| ";
        }
    }

    // [Рекурсия]
    // Для построения дерева с весами '0' или '1', при прохождении влево добавляется '0', при прохождении направо '1'
    // Для левого массива, копирование битов с прошлого нода.
    // Для правого массива, копирование битов с прошлого нода.
    // В качестве аргументов в createLeft(...) входит returnStr() для записывания '0' или '1' к прошлому значению битов.
    makeTree(array, index, node->createLeft( node->returnStr() ), v);
    makeTree(array + index, arraySize - index, node->createRight( node->returnStr() ), v);
}