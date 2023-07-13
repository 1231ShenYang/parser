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
char TypeName[][20] = {"MOSFET", "BJT", "VSource", "ISource", "Inductor",
		"Resistor", "Diode", "Capacitor"};
enum CompType { MOSFET, BJT, VSource, ISource, Inductor, Resistor, Diode, Capacitor };
enum TranType { NMOS, PMOS, NPN, PNP };
enum Flag { UNSET, SET };
enum Boolean { FALSE, TRUE };
enum EquaType{ Nodal, Modified };
int mCount=0, bCount=0, vSCount=0, iSCount=0, rCount=0, iCount=0, dCount=0, cCount=0;
const int NameLength=80, BufLength=300, NA=-1;

class Component;
class ComponentHead;
class Node;
class NodeHead;
class Model;
class ModelHead;

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
class Component
{
public:
    Component( CompType typeIn, double valueIn, double tempIn, int con0In,
        int con1In, int con2In, int con3In, Model* modelIn, char* nameIn );
    ~Component();
    CompType getType();
    int getNum();
    Component* getNext();
    void connect( int conNum, Node* nodeIn );
    double getVal();
    int getcompNum ();
    void setNext( Component* nextIn );
    void setNum( int numIn );
    int getConVal( int conNum );
    Boolean isCon( int conNum );

    Node* getNode( int conNum );
    int getNodeNum( int conNum );
    char* getName();
private:
    Component* next;
    CompType type;
    Connectors con0, con1, con2, con3;
    int compNum;
    double value, temp;
    Model* model;
    char name[NameLength];
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
    int nodeNum, conCount;
    Connections* conList;
    int nameNum;
};
class NodeHead
{
public:
    NodeHead();
    ~NodeHead();
    Node* addNode();
    int getCount();
    Node* getNode(int nodeNum);
private:
    Node* nodeList;
    int nodeCount;
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
       outFile << "    类型:" << TypeName[root->comp->getType()] << "    连接端口:" << root->conNum <<
          "    名称:" << root->comp->getName() << "    value:" << root->comp->getVal() << endl;
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