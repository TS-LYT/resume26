#include <iostream>
#include <csignal>
#include <cstdlib>
#include <execinfo.h>
#include <unistd.h>

using namespace std;

static void segv_handler(int sig)
{
    void *trace[32];
    int size = backtrace(trace, 32);

    cerr << "\nCaught signal " << sig << " (Segmentation fault)\n";
    cerr << "Backtrace:\n";

    backtrace_symbols_fd(trace, size, STDERR_FILENO);
    _exit(128 + sig);
}

static void install_segv_handler()
{
    struct sigaction sa;

    sa.sa_handler = segv_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESETHAND;

    sigaction(SIGSEGV, &sa, nullptr);
}

class LinkedList
{    
private:
    struct Node
    {
        int data;
        Node *next;
        Node(int value) : data(value), next(nullptr) {}
    };
    Node *head;
public:
    LinkedList(): head(nullptr) {}
    ~LinkedList(){
        clear();
    }
    
    void clear(){
        Node *tmp = head;
        while (tmp != nullptr)
        {
            Node *next=tmp->next;
            delete tmp;
            tmp = next;
        }
        head=NULL;
    }
    
    void push(int value){
        Node *newNode=new Node(value);
        if(head == nullptr){
            head=newNode;
            return;
        }
        Node *tmp = head;
        while(tmp->next != nullptr){
            tmp=tmp->next;
        }
        tmp->next=newNode;
    }
    
    void reserver(){
        Node *sub_head = head;    
        Node *prev = nullptr;
        while(sub_head != nullptr){
           Node *next=sub_head->next; 
           sub_head->next=prev;
           prev=sub_head;
           sub_head=next;
        }
        head=prev;
    }

    void print(){
        Node *cur=head;
        while (cur != nullptr)
        {
            cout << cur->data;
            cout << "->";
            cur=cur->next;
        }
        cout << endl;
        
    }

};

int main(){
    install_segv_handler();

    LinkedList *link = new LinkedList();
    link->push(1);
    link->push(2);
    link->push(3);
    link->push(4);
    link->print();
    link->reserver();
    link->print();
    
}
