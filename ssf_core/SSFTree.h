#include <iostream>
using namespace std;

template<class T>
class SSFTree
{
struct Node
    {
        T data;
        Node* left;
        Node* right;

        Node(T dataNew){
            data = dataNew;
            left = NULL;
            right = NULL;
        }
    };
private:
    Node* Tree;

        void Insert(T newData, Node* &no){ // insere um elemento na árvore
            if(no == NULL){
                no = new Node(newData);
                return;
            }

            if(newData < no->data)
                Insert(newData, no->left);//se menor, caminha para a esquerda
            else
                Insert(newData, no->right);//caso contrário caminha para a direita
        }

        void Print(Node* &no){ // utiliza caminhamento central
            if(no != NULL){
                Print(no->left);
                cout << no->data << " ";
                Print(no->right);
            }
        }
        bool Search(T newData, Node* &no){// verifica se um elemento está presente na árvore ou não

             if(no == NULL){//caso chegue ao fim da arvore
                return(false);
            }

            if(newData == no->data){//condição usada apenas para imprimir se o registro for encontrado
                return(true);
            }

            if((newData) < (no->data)){
                return (Search(newData,no->left));
            }
            if((newData) > (no->data)){
                return (Search(newData,no->right));
            }
            else{
                newData = no->data;
            }
        }

        void Remove(T newData, Node* &no){//retira um elemento da arvore

        Node *aux;
            if(no == NULL){
                cout<<"\nError: Element was not found."<<endl;
                return;
            }
            if(newData < no->data){
                Remove(newData,no->left);
                return;
            }
            if(newData > no->data){
                Remove(newData,no->right);
                return;
            }
            if(no->right == NULL){
                aux = no;
                no = no->left;
                delete aux;
                return;
            }
            if(no->left != NULL){
                Predecessor(no,no->left);
                return;
            }
            aux = no;
            no = no->right;
            delete aux;

        }
        void Predecessor(Node* q, Node* &r){//procedimento auxiliar da função Remove
        if(r->right != NULL){
            Predecessor(q,r->right);
            return;
            }
            q->data = r->data;
            q = r;
            r = r->left;
            delete q;
        }

        void RemoveAll(Node* &no){//retira todos os elementos da arvore

            if(no != NULL){
            RemoveAll(no->left);
            RemoveAll(no->right);
            delete no;
            }

        }

    public:
        SSFTree(){//construtor para inicializar a árvore automaticamente, quando algum objeto for criado
            Tree = NULL;
        }
        ~SSFTree(){//destrutor

            RemoveAll(Tree);//assim que o objeto é deletado a função é chamada para destruir a árvore inteira
         }

        void AddItem(T newData){
            Insert(newData, Tree);
        }

        void Print(){
            Print(Tree);
        }
        bool Search(T newData){
            return(Search(newData, Tree));
        }

        void Remove(T newData){
            Remove(newData, Tree);
        }

        void RemoveAll(){
            RemoveAll(Tree);
            Tree=NULL;
        }
    };
