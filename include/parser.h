#pragma once
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <string.h>
#include <cmath>
using namespace std;

const double K = 1.38E-23;
const double Q = 1.60E-19;

char CompTypeName[][20] = {"BJT", "VSource", "ISource", "Resistor"}; //组件类型名
enum CompType {BJT, VSource, ISource, Resistor}; //组件类型
const unsigned CompTypeCount=4, NameLength=80, BufLength=300, NA=0;

char TranTypeName[][5] = {"NPN", "PNP"};
enum TranType { NPN, PNP }; //三极管类型
enum EquaType{ Nodal, Modified };

unsigned *Counts[CompTypeCount], nodeCount=0, modelCount=0, componentCount=0;
double *nvalue = NULL;

struct Model;
struct Node;
struct Component;

Model *models = NULL;
Component *components = NULL;
Node *nodes = NULL;
typedef struct Model
{
    char name[NameLength];    // 模型名
    double is, bf, br, te, n; // bjt的IS BF BR TE N
    TranType type;            // bjt类型
    void InitModel( char* nameIn, TranType typeIn, double isIn, double bfIn, double brIn, double teIn, double nIn)
    {
        strcpy(name, nameIn);
        type = typeIn;
        is = isIn;
        bf = bfIn;
        br = brIn;
        te = teIn;
        n  = nIn;
    }
    double getfe(unsigned &x1, unsigned &x2);
    double getfc(unsigned &x1, unsigned &x2);
    double getIe(unsigned x1, unsigned x2);
    double getIc(unsigned x1, unsigned x2);
}Model;


typedef struct Component
{
    CompType type;                         // 器件类型
    unsigned con[4], nodeLinkCnt, compNum; // 端口连接的节点 连接节点数量 器件号(该类器件第几个)
    double value;                          // 器件的值(V I R)
    unsigned model;                        // 模型号(bjt)
    char name[NameLength];                 // 器件名
    void init()
    {
        strcpy(name, "");
        value = nodeLinkCnt = compNum = 0;
        memset(con, 0, sizeof(int) * 4);
        model = 0;
    }
    void InitComponent(CompType typeIn, unsigned *conIn, unsigned cntIn, unsigned compNumIn, double valueIn, char *nameIn, unsigned modelIn)
    {
        type = typeIn;
        memcpy(con, conIn, sizeof(unsigned) * cntIn);
        nodeLinkCnt = cntIn;
        compNum = compNumIn;
        value = valueIn;
        strcpy(name, nameIn);
        model = modelIn;
    }
    bool printFunc(char *str, unsigned conNum, unsigned nodeNameNum);
    void printValue(ofstream &outFile)
    {
        switch(type)
        {
            case Resistor:
                outFile << name << " = " << value << endl;
                break;
            case VSource:
                outFile << name << " = " << value << endl;
                break;
            case ISource:
                outFile << name << " = " << value << endl;
                break;
            case BJT:
                outFile << name << ": IS = " << models[model].is
                    << " BF = " << models[model].bf
                    << " BR = " << models[model].br
                    << " TE = "  << models[model].te
                    << " N = "  << models[model].n << endl;
                break;
        }
    }
}Component;

typedef struct Node
{
    unsigned *comp;                      // 节点连接的器件
    EquaType type;                       // 节点类型
    unsigned compLinkCnt, nodeNum, *con; // 节点连接的器件的数量 节点号 器件的连接端口
    double value;
    void init(unsigned nodeNumIn)
    {
        compLinkCnt = 0;
        nodeNum = nodeNumIn;
        con = NULL;
        comp = NULL;
    }
    void Myprint1(ofstream &outFile)
    {
        outFile << "节点" << nodeNum << "        所连器件数为:" << compLinkCnt << endl;
        for(unsigned a = 0; a < compLinkCnt; ++a)
        {
            outFile << "    类型:" << CompTypeName[components[comp[a]].type] << "    连接端口:" << con[a] <<
                "    名称:" << components[comp[a]].name << "    编号:" << components[comp[a]].compNum <<
                "    value:" << components[comp[a]].value << endl;
        }
    }
}Node;

double Model::getfe(unsigned &x1, unsigned &x2)
{
    return is * (bf + 1) / bf * (exp(nodes[x1].value - nodes[x2].value) - 1);
}
double Model::getfc(unsigned &x1, unsigned &x2)
{
    return is * (br + 1) / br * (exp(nodes[x1].value - nodes[x2].value) - 1);
}
double Model::getIe(unsigned x1, unsigned x2)
{
    return getfe(x1, x2) - is * (exp(nodes[x1].value - nodes[x2].value) - 1);
}
double Model::getIc(unsigned x1, unsigned x2)
{
    return getfc(x1, x2) - is * (exp(nodes[x1].value - nodes[x2].value) - 1);
}

bool Component::printFunc(char *str, unsigned conNum, unsigned nodeNameNum)
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
            nodes[con[1]].nodeNum;
            if(nodes[con[0]].nodeNum == 0 || nodes[con[1]].nodeNum == 0) sprintf(str, "x%d/%s", nodeNameNum, name);
            else if(nodes[con[0]].nodeNum == nodeNameNum) sprintf(str, "(x%d - x%d)/%s", nodeNameNum, nodes[con[1]].nodeNum, name);
            else sprintf(str, "(x%d - x%d)/%s", nodeNameNum, nodes[con[0]].nodeNum, name);
            return true;
        case VSource:
            if(nodes[con[0]].nodeNum == 0 || nodes[con[1]].nodeNum == 0) sprintf(str, "x%d - %s", nodeNameNum, name);
            else if(nodes[con[0]].nodeNum == nodeNameNum) sprintf(str, "x%d - x%d - %s", nodeNameNum, nodes[con[1]].nodeNum,name);
            else
            {
                sprintf(str, " - x%d", nodeCount - Counts[VSource][0] - 1 + compNum);
                return false;
            }
            return true;
    }
    return false;
}

void Init()
{
    for(unsigned a = 0; a < CompTypeCount; ++a) Counts[a] = (unsigned *)malloc(sizeof(unsigned));
}
void Destroy()
{
    free(models);
    free(components);
    for(unsigned a = 0; a < nodeCount; ++a)
    {
        free(nodes[a].comp);
        free(nodes[a].con);
    }
    free(nodes);
    for(unsigned a = 0; a < CompTypeCount; ++a) free(Counts[a]);
}

double stripString( char *stringIn )
{
    char buf[BufLength], buf2[BufLength];
    int a, b;
    strcpy(buf, stringIn);
    for(a = 0; buf[a] != '='; ++a){};
    ++a;
    for(b = 0; buf[a] != '\0'; b++, a++) buf2[b] = buf[a];
    buf2[b] = '\0';
    return atof(buf2);
};
char strComponentType(Component* compPtr)
{
    switch(compPtr->type)
    {
        case VSource: return 'V';
        case Resistor: return 'R';
        case BJT: return 'Q';
        case ISource: return 'I';
    }
    return '\0';
}
void printComponents()
{
    cout << "Components: " << endl;
    for(unsigned a = 0; a < componentCount; ++a)
        cout << "->" << strComponentType(&components[a]) << components[a].compNum << '(' << components[a].name << ')';
    cout << endl;
    return;
}
void printNodes()
{
    cout << "Nodes: ->节点号 {->器件号(器件名)*} {节点初始值}" << endl;
    cout << "-> " << 0 << endl;
    for(unsigned a = 1; a < nodeCount; ++a)
    {
        cout << "-> " << a << " {";
        for(unsigned b = 0; b < nodes[a].compLinkCnt; ++b)
            cout << "->" << strComponentType(&components[nodes[a].comp[b]]) << components[nodes[a].comp[b]].compNum
                 << '(' << components[nodes[a].comp[b]].name << ')';
        cout << "} {" << nodes[a].value << '}' << endl;
    }
    return;
}

void NodalFunc(ofstream &outFile)
{
    bool flag;
    char str[BufLength];
    unsigned a, b, temp1, temp2, temp3 = 0;
    outFile << endl << "Functions:" << endl;
    for(a = 1; a < nodeCount - Counts[VSource][0]; ++a)
    {
        flag = false;
        outFile << "F(" << a << ") = ";
        for(unsigned b = 0; b < nodes[a].compLinkCnt; ++b)
        {
            if(components[nodes[a].comp[b]].type == VSource && nodes[components[nodes[a].comp[b]].con[0]].nodeNum == a)
            {
                components[nodes[a].comp[b]].printFunc(str, nodes[a].con[b], a);
                flag = true;
                outFile << str;
                strcpy(str, "");
                break;
            }
        }
        if(!flag)
        {
            for(unsigned b = 0; b < nodes[a].compLinkCnt; ++b)
            {
                if(components[nodes[a].comp[b]].printFunc(str, nodes[a].con[b], a) && b != 0) outFile << " + ";
                outFile << str;
                strcpy(str, "");
            }
        }
        outFile << endl;
    }
    for(; a < nodeCount; ++a)
    {
        ++temp3;
        outFile << "F(" << a << ") = x" << a << ' ';
        temp1 = components[Counts[VSource][temp3]].con[0];
        for(b = 0; b < nodes[temp1].compLinkCnt; ++b)
        {
            if(components[nodes[temp1].comp[b]].type != VSource)
            {
                if(components[nodes[temp1].comp[b]].printFunc(str, nodes[temp1].con[b], temp1)) outFile << " + ";
                outFile << str;
            }
            strcpy(str, "");
        }
        outFile << endl;
    }
}