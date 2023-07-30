#pragma once
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <omp.h>
#include <eigen3/Eigen/Sparse>
using namespace std;

const double K = 1.3806503E-23;
const double Q = 1.602176E-19;

char CompTypeName[][20] = {"BJT", "VSource", "ISource", "Resistor"}; //组件类型名
enum CompType {BJT, VSource, ISource, Resistor}; //组件类型
const unsigned CompTypeCount=4, NameLength=80, BufLength=300, NA=0;

char TranTypeName[][5] = {"NPN", "PNP"};
enum TranType { NPN, PNP }; //三极管类型
enum EquaType{ Nodal, Modified };

unsigned *Counts[CompTypeCount], nodeCount=0, modelCount=0, componentCount=0, maxit = 500;
double *nvalue = NULL, tol = 1e-3;
struct timeval time1, time2;

struct Model;
struct Node;
struct Component;

Model *models = NULL;
Component *components = NULL;
Node *nodes = NULL;

Eigen::SparseMatrix<double, Eigen::ColMajor> Jx, invJx, I; // J(X)
Eigen::VectorXd X, Fx; // X F(X)
Eigen::SparseLU<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>> luSolver;

vector<Eigen::Triplet<double>> SpVec;
vector<Eigen::VectorXd> Xs;

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
    bool getNodalIcFunc(char *str, unsigned compNum);
    bool getNodalIeFunc(char *str, unsigned compNum);
    bool getJacobiIcFunc(char *str, unsigned compNum, unsigned nodeNum);
    bool getJacobiIeFunc(char *str, unsigned compNum, unsigned nodeNum);
    double getJacobiIe(unsigned compNum, unsigned nodeNum);
    double getJacobiIc(unsigned compNum, unsigned nodeNum);
    double getNodalIe(unsigned compNum);
    double getNodalIc(unsigned compNum);
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
    bool getNodalFunc(char *str, unsigned conNum, unsigned nodeNum, unsigned *num, unsigned *cnt);
    bool getJacobiFunc(char *str, unsigned conNum, unsigned nodeNum1, unsigned nodeNum2);
    void printNodeValue(ofstream &outFile)
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
    double getNodalValue(unsigned conNum, unsigned nodeNum);
    double getJacobiValue(unsigned conNum, unsigned nodeNum1, unsigned nodeNum2);
}Component;
typedef struct Node
{
    unsigned *comp;                         // 节点连接的器件
    EquaType type;                          // 节点类型
    unsigned compLinkCnt, nodeNum, *con;    // 节点连接的器件的数量 节点号 器件的连接端口
    double value;
    unordered_map<unsigned, bool> nodeFlag; // 该节点方程中使用的节点
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
    void printNodalFunc(ofstream &outFile)
    {
        unsigned num[4], cnt;
        bool flag = false;
        char str[BufLength];
        outFile << "F(" << nodeNum << ") = ";
        nodeFlag.reserve(nodeCount);
        nodeFlag[nodeNum] = true;
        if(type == Nodal)
        {
            for(unsigned b = 0; b < compLinkCnt; ++b)
            {
                if(components[comp[b]].type == VSource && nodes[components[comp[b]].con[0]].nodeNum == nodeNum)
                {
                    components[comp[b]].getNodalFunc(str, con[b], nodeNum, num, &cnt);
                    flag = true;
                    outFile << str;
                    strcpy(str, "");
                    for(b = 0; b < cnt; ++b) nodeFlag[num[b]] = true;
                    break;
                }
            }
            if(!flag)
            {
                for(unsigned b = 0; b < compLinkCnt; ++b)
                {
                    if(components[comp[b]].getNodalFunc(str, con[b], nodeNum, num, &cnt) && b != 0) outFile << " + ";
                    outFile << str;
                    strcpy(str, "");
                    for(unsigned c = 0; c < cnt; ++c) nodeFlag[num[c]] = true;
                }
            }
        }
        else
        {
            unsigned temp1, temp2, b;
            outFile << 'x' << nodeNum << ' ';
            temp2 = nodeNum - (nodeCount - Counts[VSource][0] - 1);
            temp1 = components[Counts[VSource][temp2]].con[0];
            for(b = 0; b < nodes[temp1].compLinkCnt; ++b)
            {
                if(components[nodes[temp1].comp[b]].type != VSource)
                {
                    if(components[nodes[temp1].comp[b]].getNodalFunc(str, nodes[temp1].con[b], temp1, num, &cnt)) outFile << " + ";
                    outFile << str;
                    for(unsigned c = 0; c < cnt; ++c) nodeFlag[num[c]] = true;
                }
                strcpy(str, "");
            }
        }
        outFile << endl;
        nodeFlag.reserve(nodeFlag.size());
    }
    void printJacobiFunc(ofstream &outFile)
    {
        bool flag1, flag2;
        char str[BufLength];
        for(unsigned a = 1; a < nodeCount; ++a)
        {
            outFile << "JAC(" << nodeNum << ", " << a << ") = ";
            if(nodeFlag[a])
            {
                if(type == Nodal)
                {       
                    flag1 = flag2 = false;
                    for(unsigned b = 0; b < compLinkCnt; ++b)
                    {
                        if(components[comp[b]].type == VSource && nodes[components[comp[b]].con[0]].nodeNum == nodeNum)
                        {
                            components[comp[b]].getJacobiFunc(str, con[b], nodeNum, a);
                            flag1 = true;
                            outFile << str;
                            strcpy(str, "");
                            break;
                        }
                    }
                    if(!flag1)
                    {
                        for(unsigned b = 0; b < compLinkCnt; ++b)
                        {
                            flag1 = components[comp[b]].getJacobiFunc(str, con[b], nodeNum, a);
                            if(str[0] != '0')
                            {
                                if(flag1 && flag2) outFile << " + ";
                                else if(flag2) outFile << ' ';
                                outFile << str;
                                flag2 = true;
                            }
                            strcpy(str, "");
                        }
                    }
                }
                else
                {
                    flag1 = flag2 = false;
                    unsigned temp1, temp2, b;
                    temp2 = nodeNum - (nodeCount - Counts[VSource][0] - 1);
                    temp1 = components[Counts[VSource][temp2]].con[0];
                    if(nodeNum == a) outFile << "1 ";
                    for(b = 0; b < nodes[temp1].compLinkCnt; ++b)
                    {
                        if(components[nodes[temp1].comp[b]].type != VSource)
                        {
                            flag1 = components[nodes[temp1].comp[b]].getJacobiFunc(str, nodes[temp1].con[b], temp1, a);
                            if(str[0] != '0')
                            {
                                if(flag1 && flag2) outFile << " + ";
                                else if(flag2) outFile << ' ';
                                outFile << str;
                                flag2 = true;
                            }
                        }
                        strcpy(str, "");
                    }
                }
                SpVec.push_back(Eigen::Triplet<double>(nodeNum - 1, a - 1, 1.0));
                outFile << endl;
            }
            else outFile << 0 << endl;
        }
    }
    double getNodalValue()
    {
        double val = 0.0;
        bool flag = false;
        if(type == Nodal)
        {
            for(unsigned b = 0; b < compLinkCnt; ++b)
            {
                if(components[comp[b]].type == VSource && nodes[components[comp[b]].con[0]].nodeNum == nodeNum)
                {
                    val = components[comp[b]].getNodalValue(con[b], nodeNum);
                    flag = true;
                    break;
                }
            }
            if(!flag)
            {
                for(unsigned b = 0; b < compLinkCnt; ++b) val += components[comp[b]].getNodalValue(con[b], nodeNum);
            }
        }
        else
        {
            unsigned temp1, temp2, b;
            val = value;
            temp2 = nodeNum - (nodeCount - Counts[VSource][0] - 1);
            temp1 = components[Counts[VSource][temp2]].con[0];
            for(b = 0; b < nodes[temp1].compLinkCnt; ++b)
            {
                if(components[nodes[temp1].comp[b]].type != VSource)
                    val += components[nodes[temp1].comp[b]].getNodalValue(nodes[temp1].con[b], temp1);
            }
        }
        return val;
    }
    double getJacobiValue(unsigned nodeNum2)
    {
        bool flag1;
        double val = 0.0;
        if(type == Nodal)
        {       
            flag1 = false;
            for(unsigned b = 0; b < compLinkCnt; ++b)
            {
                if(components[comp[b]].type == VSource && nodes[components[comp[b]].con[0]].nodeNum == nodeNum)
                {
                    val = components[comp[b]].getJacobiValue(con[b], nodeNum, nodeNum2);
                    flag1 = true;
                    break;
                }
            }
            if(!flag1)
            {
                for(unsigned b = 0; b < compLinkCnt; ++b)
                    val += components[comp[b]].getJacobiValue(con[b], nodeNum, nodeNum2);
            }
        }
        else
        {
            flag1 = false;
            unsigned temp1, temp2, b;
            temp2 = nodeNum - (nodeCount - Counts[VSource][0] - 1);
            temp1 = components[Counts[VSource][temp2]].con[0];
            if(nodeNum == nodeNum2) val += 1;
            for(b = 0; b < nodes[temp1].compLinkCnt; ++b)
            {
                if(components[nodes[temp1].comp[b]].type != VSource)
                    val += components[nodes[temp1].comp[b]].getJacobiValue(nodes[temp1].con[b], temp1, nodeNum2);
            }
        }
        return val;
    }
}Node;

bool Model::getNodalIcFunc(char *str, unsigned compNum)
{
    if(type = NPN)
        sprintf(str, "- Is/ar * (e^(-n*(x%d-x%d)) - 1) + Is * (e^(-n*(x%d-x%d)) - 1)"
            , components[compNum].con[0], components[compNum].con[1], components[compNum].con[2], components[compNum].con[1]);
    // else if(type == PNP)
    //     sprintf(str, "Is/ar * (e^(n*(x%d-x%d)) - 1) - Is * (e^(n*(x%d-x%d)) - 1)"
    //         , components[compNum].con[0], components[compNum].con[1], components[compNum].con[2], components[compNum].con[1]);
    return true;
}
bool Model::getNodalIeFunc(char *str, unsigned compNum)
{
    if(type == NPN)
        sprintf(str, "- Is/af * (e^(-n*(x%d-x%d)) - 1) + Is * (e^(-n*(x%d-x%d)) - 1)"
            , components[compNum].con[2], components[compNum].con[1], components[compNum].con[0], components[compNum].con[1]);
    // else if(type == PNP)
    //     sprintf(str, "Is/af * (e^(n*(x%d-x%d)) - 1) - Is * (e^(n*(x%d-x%d)) - 1)"
    //         , components[compNum].con[2], components[compNum].con[1], components[compNum].con[0], components[compNum].con[1]);
    return true;
}
bool Model::getJacobiIcFunc(char *str, unsigned compNum, unsigned nodeNum)
{
    if(type == NPN)
    {
        if(nodeNum == components[compNum].con[0])
            sprintf(str, "n * Is/ar * e^(-n*(x%d-x%d))", components[compNum].con[0], components[compNum].con[1]);
        else if(nodeNum == components[compNum].con[1])
            sprintf(str, "((-n) * Is/ar * e^(-n*(x%d-x%d)) + n * Is * e^(-n*(x%d-x%d)))"
                , components[compNum].con[0], components[compNum].con[1], components[compNum].con[2], components[compNum].con[1]);
        else if(nodeNum == components[compNum].con[2])
            sprintf(str, "(-n) * Is * e^(-n*(x%d-x%d))", components[compNum].con[2], components[compNum].con[1]);
        else strcpy(str, "0");
    }
    // else if(type == PNP)
    // {
    //     if(nodeNum == components[compNum].con[0])
    //         sprintf(str, "n * Is/ar * e^(n*(x%d-x%d))", components[compNum].con[0], components[compNum].con[1]);
    //     else if(nodeNum == components[compNum].con[1])
    //         sprintf(str, "((-n) * Is/ar * e^(n*(x%d-x%d)) + n * Is * e^(n*(x%d-x%d)))"
    //             , components[compNum].con[0], components[compNum].con[1], components[compNum].con[2], components[compNum].con[1]);
    //     else if(nodeNum == components[compNum].con[2])
    //         sprintf(str, "n * Is * e^(n*(x%d-x%d))", components[compNum].con[2], components[compNum].con[1]);
    //     else strcpy(str, "0");
    // }
    return true;
}
bool Model::getJacobiIeFunc(char *str, unsigned compNum, unsigned nodeNum)
{
    if(type == NPN)
    {
        if(nodeNum == components[compNum].con[0])
            sprintf(str, "(-n) * Is * e^(-n*(x%d-x%d))", components[compNum].con[0], components[compNum].con[1]);
        else if(nodeNum == components[compNum].con[1])
            sprintf(str, "((-n) * Is/af * e^(-n*(x%d-x%d)) + n * Is * e^(-n*(x%d-x%d)))"
                , components[compNum].con[2], components[compNum].con[1], components[compNum].con[0], components[compNum].con[1]);
        else if(nodeNum == components[compNum].con[2])
            sprintf(str, "n * Is/af * e^(-n*(x%d-x%d))", components[compNum].con[2], components[compNum].con[1]);
        else strcpy(str, "0");
    }
    else if(type == PNP)
    {
        if(nodeNum == components[compNum].con[0])
            sprintf(str, "n * Is * e^(n*(x%d-x%d))", components[compNum].con[0], components[compNum].con[1]);
        else if(nodeNum == components[compNum].con[1])
            sprintf(str, "((-n) * Is/af * e^(n*(x%d-x%d)) + n * Is * e^(n*(x%d-x%d)))"
                , components[compNum].con[2], components[compNum].con[1], components[compNum].con[0], components[compNum].con[1]);
        else if(nodeNum == components[compNum].con[2])
            sprintf(str, "n * Is/af * e^(n*(x%d-x%d))", components[compNum].con[2], components[compNum].con[1]);
        else strcpy(str, "0");
    }
    return true;
}
double Model::getNodalIc(unsigned compNum)
{
    double val = 0.0;
    if(type == NPN)
    {
        val = (- is * (br + 1)) / br * (exp((-n) * (nodes[components[compNum].con[0]].value - nodes[components[compNum].con[1]].value))- 1)
                 + is * (exp((-n) * (nodes[components[compNum].con[2]].value - nodes[components[compNum].con[1]].value)) - 1);
    }
    // else if(type == PNP)
    // {
    //     val = (is * (br + 1)) / br * (exp(n * (nodes[components[compNum].con[0]].value - nodes[components[compNum].con[1]].value))- 1)
    //              - is * (exp(n * (nodes[components[compNum].con[2]].value - nodes[components[compNum].con[1]].value)) - 1);
    // }
    return val;
}
double Model::getNodalIe(unsigned compNum)
{
    double val = 0.0;
    if(type == NPN)
    {
        val = (- is * (bf + 1)) / bf * (exp((-n) * (nodes[components[compNum].con[2]].value - nodes[components[compNum].con[1]].value))- 1)
                 + is * (exp((-n) * (nodes[components[compNum].con[0]].value - nodes[components[compNum].con[1]].value)) - 1);
    }
    // else if(type == PNP)
    // {
    //     val = (is * (bf + 1)) / bf * (exp(n * (nodes[components[compNum].con[2]].value - nodes[components[compNum].con[1]].value))- 1)
    //              - is * (exp(n * (nodes[components[compNum].con[0]].value - nodes[components[compNum].con[1]].value)) - 1);
    // }
    return val;
}
double Model::getJacobiIc(unsigned compNum, unsigned nodeNum)
{
    double val = 0.0;
    if(type == NPN)
    {
        if(nodeNum == components[compNum].con[0]) 
            val = n * is * (br+1) / br * exp((-n) * (nodes[components[compNum].con[0]].value - nodes[components[compNum].con[1]].value));
        else if(nodeNum == components[compNum].con[1])
            val = ((-n) * is * (br+1) / br * exp((-n) * (nodes[components[compNum].con[0]].value - nodes[components[compNum].con[1]].value))
                    + n * is * exp((-n) * (nodes[components[compNum].con[2]].value - nodes[components[compNum].con[1]].value)));
        else if(nodeNum == components[compNum].con[2])
            val = (-n) * is * exp((-n) * (nodes[components[compNum].con[2]].value - nodes[components[compNum].con[1]].value));
    }
    else if(type == PNP)
    {
        cout << "wait to finish" << endl;
    }
    // else if(type == PNP)
    // {
    //     if(nodeNum == components[compNum].con[0])
    //         sprintf(str, "n * Is/ar * e^(n*(x%d-x%d))", components[compNum].con[0], components[compNum].con[1]);
    //     else if(nodeNum == components[compNum].con[1])
    //         sprintf(str, "((-n) * Is/ar * e^(n*(x%d-x%d)) + n * Is * e^(n*(x%d-x%d)))"
    //             , components[compNum].con[0], components[compNum].con[1], components[compNum].con[2], components[compNum].con[1]);
    //     else if(nodeNum == components[compNum].con[2])
    //         sprintf(str, "n * Is * e^(n*(x%d-x%d))", components[compNum].con[2], components[compNum].con[1]);
    //     else strcpy(str, "0");
    // }
    return val;
}
double Model::getJacobiIe(unsigned compNum, unsigned nodeNum)
{
    double val = 0.0;
    if(type == NPN)
    {
        if(nodeNum == components[compNum].con[0])
            val =  (-n) * is * exp((-n) * (nodes[components[compNum].con[0]].value - nodes[components[compNum].con[1]].value));
        else if(nodeNum == components[compNum].con[1])
            val = ((-n) * is * (bf+1) / bf * exp((-n) * (nodes[components[compNum].con[2]].value - nodes[components[compNum].con[1]].value))
                     + n * is * exp((-n) * (nodes[components[compNum].con[0]].value - nodes[components[compNum].con[1]].value)));
        else if(nodeNum == components[compNum].con[2])
            val = n * is * (bf+1) / bf * exp((-n) * (nodes[components[compNum].con[2]].value - nodes[components[compNum].con[1]].value));
    }
    else if(type == PNP)
    {
        cout << "wait to finish!!!" << endl;
    }
    return val;
}

bool Component::getNodalFunc(char *str, unsigned conNum, unsigned nodeNum, unsigned *num, unsigned *cnt)
{
    bool flag = false;
    unsigned temp1, temp2, temp3;
    *cnt = 0;
    switch(type)
    {
        case BJT:
            if(conNum == 0)
            {
                sprintf(str, "Ic%d", compNum);
                flag = true;
            }
            else if(conNum == 1)
            {
                sprintf(str, "- Ic%d - Ie%d", compNum, compNum);
                flag = false;
            }
            else if(conNum == 2)
            {
                sprintf(str, "Ie%d", compNum);
                flag = true;
            }
            for(temp1 = 0; temp1 < 3; ++temp1)
            {
                temp2 = con[temp1];
                if(temp2 != 0)
                {
                    num[(*cnt)] = temp2;
                    ++(*cnt);
                }
            }
            break;
        case Resistor:
            if(con[1] == 0) sprintf(str, "x%d/%s", nodeNum, name);
            else if(con[0] == nodeNum) sprintf(str, "(x%d - x%d)/%s", nodeNum, con[1], name);
            else sprintf(str, "(x%d - x%d)/%s", nodeNum, con[0], name);
            flag = true;
            for(temp1 = 0; temp1 < 2; ++temp1)
            {
                temp2 = con[temp1];
                if(temp2 != 0)
                {
                    num[(*cnt)] = temp2;
                    ++(*cnt);
                }
            }
            break;
        case VSource:
            if(con[1] == 0)
            {
                sprintf(str, "x%d - %s", nodeNum, name);
                num[0] = nodeNum;
                ++(*cnt);
            }
            else if(con[0] == nodeNum)
            {
                sprintf(str, "x%d - x%d - %s", nodeNum, con[1],name);
                num[0] = nodeNum;
                num[1] = con[1];
                (*cnt) = 2;
            }
            else
            {
                sprintf(str, "- x%d", nodeCount - Counts[VSource][0] - 1 + compNum);
                num[0] = nodeCount - Counts[VSource][0] - 1 + compNum;
                ++(*cnt);
                flag = false;
                break;
            }
            flag =  true;
            break;
    }
    return flag;
}
bool Component::getJacobiFunc(char *str, unsigned conNum, unsigned nodeNum1, unsigned nodeNum2)
{
    bool flag = false;
    switch(type)
    {
        case BJT:
            if(conNum == 0)
            {
                models[model].getJacobiIcFunc(str, Counts[BJT][compNum], nodeNum2);
                flag = true;
            }
            else if(conNum == 1)
            {
                char str1[BufLength], str2[BufLength];
                models[model].getJacobiIcFunc(str1, Counts[BJT][compNum], nodeNum2);
                models[model].getJacobiIeFunc(str2, Counts[BJT][compNum], nodeNum2);
                if(str1[0] != '0' && str2[0] != '0') sprintf(str, "- %s - %s", str1, str2);
                else if(str2[0] != '0') sprintf(str, "- %s", str2);
                else if(str1[0] != '0') sprintf(str, "- %s", str1);
                else str[0] = '0';
                flag = false;
            }
            else if(conNum == 2)
            {
                models[model].getJacobiIeFunc(str, Counts[BJT][compNum], nodeNum2);
                flag = true;
            }
            break;
        case Resistor:
            //if(con[0] == nodeNum2)
            if(nodeNum1 == nodeNum2)
            {
                sprintf(str, "1/%s", name);
                flag = true;
            }
            //else if(con[1] == nodeNum2)
            else if(nodeNum2 == con[0] || nodeNum2 == con[1])
            {
                sprintf(str, "- 1/%s", name);
                flag = false;
            }
            else str[0] = '0';
            break;
        case VSource:
            if(con[0] == nodeNum1 && nodeNum1 == nodeNum2)
            {
                strcpy(str, "1");
                flag = true;
            }
            else if((con[1] == nodeNum2 && nodeNum1 != nodeNum2) || nodeCount - Counts[VSource][0] - 1 + compNum == nodeNum2)
            {
                
                strcpy(str, "- 1");
                flag = false;
            }
            else str[0] = '0';
            break;
    }
    return flag;
}
double Component::getNodalValue(unsigned conNum, unsigned nodeNum)
{
    double val = 0.0;
    switch(type)
    {
        case BJT:
            if(conNum == 0) val = models[model].getNodalIc(Counts[BJT][compNum]);
            else if(conNum == 1) val = - models[model].getNodalIc(Counts[BJT][compNum]) - models[model].getNodalIe(Counts[BJT][compNum]);
            else if(conNum == 2) val = models[model].getNodalIe(Counts[BJT][compNum]);
            break;
        case Resistor:
            if(con[1] == 0) val = nodes[nodeNum].value / value;
            else if(con[0] == nodeNum) val = (nodes[nodeNum].value - nodes[con[1]].value) / value;
            else val = (nodes[nodeNum].value - nodes[con[0]].value) / value;
            break;
        case VSource:
            if(con[1] == 0) val = nodes[nodeNum].value - value;
            else if(con[0] == nodeNum) val = nodes[nodeNum].value - nodes[con[1]].value - value;
            else val = - nodes[nodeCount - Counts[VSource][0] - 1 + compNum].value;
            break;
    }
    return val;
}
double Component::getJacobiValue(unsigned conNum, unsigned nodeNum1, unsigned nodeNum2)
{
    double val = 0.0;
    switch(type)
    {
        case BJT:
            if(conNum == 0) val = models[model].getJacobiIc(Counts[BJT][compNum], nodeNum2);
            else if(conNum == 1) val = - models[model].getJacobiIc(Counts[BJT][compNum], nodeNum2)
                                         - models[model].getJacobiIe(Counts[BJT][compNum], nodeNum2);
            else if(conNum == 2) val = models[model].getJacobiIe(Counts[BJT][compNum], nodeNum2);
            break;
        case Resistor:
            if(nodeNum1 == nodeNum2) val = 1.0 / value;
            else if(nodeNum2 == con[0] || nodeNum2 == con[1]) val = -1.0 / value;
            break;
        case VSource:
            if(con[0] == nodeNum1 && nodeNum1 == nodeNum2) val = 1.0;
            else if((con[1] == nodeNum2 && nodeNum1 != nodeNum2) || nodeCount - Counts[VSource][0] - 1 + compNum == nodeNum2)
                val = -1.0;
            break;
    }
    return val;
}

void Init1()
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

double GetTime(struct timeval &t1, struct timeval &t2)
{
    return (t2.tv_sec - t1.tv_sec) * 1000 + (t2.tv_usec - t1.tv_usec) / 1000.0;
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


void updateX() // 从X保存到节点
{
    for(unsigned a = 1; a < nodeCount; ++a) nodes[a].value = X[a-1];
}
void updateFx()
{
   for(unsigned a = 1; a < nodeCount; ++a) Fx[a - 1] = nodes[a].getNodalValue();
}
void updateJx()
{
    for(unsigned a = 0; a < SpVec.size(); ++a)
        Jx.coeffRef(SpVec[a].row(), SpVec[a].col()) = nodes[SpVec[a].row() + 1].getJacobiValue(SpVec[a].col() + 1);
}

void solveInit()
{
    I.reserve(nodeCount - 1);
    I.resize(nodeCount - 1, nodeCount - 1);
    for(unsigned a = 0; a < nodeCount - 1; ++a) I.coeffRef(a, a) = 1.0;

    Jx.reserve(SpVec.size());
    Jx.resize(nodeCount - 1, nodeCount - 1);
    Jx.setFromTriplets(SpVec.begin(), SpVec.end());
    invJx.resize(nodeCount - 1, nodeCount - 1);

    X.resize(nodeCount - 1);
    for(unsigned a = 1; a < nodeCount; ++a) X[a - 1] = nodes[a].value;
    Xs.push_back(X);
    Fx.resize(nodeCount - 1);
}
void solve(ofstream &outFile)
{
    outFile << endl << "solve start" << endl;
    gettimeofday(&time1, NULL);
    solveInit();
    unsigned iters = 0, utemp1;
    double dtemp1 = tol + 1;
    while(dtemp1 > tol && iters < maxit)
    {
        updateFx();
        updateJx();
        
        if(!Jx.isCompressed()) Jx.makeCompressed();
        luSolver.analyzePattern(Jx);
        luSolver.factorize(Jx);
        invJx = luSolver.solve(I);
        
        X = X - invJx * Fx;
        
        updateX();
        Xs.push_back(X);
        ++iters;
        dtemp1 = (X - Xs[Xs.size()-1]).norm();
        if(iters == 2) break;
    }
    gettimeofday(&time2, NULL);
    outFile << "solve end, use time = " << GetTime(time1, time2) << "ms" << endl;
    outFile << "iters: " << iters << "    tol: " << dtemp1 << endl;
    outFile << "Xs:" << endl;
    for(unsigned a = 0; a < Xs.size(); ++a)
    {
        for(unsigned b = 0; b < nodeCount - 1; ++b)
            outFile << setiosflags(ios::right) << setw(10) << setprecision(4) << Xs[a][b] << ' ';
        outFile << endl;
    }
}
