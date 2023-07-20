#pragma once
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <string.h>
using namespace std;

//  declarations:
const double K = 1.38E-23;
const double Q = 1.60E-19;
char CompTypeName[][20] = {"MOSFET", "BJT", "VSource", "ISource", "Inductor", "Resistor", "Diode", "Capacitor"}; //组件类型名
enum CompType { MOSFET, BJT, VSource, ISource, Inductor, Resistor, Diode, Capacitor }; //组件类型
char TranTypeName[][5] = {"NMOS", "PMOS", "NPN", "PNP"};
enum TranType { NMOS, PMOS, NPN, PNP }; //三极管类型
enum Flag { UNSET, SET }; //标志
enum Boolean { FALSE, TRUE };
enum EquaType{ Nodal, Modified };
int mCount=0, bCount=0, vSCount=0, iSCount=0, rCount=0, iCount=0, dCount=0, cCount=0;
const int NameLength=80, BufLength=300, NA=-1;

class Component; //组件
class ComponentHead; //组件头部
class Node; //节点
class NodeHead; //节点头部
class Model; //模型
class ModelHead; //模型头

double stripString( char *stringIn );
void printComponents( Component* compPtr);
void printNodes(Node* nodePtr, int compFlag);
char* strComponentType(Component* compPtr);

struct Connectors
{
    Flag flag;
    Node* node;
    int conNum;
};
struct Connections
{
    Connections* next;
    Component* comp; // ~> pointer to component
    int conNum;
};
class Node
{
public:
    Node(int num);
    ~Node();
    int getNum();
    void setNameNum( int numIn );
    int getNameNum();
    int getCount();
    Connections* getConList();
    void connect( int conNumIn, Component* compIn );
    Node* getNext();
    void Myprint(ofstream &outFile);
    void setNext( Node* nodeIn );
private:
    Node* next;
    int conCount; // conCount:连接的器件数
    Connections* conList;
    int nodeNum, nameNum; // nameNum:节点号
};
class NodeHead
{
public:
    NodeHead();
    ~NodeHead();
    Node* addNode();
    int getCount();
    Node* getNode(int nodeNum);
    Node* getNodeByNameNum(int namenum);
private:
    Node* nodeList;
    int nodeCount;
};
class Component
{
public:
    Component( CompType typeIn, double valueIn, double tempIn, int con0In,
        int con1In, int con2In, int con3In, Model* modelIn, char* nameIn );
    ~Component();
    Component* getNext();
    CompType getType();
    Node* getNode( int conNum );
    Boolean isCon( int conNum );
    void connect( int conNum, Node* nodeIn );
    double getVal();
    int getcompNum ();
    int getConVal( int conNum );
    int getNum();
    int getNodeNum( int conNum );
    char* getName();
    void setNext( Component* nextIn );
    void setNum( int numIn );

    bool printFunc(char *str, int conNum, int nodeNameNum, NodeHead *head);
private:
    Component* next;
    CompType type;
    Connectors con0, con1, con2, con3;
    int compNum;
    double value, temp;
    Model* model;
    char name[NameLength];
};
class CompHead
{
public:
    CompHead();
    ~CompHead();
    void addComp( Component* component );
    int getCount( CompType type );
    Component* getComp( int compNum );
private:
    Component* compList;
    int sCount, rCount, tCount, dCount, cCount;
};
class Model
{
public:
    Model( char* nameIn, TranType typeIn, double isIn, double bfIn, double brIn, double tempIn );
    char* getName();
    TranType getType();
    double getIs();
    double getBf();
    double getBr();
    double getTemp();
    void setNext( Model* nextIn );
    Model* getNext();
private:
    char name[NameLength];
    double is, bf, br, temp;
    Model* next;
    TranType type;
};
class ModelHead
{
public:
    ModelHead();
    void addModel( Model* modelIn );
    Model* getModel ( char* nameIn );
private:
    Model* modelList;
};

Component::Component( CompType typeIn, double valueIn=NA, double tempIn=NA,
		      int con0In=NA, int con1In=NA, int con2In=NA, int con3In=NA,
		      Model* modelIn=NULL, char *nameIn = NULL ){

    type = typeIn;
    con0.conNum = con0In;
    con1.conNum = con1In;
    con2.conNum = con2In;
    con3.conNum = con3In;
    con0.flag = UNSET;
    con1.flag = UNSET;
    con2.flag = UNSET;
    con3.flag = UNSET;
    value = valueIn;
    temp = tempIn;
    next = NULL;
    model = modelIn;
    strcpy( name, nameIn );
}
Component::~Component(){};
void Component::connect( int conNum, Node* nodeIn )
{
    if( conNum == 0 )
    {
        con0.node = nodeIn;
        con0.flag = SET;
    }
    if( conNum == 1 )
    {
        con1.node = nodeIn;
        con1.flag = SET;
    }
    if( conNum == 2 )
    {
        con2.node = nodeIn;
        con2.flag = SET;
    }
    if( conNum == 3 )
    {
        con3.node = nodeIn;
        con3.flag = SET;
    }
}
CompType Component::getType(){ return type; }
int Component::getNum(){ return compNum; }
Component* Component::getNext(){ return next; }
double Component::getVal(){ return value; }
int Component::getcompNum() { return compNum; }
void Component::setNext( Component* nextIn ){ next = nextIn; }
void Component::setNum( int numIn ){ compNum = numIn; }
int Component::getConVal( int conNum )
{
    int rtVal;
    if( conNum == 0 ) rtVal = con0.conNum;
    else if( conNum == 1 ) rtVal = con1.conNum;
    else if( conNum == 2 ) rtVal = con2.conNum;
    else if( conNum == 3 ) rtVal = con3.conNum;
    return rtVal;
}
Boolean Component::isCon( int conNum )
{
    Boolean rtVal;
    if( conNum == 0 ) rtVal = ( con0.flag == SET )? TRUE: FALSE;
    else if( conNum == 1 ) rtVal = ( con1.flag == SET )? TRUE: FALSE;
    else if( conNum == 2 ) rtVal = ( con2.flag == SET )? TRUE: FALSE;
    else if( conNum == 3 ) rtVal = ( con3.flag == SET )? TRUE: FALSE;
    return rtVal;
}
Node* Component::getNode( int conNum )
{
    switch(conNum)
    {
        case 0:
            return con0.node;
        case 1:
            return con1.node;
        case 2:
            return con2.node;
        case 3:
            return con3.node;
    }
    return NULL;
}
int Component::getNodeNum( int conNum )
{
    switch(conNum)
    {
        case 0:
            return con0.node->getNum();
        case 1:
            return con1.node->getNum();
        case 2:
            return con2.node->getNum();
        case 3:
            return con3.node->getNum();
    }
    return -1;
}
char* Component::getName(){ return name; }
bool Component::printFunc(char *str, int conNum, int nodeNameNum, NodeHead *head)
{
    //{ MOSFET, BJT, VSource, ISource, Inductor, Resistor, Diode, Capacitor };
    switch(type)
    {
        case BJT:
            if(conNum == 0)
            {
                sprintf(str, "Ic%d", compNum);
                return true;
            }
            else if(conNum == 1)
            {
                sprintf(str, " - Ic%d - Ie%d", compNum, compNum);
                return false;
            }
            else if(conNum == 2)
            {
                sprintf(str, "Ie%d", compNum);
                return true;
            }
        case Resistor:
            if(con0.node->getNameNum() == 0 || con1.node->getNameNum() == 0) sprintf(str, "x%d/%s", nodeNameNum, name);
            else if(con0.node->getNameNum() == nodeNameNum) sprintf(str, "(x%d - x%d)/%s", nodeNameNum, con1.node->getNameNum(), name);
            else sprintf(str, "(x%d - x%d)/%s", nodeNameNum, con0.node->getNameNum(), name);
            return true;
        case VSource:
            if(con0.node->getNameNum() == 0 || con1.node->getNameNum() == 0) sprintf(str, "x%d - %s", nodeNameNum, name);
            else if(con0.node->getNameNum() == nodeNameNum) sprintf(str, "x%d - x%d - %s", nodeNameNum, con1.node->getNameNum(),name);
            else
            {
                sprintf(str, " - x%d", head->getCount() - 1 + compNum);
                return false;
            }
            return true;
    }
    return false;
}

Node::Node( int Num )
{
    next = NULL;
    nodeNum = Num;
    conCount = 0;
    conList = NULL;
    nameNum = 0;
}
Node::~Node(){};
int Node::getNum(){ return nodeNum; }
void Node::setNameNum( int numIn ){ nameNum = numIn; }
int Node::getNameNum(){ return nameNum; }
int Node::getCount(){ return conCount; }
Connections * Node::getConList() { return conList; }
void Node::connect( int conNumIn, Component* compIn )
{
    Connections* conPtr;
    conCount++;
    if( conList == NULL )
    {
        conList = new Connections;
        conList->next = NULL;
        conList->conNum = conNumIn;
        conList->comp = compIn;
    }
    else
    {
        conPtr = conList;
        while ( conPtr->next != NULL ) conPtr = conPtr->next;
        conPtr->next = new Connections;
        conPtr = conPtr->next;
        conPtr->next = NULL;
        conPtr->conNum = conNumIn;
        conPtr->comp = compIn;
    }
}
Node* Node::getNext(){ return next; }
void Node::setNext( Node* nodeIn ){ next = nodeIn; }
void Node::Myprint(ofstream &outFile)
{
    outFile << "节点" << nameNum << "        所连器件数为:" << conCount << endl;
    Connections* root = conList;
    while(root != NULL)
    {
       outFile << "    类型:" << CompTypeName[root->comp->getType()] << "    连接端口:" << root->conNum <<
          "    名称:" << root->comp->getName() << "    编号:" << root->comp->getcompNum()<<
          "    value:" << root->comp->getVal() << endl;
       root = root->next;
    }
}

NodeHead::NodeHead()
{
    nodeList = NULL;
    nodeCount = 0;
}
NodeHead::~NodeHead(){};
Node* NodeHead::addNode()
{
    Node* nodePtr;
    nodeCount++;
    if( nodeList == NULL )
    {
        nodeList = new Node(nodeCount);
        return nodeList;
	}
	else
    {
        nodePtr = nodeList;
        while( nodePtr->getNext() != NULL ) nodePtr = nodePtr->getNext();
        nodePtr->setNext( new Node(nodeCount) );
        return nodePtr->getNext();
	}
}
int NodeHead::getCount(){ return nodeCount; }
Node* NodeHead::getNode( int nodeNum )
{
    Node* nodePtr;
    nodePtr = nodeList;
    for( int a=0; a < nodeNum; a++ ) nodePtr = nodePtr->getNext();
    return nodePtr;
}
Node* NodeHead::getNodeByNameNum(int namenum)
{
    Node* nodePtr = NULL;
    nodePtr = nodeList;
    while(nodePtr != NULL)
    {
        if(nodePtr->getNameNum() == namenum) break;
        nodePtr = nodePtr->getNext();
    }
    return nodePtr;
}

CompHead::CompHead()
{
    compList = NULL;
    mCount = 0;
    bCount = 0;
    iCount = 0;
    rCount = 0;
    dCount = 0;
    cCount = 0;
    vSCount = 0;
    iSCount = 0;
}
CompHead::~CompHead(){};
void CompHead::addComp( Component* component )
{
	Component* compPtr;
    switch ( component->getType() )
    {
        case ISource:
            iSCount++;
            if( compList == NULL )
            {
                compList = component;
                component->setNum( iSCount );
            }
            else
            {
                compPtr = compList;
                while( compPtr->getNext() != NULL ) compPtr = compPtr->getNext();
                compPtr->setNext( component );
                compPtr = compPtr->getNext();
                compPtr->setNum( iSCount );
            }
            break;
        case VSource:
            vSCount++;
            if( compList == NULL )
            {
                compList = component;
                component->setNum( vSCount );
            }
            else
            {
                compPtr = compList;
                while( compPtr->getNext() != NULL ) compPtr = compPtr->getNext();
                compPtr->setNext( component );
                compPtr = compPtr->getNext();
                compPtr->setNum( vSCount );
            }
            break;
        case Resistor:
            rCount++;
            if( compList == NULL )
            {
                compList = component;
                component->setNum( rCount );
            }
            else
            {
                compPtr = compList;
                while( compPtr->getNext() != NULL ) compPtr = compPtr->getNext();
                compPtr->setNext( component );
                compPtr = compPtr->getNext();
                compPtr->setNum( rCount );
            }
            break;
        case MOSFET:
            mCount++;
            if( compList == NULL )
            {
                compList = component;
                component->setNum( mCount );
            }
            else
            {
                compPtr = compList;
                while( compPtr->getNext() != NULL ) compPtr = compPtr->getNext();
                compPtr->setNext( component );
                compPtr = compPtr->getNext();
                compPtr->setNum( mCount );
            }
            break;
        case BJT:
            bCount++;
            if( compList == NULL )
            {
                compList = component;
                component->setNum( bCount );
            }
            else
            {
                compPtr = compList;
                while( compPtr->getNext() != NULL ) compPtr = compPtr->getNext();
                compPtr->setNext( component );
                compPtr = compPtr->getNext();
                compPtr->setNum( bCount );
            }
            break;
        case Diode:
            dCount++;
            if( compList == NULL )
            {
                compList = component;
                component->setNum( dCount );
            }
            else
            {
                compPtr = compList;
                while( compPtr->getNext() != NULL ) compPtr = compPtr->getNext();
                compPtr->setNext( component );
                compPtr = compPtr->getNext();
                compPtr->setNum( dCount );
            }
            break;
        case Capacitor:
            cCount++;
            if( compList == NULL )
            {
                compList = component;
                component->setNum( cCount );
            }
            else
            {
                compPtr = compList;
                while( compPtr->getNext() != NULL ) compPtr = compPtr->getNext();
                compPtr->setNext( component );
                compPtr = compPtr->getNext();
                compPtr->setNum( cCount );
            }
            break;
        case Inductor:
            iCount++;
            if( compList == NULL )
            {
                compList = component;
                component->setNum( iCount );
            }
            else
            {
                compPtr = compList;
                while( compPtr->getNext() != NULL ) compPtr = compPtr->getNext();
                compPtr->setNext( component );
                compPtr = compPtr->getNext();
                compPtr->setNum( iCount );
            }
            break;
    }
}
int CompHead::getCount( CompType type)
{
    switch (type)
    {
        case ISource:
            return iSCount;
        case VSource:
            return vSCount;
        case Resistor:
            return rCount;
        case Diode:
            return dCount;
        case MOSFET:
            return mCount;
        case BJT:
            return bCount;
        case Capacitor:
            return cCount;
        case Inductor:
            return iCount;
    }
    return -1;
}
Component* CompHead::getComp( int compNum )
{
    Component* compPtr;
    compPtr = compList;
    for( int a=0; a<compNum; a++ ) compPtr = compPtr->getNext();
    return compPtr;
}

Model::Model( char* nameIn, TranType typeIn, double isIn, double bfIn,
	      double brIn, double tempIn )
{
    strcpy( name, nameIn );
    type = typeIn;
    is = isIn;
    bf = bfIn;
    br = brIn;
    temp = tempIn;
    next = NULL;
}
TranType Model::getType(){ return type; }
char* Model::getName(){ return name; }
double Model::getIs(){ return is; }
double Model::getBf(){ return bf; }
double Model::getBr(){ return br; }
double Model::getTemp(){ return temp; }
void Model::setNext( Model* nextIn ){ next = nextIn; }
Model* Model::getNext(){ return next; }
ModelHead::ModelHead(){ modelList = NULL; }
void ModelHead::addModel( Model* modelIn )
{
    Model* modelPtr;
    if( modelList == NULL ) modelList = modelIn;
    else
    {
        modelPtr = modelList;
        while( modelPtr->getNext() != NULL ) modelPtr = modelPtr->getNext();
        modelPtr->setNext( modelIn );
    }
}
Model* ModelHead::getModel ( char* nameIn )
{
    Model* modelPtr = modelList;
    while( strcmp( modelPtr->getName(), nameIn ) ) modelPtr = modelPtr->getNext();
    return modelPtr;
}

double stripString( char *stringIn ){
    char buf[BufLength], buf2[BufLength];
    int a, b;
    strcpy( buf, stringIn );
    for( a=0; buf[a] != '='; a++ ){};
    a++;
    for( b=0; buf[a] != '\0'; b++, a++ ) buf2[b] = buf[a];
    buf2[b] = '\0';
    return atof( buf2 );
};
//Print the linked list of components to check
void printComponents( Component* compPtr){
    char compTypeName[6];
    cout << "Components: " << endl;
    while(compPtr != NULL)
    {
        strcpy(compTypeName, strComponentType(compPtr));
        cout << "->" << compTypeName << compPtr->getcompNum();
        compPtr = compPtr->getNext();
    }
    cout << endl;
    return;
}
void printNodes(Node* nodePtr, int compFlag)
{
    Connections* conPtr;
    cout << "Nodes: " << endl;
    while(nodePtr != NULL)
    {
        if(compFlag == 0) cout << "-> " << nodePtr->getNameNum();//It is printed just the names of the nodes
        else if (compFlag == 1)
        { //It is printed the nodes and the connections
            cout << "-> " << nodePtr->getNameNum() << " {";
            conPtr = nodePtr->getConList();
            while(conPtr->next != NULL)
            {
                cout << strComponentType(conPtr->comp) << conPtr->comp->getcompNum() << ", ";
                conPtr = conPtr->next;
            }
            cout << strComponentType(conPtr->comp) << conPtr->comp->getcompNum() << '}' << endl;
        }
        else
        {
            cout << "Invalid value for compFlag. (0) to print just nodes, (1) to print nodes and connections!";
            exit(1);
        }
        nodePtr = nodePtr->getNext();
    }
    return;
}
char* strComponentType(Component* compPtr)
{
    char* compTypeName = new char[6];
    switch(compPtr->getType())
    {
        case VSource: strcpy(compTypeName, "V"); break;
        case Resistor: strcpy(compTypeName, "R"); break;
        case BJT: strcpy(compTypeName, "T"); break;
        case MOSFET: strcpy(compTypeName, "M"); break;
        case ISource: strcpy(compTypeName, "I"); break;
        case Inductor: strcpy(compTypeName, "ind"); break;
        case Diode: strcpy(compTypeName, "Diode"); break;
        case Capacitor: strcpy(compTypeName, "Cap"); break;
    }
    return compTypeName;
}

void ModifiedNodalFunc(ofstream &outFile, NodeHead *head1, CompHead *head2)
{
    Component *comp = head2->getComp(0);
    Node *p;
    Connections* root, *temp;
    char str[1000];
    bool flag;
    while(comp != NULL)
    {
        if(comp->getType() == VSource)
        {
            outFile << "F(" << head1->getCount() - 1 + comp->getcompNum() << ") = x" <<
                head1->getCount() - 1 + comp->getcompNum() << " ";
            p = comp->getNode(0);
            root = p->getConList();
            for(int a = 0; a < p->getCount(); ++a)
            {
                if(root->comp->getType() != VSource)
                {
                    if(root->comp->printFunc(str, root->conNum, p->getNameNum(), head1)) outFile << " + ";
                    outFile << str;
                }
                root = root->next;
                strcpy(str, "");
            }
            outFile << endl;
        }
        comp = comp->getNext();
    }
}
void NodalFunc(ofstream &outFile, NodeHead *head1, CompHead *head2)
{
    Node *p;
    Connections* root;
    char str[1000];
    bool flag;
    outFile << endl << "Functions:" << endl;
    for(int a = 1; a < head1->getCount(); ++a)
    {
        flag = false;
        p = head1->getNodeByNameNum(a);
        root = p->getConList();
        outFile << "F(" << a << ") = ";
        for(int b = 0; b < p->getCount(); ++b)
        {
            if(root->comp->getType() == VSource && root->comp->getNode(0)->getNameNum() == p->getNameNum())
            {
                root->comp->printFunc(str, root->conNum, p->getNameNum(), head1);
                flag = true;
                outFile << str << endl;
                break;
            }
            root = root->next;
        }
        if(flag) continue;
        root = p->getConList();
        for(int b = 0; b < p->getCount(); ++b)
        {
            if(root->comp->printFunc(str, root->conNum, p->getNameNum(), head1) && b != 0) outFile << " + ";
            outFile << str;
            root = root->next;
            strcpy(str, "");
        }
        outFile << endl;
    }
    ModifiedNodalFunc(outFile, head1, head2);
}