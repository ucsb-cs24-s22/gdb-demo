#include <iostream>

using namespace std;

/*
 *  copied from lab01 starter code
 */
struct Node {
    int data;
    Node* next = nullptr;
};

/*
 *  copied from lab01 starter code
 */
struct LinkedList {
    Node* head = nullptr;
    Node* tail = nullptr;
};

/*
 *  copied from lab01 starter code
 */
string linkedListToString(LinkedList *list) {
    string result="";
    for(const Node *  p=list->head; p!=nullptr; p=p->next) {
        result += "[" + to_string(p->data) + "]->";
    }
    result += "null";
    return result;
}

/*
 *  copied from lab01 starter code
 */
void freeLinkedList(LinkedList * list) {
    Node *next;
    for (Node *p=list->head; p!=nullptr; p=next) {
        next = p->next;
        delete p;
    }
    delete list;
}

void addIntToEndOfList(LinkedList* list, int value) {
    Node* p = new Node{value, nullptr};
    list->tail->next = p;
    list->tail = p;
}

int main(int argc, char* argv[]) {
    // allocate space on the heap for a 
    // LinkedList struct
    LinkedList* list = new LinkedList;

    // print out the linked list
    cout << "BEFORE: " << linkedListToString(list) << endl;

    // add some nodes with user-inputted
    // data to the end of the linked list
    addIntToEndOfList(list, 1);
    addIntToEndOfList(list, 2);
    addIntToEndOfList(list, 3);

    // print out the linked list
    cout << "AFTER: " << linkedListToString(list) << endl;

    // free space on the heap
    freeLinkedList(list);
}