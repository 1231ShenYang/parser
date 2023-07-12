//  PARSER.H
//  Header for Parser program.

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
enum CompType { MOSFET, BJT, VSource, ISource, Inductor,
		  Resistor, Diode, Capacitor };
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

struct Connectors{
	Flag flag;
	Node* node;
	int conNum;
};
struct Connections{
	Connections* next;
	Component* comp; // ~> pointer to component
	int conNum;
};
class Component{
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
  void print( int nodeNum, ofstream &outFile, int datum, int lastnode );
  void specialPrint( ofstream &outFile, int datum );
  void specialPrintJac( ofstream &outFile, int datum, Node* wrt/**/, int lastnode, EquaType eqType, Component* compPtr2, int* specPrintJacMNA /**/  );
  void printVal( ofstream &outFile );
  void Myprint( ofstream &outFile );
  void printJac( int nodeNum, ofstream &outFile, int datum, int wrt, bool MNAflag);
  /*~> function to print the super node equation (for Nodal equation) */
  void printSuperNode(ofstream &outFile, int datum, int lastnode);
  /* */
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
class Node{
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
  void printNodal( ofstream &outFile, int datum, int lastnode );
  void printMNA( ofstream &outFile, int datum, int lastnode );
  void printJac( ofstream &outFile, int datum, Node* wrt, int lastnode,
EquaType eqType);
  void printJacMNA( ofstream &outFile, int datum, Node * wrt, int lastnode);
  void setNext( Node* nodeIn );
  /*~>function to print the super node equation (for Nodal equation)*/
  void printSuperNodal(ofstream &outFile, int datum, int lastnode);
  /* */
private:
  Node* next;
  int nodeNum, conCount;
  Connections* conList;
  int nameNum;
};
class NodeHead {
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
class CompHead {
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
class Model {
public:
  Model( char* nameIn, TranType typeIn, double isIn, double bfIn,
	 double brIn, double tempIn );
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
class ModelHead{
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
void Component::connect( int conNum, Node* nodeIn ){
	if( conNum == 0 ){
		con0.node = nodeIn;
		con0.flag = SET;
		}
	if( conNum == 1 ){
		con1.node = nodeIn;
		con1.flag = SET;
		}
	if( conNum == 2 ){
		con2.node = nodeIn;
		con2.flag = SET;
		}
	if( conNum == 3 ){
		con3.node = nodeIn;
		con3.flag = SET;
		}
	}
CompType Component::getType(){
	return type;
	}
int Component::getNum(){
	return compNum;
	}
Component* Component::getNext(){
	return next;
	}
double Component::getVal(){
	return value;
	}
int Component::getcompNum() {
        return compNum;
	}
void Component::setNext( Component* nextIn ){
	next = nextIn;
	}
void Component::setNum( int numIn ){
	compNum = numIn;
	}
int Component::getConVal( int conNum ){
	int rtVal;
	if( conNum == 0 )
		rtVal = con0.conNum;
	if( conNum == 1 )
		rtVal = con1.conNum;
	if( conNum == 2 )
		rtVal = con2.conNum;
	if( conNum == 3 )
		rtVal = con3.conNum;
	return rtVal;
	}
Boolean Component::isCon( int conNum ){
	Boolean rtVal;
	if( conNum == 0 )
		rtVal = ( con0.flag == SET )? TRUE: FALSE;
	if( conNum == 1 )
		rtVal = ( con1.flag == SET )? TRUE: FALSE;
	if( conNum == 2 )
		rtVal = ( con2.flag == SET )? TRUE: FALSE;
	if( conNum == 3 )
		rtVal = ( con3.flag == SET )? TRUE: FALSE;
	return rtVal;
	}
void Component::print( int nodeNum, ofstream &outFile, int datum, int
lastnode ){
  switch( type ){
  case MOSFET:
    outFile << name
	    << " connected: Drain->Node" << getNodeNum(0)
	      << " Source->Node" << getNodeNum(1)
		<< " Gate->Node" << getNodeNum(2)
		  << " Bulk->Node" << getNodeNum(3)<< " ." << endl;


  case BJT:
    if( (con0.node->getNum() == nodeNum) && (model->getType() == NPN) ){
      outFile << " (" << name << "IS "<< ")*(exp(-"
	      << name << "N*(";
      if( con2.node->getNameNum() != datum )
	outFile << "X(" << con2.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << "-X(" << con1.node->getNameNum() << ')';
      outFile << ")) -1)";
      outFile << " + ";
      outFile << "(-" << name << "IS "
	      << "/" << name << "BR "
	      << ")*(1+" << name << "BR "
	      << ")*(exp(-"
	      << name << "N*(";
      if( con0.node->getNameNum() != datum )
	outFile << "X(" << con0.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << "-X(" << con1.node->getNameNum() << ')';
      outFile << ")) -1) ";
    }

    if( (con2.node->getNum() == nodeNum) && (model->getType() == NPN) ){
      outFile << " (-" << name << "IS "
	      << "/" << name << "BF "
	      << ")*(1+" << name << "BF "
	      << ")*(exp(-"
	      << name << "N*(";
      if( con2.node->getNameNum() != datum )
	outFile << "X(" << con2.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << "-X(" << con1.node->getNameNum() << ')';
      outFile << ")) -1)";
      outFile << " - ";
      outFile << "(- " << name << "IS "<< ")*(exp(-"
	      << name << "N*(";
      if( con0.node->getNameNum() != datum )
	outFile << "X(" << con0.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << "-X(" << con1.node->getNameNum() << ')';
      outFile << ")) -1) ";
    }

    if( (con1.node->getNum() == nodeNum) && (model->getType() == NPN) ){
      outFile << " (-" << name << "IS "<< ")*(exp(-"
	      << name << "N*(";
      if( con2.node->getNameNum() != datum )
	outFile << "X(" << con2.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << "-X(" << con1.node->getNameNum() << ')';
      outFile << ")) -1)";
      outFile << " + ";
      outFile << "(" << name << "IS "
	      << "/" << name << "BR "
	      << ")*(1+" << name << "BR "
	      << ")*(exp(-"
	      << name << "N*(";
      if( con0.node->getNameNum() != datum )
	outFile << "X(" << con0.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << "-X(" << con1.node->getNameNum() << ')';
      outFile << ")) -1)";
      outFile << " + ";
      outFile << "(" << name << "IS "
	      << "/" << name << "BF "
	      << ")*(1+" << name << "BF "
	      << ")*(exp(-"
	      << name << "N*(";
      if( con2.node->getNameNum() != datum )
	outFile << "X(" << con2.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << "-X(" << con1.node->getNameNum() << ')';
      outFile << ")) -1)";
      outFile << " - ";
      outFile << "( " << name << "IS "<< ")*(exp(-"
	      << name << "N*(";
      if( con0.node->getNameNum() != datum )
	outFile << "X(" << con0.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << "-X(" << con1.node->getNameNum() << ')';
      outFile << ")) -1) ";
    }

    if( (con0.node->getNum() == nodeNum) && (model->getType() == PNP) ){
      outFile << " (-" << name << "IS "<< ")*(exp("
	      << name << "N*(";
      if( con2.node->getNameNum() != datum )
	outFile << "X(" << con2.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << "-X(" << con1.node->getNameNum() << ')';
      outFile << ")) -1)";
      outFile << " + ";
      outFile << "(" << name << "IS "
	      << "/" << name << "BR "
	      << ")*(1+" << name << "BR "
	      << ")*(exp("
	      << name << "N*(";
      if( con0.node->getNameNum() != datum )
	outFile << "X(" << con0.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << "-X(" << con1.node->getNameNum() << ')';
      outFile << ")) -1) ";
    }

    if( (con2.node->getNum() == nodeNum) && (model->getType() == PNP) ){
      outFile << " (" << name << "IS "
	      << "/" << name << "BF "
	      << ")*(1+" << name << "BF "
	      << ")*(exp("
	      << name << "N*(";
      if( con2.node->getNameNum() != datum )
	outFile << "X(" << con2.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << "-X(" << con1.node->getNameNum() << ')';
      outFile << ")) -1)";
      outFile << " - ";
      outFile << "( " << name << "IS "<< ")*(exp("
	      << name << "N*(";
      if( con0.node->getNameNum() != datum )
	outFile << "X(" << con0.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << "-X(" << con1.node->getNameNum() << ')';
      outFile << ")) -1) ";
    }

    if( (con1.node->getNum() == nodeNum) && (model->getType() == PNP) ){
      outFile << " (" << name << "IS "<< ")*(exp("
	      << name << "N*(";
      if( con2.node->getNameNum() != datum )
	outFile << "X(" << con2.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << "-X(" << con1.node->getNameNum() << ')';
      outFile << ")) -1)";
      outFile << " + ";
      outFile << "(-" << name << "IS "
	      << "/" << name << "BR "
	      << ")*(1+" << name << "BR "
	      << ")*(exp("
	      << name << "N*(";
      if( con0.node->getNameNum() != datum )
	outFile << "X(" << con0.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << "-X(" << con1.node->getNameNum() << ')';
      outFile << ")) -1)";
      outFile << " + ";
      outFile << "(-" << name << "IS "
	      << "/" << name << "BF "
	      << ")*(1+" << name << "BF "
	      << ")*(exp("
	      << name << "N*(";
      if( con2.node->getNameNum() != datum )
	outFile << "X(" << con2.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << "-X(" << con1.node->getNameNum() << ')';
      outFile << ")) -1)";
      outFile << " - ";
      outFile << "(-" << name << "IS "<< ")*(exp("
	      << name << "N*(";
      if( con0.node->getNameNum() != datum )
	outFile << "X(" << con0.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << "-X(" << con1.node->getNameNum() << ')';
      outFile << ")) -1) ";
    }
    break;

  case VSource:
    if( con0.node->getNum() == nodeNum )
      outFile << " X(" << lastnode + compNum << ")";
    else if( con1.node->getNum() == nodeNum )
      outFile << " (-X(" << lastnode + compNum << ") )";
    break;

  case ISource:
    if( con0.node->getNum() == nodeNum )
      outFile << " (" << value << ") ";
    else if( con1.node->getNum() == nodeNum )
      outFile << " (-" << value << ") ";
    break;

  case Diode:
    if( con0.node->getNum() == nodeNum ){
      outFile << " (" << name << "IS "<< ")*(exp("
	      << name << "N*(";
      if( con0.node->getNameNum() != datum )
	outFile << "X(" << con0.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << "-X(" << con1.node->getNameNum() << ')';
      outFile << ")) -1) ";
    }
    else if( con1.node->getNum() == nodeNum ){
      outFile << " (-" << name << "IS "<< ")*(exp("
	      << name << "N*(";
      if( con0.node->getNameNum() != datum )
	outFile << "X(" << con0.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << "-X(" << con1.node->getNameNum() << ')';
      outFile << ")) -1) ";
    }
    break;

  case Resistor:
    if( con0.node->getNum() == nodeNum ){
      outFile << " (";
      if( con0.node->getNameNum() != datum )
	outFile << "X(" << con0.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << "-X(" << con1.node->getNameNum() << ')';
      outFile << ")/" << name << " ";
    }
    if( con1.node->getNum() == nodeNum ){
      outFile << " (";
      if( con1.node->getNameNum() != datum )
	outFile << "X(" << con1.node->getNameNum() << ')';
      if( con0.node->getNameNum() != datum )
	outFile << "-X(" << con0.node->getNameNum() << ')';
      outFile << ")/" << name << " ";
    }
    break;

  case Capacitor:
    outFile << " 0 ";
    break;

  case Inductor:
    if( con0.node->getNum() == nodeNum )
      outFile << " Il" << compNum << " ";
    else if( con1.node->getNum() == nodeNum )

    break;
  };
  return;
}
void Component::specialPrint( ofstream &outFile, int datum ){
  int eqnum;
  if( type == VSource ){
    if (con0.node->getNameNum() == datum) eqnum = con1.node->getNameNum();
    else eqnum = con0.node->getNameNum();
    outFile << endl << "F(" << eqnum << ") = ";
    outFile << " (";
    if( con0.node->getNameNum() != datum )
      outFile << "X(" << con0.node->getNameNum() << ')';
    if( con1.node->getNameNum() != datum )
      outFile << "-X(" << con1.node->getNameNum() << ')';
    outFile << ") -" << value << ';' << endl;
  }
  else if( type == Inductor ){
    outFile << endl << "F(I" << compNum << ") = ";
    outFile << " (";
    if( con0.node->getNameNum() != datum )
      outFile << "X(" << con0.node->getNameNum() << ')';
    if( con1.node->getNameNum() != datum )
      outFile << "-X(" << con1.node->getNameNum() << ')';
    outFile << ") " << ';' << endl;
  }
}
/// ~> POSSIBLE DEBUG: insert a condition with relation to the type of equation: MNA. i.e. if(typeEq = MNA) ...
/// ~> MODIFYING THE PROTOTYPE OF THE FUNCTION: including 'int lastnode' and 'EquaType eqType', Component* compPtr2
void Component::specialPrintJac( ofstream &outFile, int datum, Node* wrt /**/, int lastnode, EquaType eqType, Component* compPtr2, int* specPrintJacMNA /**/ ){
  int eqnum;
  if( type == VSource ){
    if (con0.node->getNameNum() == datum)  eqnum = con1.node->getNameNum();
    else  eqnum = con0.node->getNameNum();
    outFile << endl << "JAC(" << eqnum << ", " << wrt->getNameNum() << ") = ";
    if( con0.node->getNameNum() == wrt->getNameNum() )
      outFile << "1";
    else if( con1.node->getNameNum() == wrt->getNameNum() )
      outFile << "-1";
    else
      outFile << "0";
    outFile << ';' << endl;
    //==================================================
    //~> Printing the jacobians of each voltage source with respect to each unknown current (MNEqs)
    if ((eqType == Modified) && (*specPrintJacMNA == 0)) {
        while ( compPtr2 != NULL ) {
            if( compPtr2->getType() == VSource ) {
                outFile << endl << "JAC(" << eqnum << ", " ;
                outFile << lastnode + compPtr2->getcompNum() << ") = 0; ";
                outFile << endl;
                }
            compPtr2 = compPtr2->getNext();
        }
        (*specPrintJacMNA) = 1;

  }
   //=====================================================

  }
  else if( type == Inductor ){
    outFile << endl << "JAC(I" << compNum << ", " << wrt->getNameNum() << ") = ";
    if( con0.node->getNameNum() == wrt->getNameNum() )
      outFile << "1";
    else if( con1.node->getNameNum() == wrt->getNameNum() )
      outFile << "-1";
    else
      outFile << "0";
    outFile << ';' << endl;
  }
}
void Component::printVal( ofstream &outFile ){
  switch ( type ){
  case Resistor:
    outFile << name << " = " << value << ';' << endl;
    break;
  case Capacitor:
    outFile << name << " = " << value << ';' << endl;
    break;
  case BJT:
    outFile << name << "IS = " << model->getIs() << ';' << endl
      << name << "BF = " << model->getBf() << ';' << endl
	<< name << "BR = " << model->getBr() << ';' << endl
	<< name << "N = " << ( Q / (K * model->getTemp()) ) << ';' << endl;
    break;
  case Diode:
    outFile << name << "IS = " << value << ';' << endl
	    << name << "N = " << (Q / (K * temp) ) << ';' << endl;
    break;
  };
}
void Component::Myprint( ofstream &outFile ){
  switch ( type ){
  case Resistor:
    outFile << "节点" << compNum << " " << name << " = " << value << ' ' << TypeName[type] << ';' << endl;
    break;
  case Capacitor:
    outFile << "节点" << compNum << " " << name << " = " << value << ' ' << TypeName[type] << ';' << endl;
    break;
  case BJT:
    outFile << name << "IS = " << model->getIs() << ';' << endl
      << name << "BF = " << model->getBf() << ';' << endl
	<< name << "BR = " << model->getBr() << ';' << endl
	<< name << "N = " << ( Q / (K * model->getTemp()) ) << ';' << endl;
    break;
  case Diode:
    outFile << name << "IS = " << value << ';' << endl
	    << name << "N = " << (Q / (K * temp) ) << ';' << endl;
    break;
  };
}
//   	wrt = With Respect To
void Component::printJac( int nodeNum, ofstream &outFile, int datum, int wrt, bool MNAflag){
    //~> MNAflag says if the type of analysis is MNA or NA
  switch( type ){
  case MOSFET:
    cout << "printJac for MOSFETs not implemented" << endl
	   << "PROGRAM ENDED ABNORMALLY!" << endl;
    exit(0);
    break;

  case BJT:
    if( (con0.node->getNum() == nodeNum) &&
        (model->getType() == NPN) &&
        (con0.node->getNameNum() == wrt) ){
      outFile << " (" << name << "IS"
	      << " * " << name << "N"
	      << " / " << name << "BR"
	      << ")*(1 + " << name << "BR"
	      << ")*(exp( - "
	      << name << "N*(";
      if( con0.node->getNameNum() != datum )
	outFile << "X(" << con0.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << " - X(" << con1.node->getNameNum() << ')';
      outFile << "))) ";
    }
    else if( (con0.node->getNum() == nodeNum) &&
	     (model->getType() == NPN) &&
	     (con2.node->getNameNum() == wrt) ){
      outFile << " ( - " << name << "IS"
	      << " * " << name << "N"
	      << ")*(exp( - "
	      << name << "N*(";
      if( con2.node->getNameNum() != datum )
	outFile << "X(" << con2.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << " - X(" << con1.node->getNameNum() << ')';
      outFile << "))) ";
    }
    else if( (con0.node->getNum() == nodeNum) &&
	     (model->getType() == NPN) &&
	     (con1.node->getNameNum() == wrt) ){
      outFile << " (" << name << "IS"
	      << " * " << name << "N"
	      << ")*(exp( - "
	      << name << "N*(";
      if( con2.node->getNameNum() != datum )
	outFile << "X(" << con2.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << " - X(" << con1.node->getNameNum() << ')';
      outFile << "))) ";
      outFile << " - ";
      outFile << "(" << name << "IS"
	      << " * " << name << "N"
	      << " / " << name << "BR"
	      << ")*(1 + " << name << "BR"
	      << ")*(exp( - "
	      << name << "N*(";
      if( con0.node->getNameNum() != datum )
	outFile << "X(" << con0.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << "-X(" << con1.node->getNameNum() << ')';
      outFile << "))) ";
    }

    else if( (con2.node->getNum() == nodeNum) &&
	     (model->getType() == NPN) &&
	     (con0.node->getNameNum() == wrt) ){
      outFile << " ( -  " << name << "IS"
       	      << " * " << name << "N"
	      << ")*(exp(-"
	      << name << "N*(";
      if( con0.node->getNameNum() != datum )
	outFile << "X(" << con0.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << " - X(" << con1.node->getNameNum() << ')';
      outFile << "))) ";
    }
    else if( (con2.node->getNum() == nodeNum) &&
	     (model->getType() == NPN) &&
	     (con2.node->getNameNum() == wrt) ){
      outFile << " (" << name << "IS"
	      << " * " << name << "N"
	      << " / " << name << "BF"
	      << ")*(1 + " << name << "BF"
	      << ")*(exp(-"
	      << name << "N*(";
      if( con2.node->getNameNum() != datum )
	outFile << "X(" << con2.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << " - X(" << con1.node->getNameNum() << ')';
      outFile << "))) ";
    }
    else if( (con2.node->getNum() == nodeNum) &&
	     (model->getType() == NPN) &&
	     (con1.node->getNameNum() == wrt) ){
      outFile << " ( - " << name << "IS"
	      << " * " << name << "N"
	      << " / " << name << "BF"
	      << ")*(1 + " << name << "BF"
	      << ")*(exp(-"
	      << name << "N*(";
      if( con2.node->getNameNum() != datum )
	outFile << "X(" << con2.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << "-X(" << con1.node->getNameNum() << ')';
      outFile << ")))";
      outFile << " + ";
      outFile << "(" << name << "IS"
	      << " * " << name << "N"
	      << ")*(exp(-"
	      << name << "N*(";
      if( con0.node->getNameNum() != datum )
	outFile << "X(" << con0.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << " - X(" << con1.node->getNameNum() << ')';
      outFile << "))) ";
    }

    else if( (con1.node->getNum() == nodeNum) &&
	     (model->getType() == NPN) &&
	     (con0.node->getNameNum() == wrt) ){
      outFile << " ( - " << name << "IS"
 	      << " * " << name << "N"
	      << " / " << name << "BR"
	      << ")*(1 + " << name << "BR"
	      << ")*(exp(-"
	      << name << "N*(";
      if( con0.node->getNameNum() != datum )
	outFile << "X(" << con0.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << " - X(" << con1.node->getNameNum() << ')';
      outFile << ")))";
      outFile << " + ";
      outFile << "(" << name << "IS"
 	      << " * " << name << "N"
	      << ")*(exp(-"
	      << name << "N*(";
      if( con0.node->getNameNum() != datum )
	outFile << "X(" << con0.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << " - X(" << con1.node->getNameNum() << ')';
      outFile << "))) ";
    }
    else if( (con1.node->getNum() == nodeNum) &&
	     (model->getType() == NPN) &&
	     (con2.node->getNameNum() == wrt) ){
      outFile << " (" << name << "IS"
	      << " * " << name << "N"
	      << ")*(exp(-"
	      << name << "N*(";
      if( con2.node->getNameNum() != datum )
	outFile << "X(" << con2.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << " - X(" << con1.node->getNameNum() << ')';
      outFile << ")))";
      outFile << " - ";
      outFile << "(" << name << "IS"
	      << " * " << name << "N"
	      << " / " << name << "BF"
	      << ")*(1 + " << name << "BF"
	      << ")*(exp(-"
	      << name << "N*(";
      if( con2.node->getNameNum() != datum )
	outFile << "X(" << con2.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << " - X(" << con1.node->getNameNum() << ')';
      outFile << "))) ";
    }
    else if( (con1.node->getNum() == nodeNum) &&
	     (model->getType() == NPN) &&
	     (con1.node->getNameNum() == wrt) ){
      outFile << " ( - " << name << "IS"
	      << " * " << name << "N"
	      << ")*(exp(-"
	      << name << "N*(";
      if( con2.node->getNameNum() != datum )
	outFile << "X(" << con2.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << " - X(" << con1.node->getNameNum() << ')';
      outFile << ")))";
      outFile << " + ";
      outFile << "(" << name << "IS"
	      << " * " << name << "N"
	      << " / " << name << "BR "
	      << ")*(1 + " << name << "BR "
	      << ")*(exp(-"
	      << name << "N*(";
      if( con0.node->getNameNum() != datum )
	outFile << "X(" << con0.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << "-X(" << con1.node->getNameNum() << ')';
      outFile << ")))";
      outFile << " + ";
      outFile << "(" << name << "IS"
	      << " * " << name << "N"
	      << " / " << name << "BF "
	      << ")*(1 + " << name << "BF "
	      << ")*(exp(-"
	      << name << "N*(";
      if( con2.node->getNameNum() != datum )
	outFile << "X(" << con2.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << " - X(" << con1.node->getNameNum() << ')';
      outFile << ")))";
      outFile << " - ";
      outFile << "(" << name << "IS"
	      << " * " << name << "N"
	      << ")*(exp(-"
	      << name << "N*(";
      if( con0.node->getNameNum() != datum )
	outFile << "X(" << con0.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << " - X(" << con1.node->getNameNum() << ')';
      outFile << "))) ";
    }

    else if( (con0.node->getNum() == nodeNum) &&
        (model->getType() == PNP) &&
        (con0.node->getNameNum() == wrt) ){
      outFile << " (" << name << "IS"
	      << " * " << name << "N"
	      << " / " << name << "BR"
	      << ")*(1 + " << name << "BR"
	      << ")*(exp( "
	      << name << "N*(";
      if( con0.node->getNameNum() != datum )
	outFile << "X(" << con0.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << " - X(" << con1.node->getNameNum() << ')';
      outFile << "))) ";
    }
    else if( (con0.node->getNum() == nodeNum) &&
	     (model->getType() == PNP) &&
	     (con2.node->getNameNum() == wrt) ){
      outFile << " ( - " << name << "IS"
	      << " * " << name << "N"
	      << ")*(exp( "
	      << name << "N*(";
      if( con2.node->getNameNum() != datum )
	outFile << "X(" << con2.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << " - X(" << con1.node->getNameNum() << ')';
      outFile << "))) ";
    }
    else if( (con0.node->getNum() == nodeNum) &&
	     (model->getType() == PNP) &&
	     (con1.node->getNameNum() == wrt) ){
      outFile << " (" << name << "IS"
	      << " * " << name << "N"
	      << ")*(exp( "
	      << name << "N*(";
      if( con2.node->getNameNum() != datum )
	outFile << "X(" << con2.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << " - X(" << con1.node->getNameNum() << ')';
      outFile << "))) ";
      outFile << " - ";
      outFile << "(" << name << "IS"
	      << " * " << name << "N"
	      << " / " << name << "BR"
	      << ")*(1 + " << name << "BR"
	      << ")*(exp( "
	      << name << "N*(";
      if( con0.node->getNameNum() != datum )
	outFile << "X(" << con0.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << "-X(" << con1.node->getNameNum() << ')';
      outFile << "))) ";
    }

    else if( (con2.node->getNum() == nodeNum) &&
	     (model->getType() == PNP) &&
	     (con0.node->getNameNum() == wrt) ){
      outFile << " ( -  " << name << "IS"
       	      << " * " << name << "N"
	      << ")*(exp("
	      << name << "N*(";
      if( con0.node->getNameNum() != datum )
	outFile << "X(" << con0.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << " - X(" << con1.node->getNameNum() << ')';
      outFile << "))) ";
    }
    else if( (con2.node->getNum() == nodeNum) &&
	     (model->getType() == PNP) &&
	     (con2.node->getNameNum() == wrt) ){
      outFile << " (" << name << "IS"
	      << " * " << name << "N"
	      << " / " << name << "BF"
	      << ")*(1 + " << name << "BF"
	      << ")*(exp("
	      << name << "N*(";
      if( con2.node->getNameNum() != datum )
	outFile << "X(" << con2.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << " - X(" << con1.node->getNameNum() << ')';
      outFile << "))) ";
    }
    else if( (con2.node->getNum() == nodeNum) &&
	     (model->getType() == PNP) &&
	     (con1.node->getNameNum() == wrt) ){
      outFile << " ( - " << name << "IS"
	      << " * " << name << "N"
	      << " / " << name << "BF"
	      << ")*(1 + " << name << "BF"
	      << ")*(exp("
	      << name << "N*(";
      if( con2.node->getNameNum() != datum )
	outFile << "X(" << con2.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << "-X(" << con1.node->getNameNum() << ')';
      outFile << ")))";
      outFile << " + ";
      outFile << "(" << name << "IS"
	      << " * " << name << "N"
	      << ")*(exp("
	      << name << "N*(";
      if( con0.node->getNameNum() != datum )
	outFile << "X(" << con0.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << " - X(" << con1.node->getNameNum() << ')';
      outFile << "))) ";
    }

    else if( (con1.node->getNum() == nodeNum) &&
	     (model->getType() == PNP) &&
	     (con0.node->getNameNum() == wrt) ){
      outFile << " ( - " << name << "IS"
 	      << " * " << name << "N"
	      << " / " << name << "BR"
	      << ")*(1 + " << name << "BR"
	      << ")*(exp("
	      << name << "N*(";
      if( con0.node->getNameNum() != datum )
	outFile << "X(" << con0.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << " - X(" << con1.node->getNameNum() << ')';
      outFile << ")))";
      outFile << " + ";
      outFile << "(" << name << "IS"
 	      << " * " << name << "N"
	      << ")*(exp("
	      << name << "N*(";
      if( con0.node->getNameNum() != datum )
	outFile << "X(" << con0.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << " - X(" << con1.node->getNameNum() << ')';
      outFile << "))) ";
    }
    else if( (con1.node->getNum() == nodeNum) &&
	     (model->getType() == PNP) &&
	     (con2.node->getNameNum() == wrt) ){
      outFile << " (" << name << "IS"
	      << " * " << name << "N"
	      << ")*(exp("
	      << name << "N*(";
      if( con2.node->getNameNum() != datum )
	outFile << "X(" << con2.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << " - X(" << con1.node->getNameNum() << ')';
      outFile << ")))";
      outFile << " - ";
      outFile << "(" << name << "IS"
	      << " * " << name << "N"
	      << " / " << name << "BF"
	      << ")*(1 + " << name << "BF"
	      << ")*(exp("
	      << name << "N*(";
      if( con2.node->getNameNum() != datum )
	outFile << "X(" << con2.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << " - X(" << con1.node->getNameNum() << ')';
      outFile << "))) ";
    }
    else if( (con1.node->getNum() == nodeNum) &&
	     (model->getType() == PNP) &&
	     (con1.node->getNameNum() == wrt) ){
      outFile << " ( - " << name << "IS"
	      << " * " << name << "N"
	      << ")*(exp("
	      << name << "N*(";
      if( con2.node->getNameNum() != datum )
	outFile << "X(" << con2.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << " - X(" << con1.node->getNameNum() << ')';
      outFile << ")))";
      outFile << " + ";
      outFile << "(" << name << "IS"
	      << " * " << name << "N"
	      << " / " << name << "BR "
	      << ")*(1 + " << name << "BR "
	      << ")*(exp("
	      << name << "N*(";
      if( con0.node->getNameNum() != datum )
	outFile << "X(" << con0.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << "-X(" << con1.node->getNameNum() << ')';
      outFile << ")))";
      outFile << " + ";
      outFile << "(" << name << "IS"
	      << " * " << name << "N"
	      << " / " << name << "BF "
	      << ")*(1 + " << name << "BF "
	      << ")*(exp("
	      << name << "N*(";
      if( con2.node->getNameNum() != datum )
	outFile << "X(" << con2.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << " - X(" << con1.node->getNameNum() << ')';
      outFile << ")))";
      outFile << " - ";
      outFile << "(" << name << "IS"
	      << " * " << name << "N"
	      << ")*(exp("
	      << name << "N*(";
      if( con0.node->getNameNum() != datum )
	outFile << "X(" << con0.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << " - X(" << con1.node->getNameNum() << ')';
      outFile << "))) ";
    }
    else
      outFile << " 0 ";
    break;

//           NOTE:  Assuming DC voltages, the derivatives are all zero
  case VSource:
    if ( ((con0.node->getNum() == nodeNum) && (con0.node->getNameNum() == wrt) /*~>*/&& (MNAflag == FALSE)) ||
         ((con1.node->getNum() == nodeNum) && (con1.node->getNameNum() == wrt) /*~>*/&& (MNAflag == FALSE)) )
      outFile << " 1 ";
    else if( ((con0.node->getNum() == nodeNum) && (con1.node->getNameNum() == wrt) /*~>*/&& (MNAflag == FALSE)) ||
             ((con1.node->getNum() == nodeNum) && (con0.node->getNameNum() == wrt) /*~>*/&& (MNAflag == FALSE)) )
      outFile << " (-1)";
    else
     outFile << " 0 ";
    break;

  case ISource:
    outFile << " 0 ";
    break;

  case Diode:
    if( ((con0.node->getNum() == nodeNum) && (con0.node->getNameNum() == wrt)) ||
	((con1.node->getNum() == nodeNum) && (con1.node->getNameNum() == wrt)) ){
      outFile << " (" << name << "IS"
	      << " * " << name << "N"
	      << ")*(exp("
	      << name << "N*(";
      if( con0.node->getNameNum() != datum )
	outFile << "X(" << con0.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << " - X(" << con1.node->getNameNum() << ')';
      outFile << "))) ";
    }
    else if( ((con0.node->getNum() == nodeNum) && (con1.node->getNameNum() == wrt)) ||
	     ((con1.node->getNum() == nodeNum) && (con0.node->getNameNum() == wrt)) ){
      outFile << " ( - " << name << "IS"
	      << " * " << name << "N"
	      << ")*(exp("
	      << name << "N*(";
      if( con0.node->getNameNum() != datum )
	outFile << "X(" << con0.node->getNameNum() << ')';
      if( con1.node->getNameNum() != datum )
	outFile << " - X(" << con1.node->getNameNum() << ')';
      outFile << "))) ";
    }
    else
       outFile << " 0 ";
    break;

  case Resistor:
    if( ((con0.node->getNum() == nodeNum) && (con0.node->getNameNum() == wrt)) ||
	((con1.node->getNum() == nodeNum) && (con1.node->getNameNum() == wrt)) )
      outFile << " 1/" << name << " ";
    else if( ((con0.node->getNum() == nodeNum) && (con1.node->getNameNum() == wrt)) ||
	     ((con1.node->getNum() == nodeNum) && (con0.node->getNameNum() == wrt)) )
      outFile << " (-1/" << name << ") ";
    else
      outFile << " 0 ";
    break;

  case Capacitor:
    outFile << " 0 ";
    break;

  case Inductor:
    cerr << "This section is not completed" << endl
	 << "PROGRAM ENDED ABNORMALLY!" << endl;
    exit(0);
    break;
  };
  return;
}
/* ~> */
void Component::printSuperNode(ofstream &outFile, int datum, int lastnode){
    if(type == VSource ){
        if((con0.node->getNameNum() != datum) && (con1.node->getNameNum() != datum))
        { //~> if is a floating source
            outFile << endl << "F(" << con1.node->getNameNum() << ") = ";
            outFile << "(";
            con0.node->printSuperNodal(outFile, datum, lastnode);
            outFile << ") ";
            outFile << '+';
            outFile << " (";
            con1.node->printSuperNodal(outFile, datum, lastnode);
            outFile << ")";
            outFile << ';' << endl;
        }
    }
    return;
}
Node* Component::getNode( int conNum ){
  switch(conNum) {
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
int Component::getNodeNum( int conNum ){
  switch(conNum) {
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
char* Component::getName(){
  return name;
}
Node::Node( int Num ){
	next = NULL;
	nodeNum = Num;
	conCount = 0;
	conList = NULL;
	nameNum = 0;
}
Node::~Node(){};
int Node::getNum(){
	return nodeNum;
	}
void Node::setNameNum( int numIn ){
  nameNum = numIn;
}
int Node::getNameNum(){
  return nameNum;
}
int Node::getCount(){
	return conCount;
	}
Connections * Node::getConList() {
 	return conList;
	}
void Node::connect( int conNumIn, Component* compIn ){
	Connections* conPtr;
	conCount++;
	if( conList == NULL ){
		conList = new Connections;
		conList->next = NULL;
		conList->conNum = conNumIn;
		conList->comp = compIn;
	}
	else {
		conPtr = conList;
		while ( conPtr->next != NULL )
			conPtr = conPtr->next;
		conPtr->next = new Connections;
		conPtr = conPtr->next;
		conPtr->next = NULL;
		conPtr->conNum = conNumIn;
		conPtr->comp = compIn;
		}
	}
Node* Node::getNext(){
	return next;
	}
void Node::setNext( Node* nodeIn ){
  next = nodeIn;
}
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
void Node::printNodal( ofstream &outFile, int datum, int lastnode) {
  Connections* conPtr;
  conPtr = conList;
  while( conPtr != NULL ){ // ~> checks if some 'connection' is connected to any Voltage source. If there is no connections with Voltages source, print the equations.
    if( conPtr->comp->getType() == VSource )
      return;
    conPtr = conPtr->next;
  }
  outFile << endl << "F(" << nameNum << ") = ";
  conPtr=conList;
  while (conPtr->next != NULL) { //~> Using 'conPtr->next' not conPtr to check the end of the while loop
    conPtr->comp->print( nodeNum, outFile, datum, lastnode );
    outFile << '+';
    conPtr = conPtr->next;
  }
  conPtr->comp->print( nodeNum, outFile, datum, lastnode );
  outFile << ';' << endl;
  return;
}
void Node::printMNA( ofstream &outFile, int datum, int lastnode) {
  Connections *conPtr;
  int print;
  print = 0;
  conPtr = conList;
  while (conPtr != NULL) {
    if ( conPtr->comp->getType() == VSource ) {  //~> seeking for a VSource
      print = 1;
      outFile << endl;
      outFile << "F(" << lastnode + conPtr->comp->getcompNum() << ")=";
    }
    conPtr = conPtr->next;
  }
  if (print) {
    conPtr = conList;
    while (conPtr->next != NULL) {
      conPtr->comp->print( nodeNum, outFile, datum, lastnode);
      outFile << "+";
      conPtr = conPtr->next;
    }
    conPtr->comp->print( nodeNum, outFile, datum, lastnode);
    outFile << ';' << endl;
  }
  return;
}
void Node::printJac( ofstream &outFile, int datum, Node* wrt, int lastnode,
EquaType eqType) {
  Connections* conPtr;
  conPtr = conList;
  while( conPtr != NULL ){
    if( conPtr->comp->getType() == VSource )
       return;
    conPtr = conPtr->next;
  }
  outFile << endl << "JAC(" << nameNum << ", " << wrt->getNameNum() << ") = ";
  conPtr = conList;
  while( conPtr->next != NULL ){
    conPtr->comp->printJac( nodeNum, outFile, datum, wrt->getNameNum(), FALSE );
    outFile << "+";
    conPtr = conPtr->next;
  }
  conPtr->comp->printJac( nodeNum, outFile, datum, wrt->getNameNum(), FALSE );
  outFile << ';' << endl;
  if (eqType == Modified) {
    conPtr = wrt->conList;
    while ( conPtr != NULL ) {
      if( conPtr->comp->getType() == VSource ) {
        outFile << endl << "JAC(" << nameNum << ", " ;
        outFile << lastnode + conPtr->comp->getcompNum() << ") = 0; ";
        outFile << endl;
      }
      conPtr = conPtr->next;
    }
  }
  return;
}
void Node::printJacMNA( ofstream &outFile, int datum, Node *wrt, int lastnode) {
  Connections *conPtr;
  int print, val;
  /* ~>*/
  bool MNAflagFALSE = FALSE; //Used to indicate if it is been printed JacMNA
  bool MNAflagTRUE = TRUE;
  /* <~*/
  print = 0;
  conPtr=conList;
  while (conPtr != NULL) {
    if (conPtr->comp->getType() == VSource){
      print = 1;
      val = conPtr->comp->getcompNum();
      outFile << endl << "JAC(" << lastnode + val;
      outFile << ", " << wrt->getNameNum() << ") = ";
    }
    conPtr = conPtr->next;
  }
  if (print) {
    conPtr = conList;
    while (conPtr->next != NULL) {
      conPtr->comp->printJac(nodeNum, outFile, datum, wrt->getNameNum(), MNAflagTRUE);
      outFile << "+";
      conPtr = conPtr->next;
    }
    conPtr->comp->printJac(nodeNum, outFile, datum, wrt->getNameNum(),MNAflagTRUE);
    outFile << ';' << endl;
    conPtr = wrt->conList;
    while ( conPtr != NULL) {
      if( conPtr->comp->getType() == VSource ) {
        outFile << endl << "JAC(" << lastnode + val << ", " ;
        outFile << lastnode + conPtr->comp->getcompNum() << ") = ";
        if ( conPtr->comp->getcompNum() == val)  outFile << "1;";
        else outFile << "0;";
	outFile << endl;
      }
      conPtr = conPtr->next;
    }
  }
  return;
}
void Node::printSuperNodal(ofstream &outFile, int datum, int lastnode) {
  Connections* conPtr;
  conPtr = conList;
  conPtr=conList;
  while (conPtr->next != NULL) { //~> Using 'conPtr->next' not conPtr to check the end of the while loop
        if(conPtr->comp->getType() != VSource){
            conPtr->comp->print( nodeNum, outFile, datum, lastnode );
            }
            conPtr = conPtr->next;

  }
    if(conPtr->comp->getType() != VSource){
            outFile << '+';
        conPtr->comp->print( nodeNum, outFile, datum, lastnode );
    }
  return;
}
NodeHead::NodeHead(){
	nodeList = NULL;
	nodeCount = 0;
	}
NodeHead::~NodeHead(){};
Node* NodeHead::addNode(){
	Node* nodePtr;
	nodeCount++;
	if( nodeList == NULL ){
		nodeList = new Node(nodeCount);
		return nodeList;
		}
	else {
		nodePtr = nodeList;
		while( nodePtr->getNext() != NULL )
			nodePtr = nodePtr->getNext();
		nodePtr->setNext( new Node(nodeCount) );
	       	return nodePtr->getNext();
		}
	}
int NodeHead::getCount(){
	return nodeCount;
	}
Node* NodeHead::getNode( int nodeNum ){
	Node* nodePtr;
	// need check that nodeNum does not exceed node count
	nodePtr = nodeList;
	for( int a=0; a < nodeNum; a++ )
		nodePtr = nodePtr->getNext();
	return nodePtr;
	}
CompHead::CompHead(){
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
void CompHead::addComp( Component* component ){
	Component* compPtr;
	switch ( component->getType() ){
	case ISource:
	  iSCount++;
	  if( compList == NULL ){
	    compList = component;
	    component->setNum( iSCount );
	  }
	  else{
	    compPtr = compList;
	    while( compPtr->getNext() != NULL )
	      compPtr = compPtr->getNext();
	    compPtr->setNext( component );
	    compPtr = compPtr->getNext();
	    compPtr->setNum( iSCount );
		}
	  break;
	case VSource:
	  vSCount++;
	  if( compList == NULL ){
	    compList = component;
	    component->setNum( vSCount );
	  }
	  else{
	    compPtr = compList;
	    while( compPtr->getNext() != NULL )
	      compPtr = compPtr->getNext();
	    compPtr->setNext( component );
	    compPtr = compPtr->getNext();
	    compPtr->setNum( vSCount );
		}
	  break;
	case Resistor:
	  rCount++;
	  if( compList == NULL ){
	    compList = component;
	    component->setNum( rCount );
	  }
	  else{
	    compPtr = compList;
	    while( compPtr->getNext() != NULL )
	      compPtr = compPtr->getNext();
	    compPtr->setNext( component );
	    compPtr = compPtr->getNext();
	    compPtr->setNum( rCount );
		}
	  break;
	case MOSFET:
	  mCount++;
	  if( compList == NULL ){
	    compList = component;
	    component->setNum( mCount );
	  }
	  else{
	    compPtr = compList;
	    while( compPtr->getNext() != NULL )
	      compPtr = compPtr->getNext();
	    compPtr->setNext( component );
	    compPtr = compPtr->getNext();
	    compPtr->setNum( mCount );
		}
	  break;
	case BJT:
	  bCount++;
	  if( compList == NULL ){
	    compList = component;
	    component->setNum( bCount );
	  }
	  else{
	    compPtr = compList;
	    while( compPtr->getNext() != NULL )
	      compPtr = compPtr->getNext();
	    compPtr->setNext( component );
	    compPtr = compPtr->getNext();
	    compPtr->setNum( bCount );
		}
	  break;
	case Diode:
	  dCount++;
	  if( compList == NULL ){
	    compList = component;
	    component->setNum( dCount );
	  }
	  else{
	    compPtr = compList;
	    while( compPtr->getNext() != NULL )
	      compPtr = compPtr->getNext();
	    compPtr->setNext( component );
	    compPtr = compPtr->getNext();
	    compPtr->setNum( dCount );
		}
	  break;
        case Capacitor:
	  cCount++;
	  if( compList == NULL ){
	    compList = component;
	    component->setNum( cCount );
	  }
	  else{
	    compPtr = compList;
	    while( compPtr->getNext() != NULL )
	      compPtr = compPtr->getNext();
	    compPtr->setNext( component );
	    compPtr = compPtr->getNext();
	    compPtr->setNum( cCount );
		}
	  break;
        case Inductor:
	  iCount++;
	  if( compList == NULL ){
	    compList = component;
	    component->setNum( iCount );
	  }
	  else{
	    compPtr = compList;
	    while( compPtr->getNext() != NULL )
	      compPtr = compPtr->getNext();
	    compPtr->setNext( component );
	    compPtr = compPtr->getNext();
	    compPtr->setNum( iCount );
		}
	  break;
	}
}
int CompHead::getCount( CompType type){
  switch (type) {
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
Component* CompHead::getComp( int compNum ){
	Component* compPtr;
	compPtr = compList;
	for( int a=0; a<compNum; a++ )
		compPtr = compPtr->getNext();
	return compPtr;
	}
Model::Model( char* nameIn, TranType typeIn, double isIn, double bfIn,
	      double brIn, double tempIn ) {
  strcpy( name, nameIn );
  type = typeIn;
  is = isIn;
  bf = bfIn;
  br = brIn;
  temp = tempIn;
  next = NULL;
}
TranType Model::getType(){
  return type;
}
char* Model::getName(){
  return name;
}
double Model::getIs(){
  return is;
}
double Model::getBf(){
  return bf;
}
double Model::getBr(){
  return br;
}
double Model::getTemp(){
  return temp;
}
void Model::setNext( Model* nextIn ){
  next = nextIn;
}
Model* Model::getNext(){
  return next;
}
ModelHead::ModelHead(){
  modelList = NULL;
}
void ModelHead::addModel( Model* modelIn ){
  Model* modelPtr;
  if( modelList == NULL ){
    modelList = modelIn;
  }
  else{
    modelPtr = modelList;
    while( modelPtr->getNext() != NULL ){
      modelPtr = modelPtr->getNext();
    }
    modelPtr->setNext( modelIn );
  }
}
Model* ModelHead::getModel ( char* nameIn ){
  Model* modelPtr = modelList;
  while( strcmp( modelPtr->getName(), nameIn ) ){
    modelPtr = modelPtr->getNext();
  }
  return modelPtr;
}