#include "parser.h"

int main( int argc, char *argv[] )
{
    ifstream inFile;
    ofstream outFile;
    bool flag;
    char inName[NameLength], outName[NameLength], buf[BufLength], title[BufLength],
      buf1[BufLength], buf2[BufLength], buf3[BufLength], nameBuf[NameLength], *charPtr1;
    unsigned intBuf1, intBuf2, intBuf3, datum=NA, tempNum[CompTypeCount], conIn[4], cnts[3], tempcnts1=0, tempcnts2=0; 
    unsigned *nodeTempNum=NULL;
    double douBuf1, douBuf2, douBuf3, douBuf4, douBuf5;
    CompType typeBuf;
    TranType TtypeBuf;
    memset(tempNum, 0, sizeof(unsigned) * (CompTypeCount));

    if(argc < 3)
    {
        cerr << "./parser infile outfile" << endl;
        exit(1);
    }

    // 读取输入和输出的文件位置
    strcpy(inName, argv[1]);
    strcpy(outName, argv[2]);

    Init(); // 初始化

    // 设置输入文件地址
    inFile.open( inName, ios::in );
    while( !inFile )
    {
        cerr << inName << " is an invalid input file" << endl;
        exit(1);
    }
    inFile.getline(title, BufLength); // 读取Titile
    // 设置输出文件地址
    outFile.open( outName, ios::out );
    cout << "Output saved to file: " << outName << endl;
    // 第一次遍历文件 获得Model个数、节点个数和器件个数
    inFile.getline(buf, BufLength);
    while(inFile.good())
    {
        if((buf == NULL ) || (*buf == '\0'))
        {
            inFile.getline(buf, BufLength);
            continue;
        }
        strcpy(buf1, buf);
        if(!strcmp(strtok(buf1, " "), ".model")) ++modelCount;
        else if(isalpha(*buf))
        {
            switch(*buf)
            {
                
                case 'v':
                case 'V':
                    sscanf(buf, "%s %u %u", nameBuf, &intBuf1, &intBuf2);
                    if(max(intBuf1, intBuf2) + 1 > nodeCount) nodeCount = max(intBuf1, intBuf2) + 1;
                    ++componentCount;
                    ++Counts[VSource][0];
                    break;
                case 'i':
                case 'I':
                    sscanf(buf, "%s %u %u", nameBuf, &intBuf1, &intBuf2);
                    if(max(intBuf1, intBuf2) + 1 > nodeCount) nodeCount = max(intBuf1, intBuf2) + 1;
                    ++componentCount;
                    ++Counts[ISource][0];
                    break;
                case 'q':
                case 'Q':
                    sscanf(buf, "%s %u %u %u", nameBuf, &intBuf1, &intBuf2, &intBuf3);
                    intBuf1 = max(intBuf1, intBuf2);
                    if(max(intBuf1, intBuf3) + 1 > nodeCount) nodeCount = max(intBuf1, intBuf3) + 1;
                    ++componentCount;
                    ++Counts[BJT][0];
                    break;
                case 'r':
                case 'R':
                    sscanf(buf, "%s %u %u", nameBuf, &intBuf1, &intBuf2);
                    if(max(intBuf1, intBuf2) + 1 > nodeCount) nodeCount = max(intBuf1, intBuf2) + 1;
                    ++componentCount;
                    ++Counts[Resistor][0];
                    break;
            };
        }
        inFile.getline(buf, BufLength);
    }
    intBuf1 = nodeCount;
    nodeCount += Counts[VSource][0];
    nodeTempNum = (unsigned *)malloc(sizeof(unsigned) * nodeCount); memset(nodeTempNum, 0, sizeof(unsigned) * nodeCount);
    models = (Model *)malloc(sizeof(Model) * modelCount);
    components = (Component *)malloc(sizeof(Component) * componentCount); for(unsigned a = 0; a < componentCount; ++a) components[a].init();
    nodes = (Node *)malloc(sizeof(Node) * nodeCount); for(unsigned a = 0; a < nodeCount; ++a) nodes[a].init(a);
    for(intBuf2 = 0; intBuf2 < intBuf1; ++intBuf2) nodes[intBuf2].type = Nodal;
    for(; intBuf2 < nodeCount; ++intBuf2) nodes[intBuf2].type = Modified;
    for(unsigned a = 0, temp; a < CompTypeCount; ++a)
    {
        temp = Counts[a][0];
        free(Counts[a]);
        Counts[a] = (unsigned *)malloc(sizeof(unsigned) * (temp + 1));
        Counts[a][0] = temp;
    }

    inFile.seekg(ios::beg); // 返回文件起点
    // 第二次遍历文件 1.添加模型、器件 2.记录节点中器件的数量 3.初始化模型 器件 或节点
    inFile.getline(buf, BufLength);
    while(inFile.good())
    {
        if((buf == NULL ) || (*buf == '\0') || (*buf == '*'))
        {
            inFile.getline(buf, BufLength);
            continue;
        }
        strcpy(buf1, buf);
        charPtr1 = strtok(buf1, " ");
        if(!strcmp(charPtr1, ".model"))
        {
            flag = false;
            strcpy(buf2, strtok(NULL, " "));
            charPtr1 = strtok(NULL, " ");
            if(!strcmp(charPtr1, "PNP" )) TtypeBuf = PNP;
            else if(!strcmp(charPtr1, "NPN")) TtypeBuf = NPN;
            charPtr1 = strtok(NULL, " ");
            while (charPtr1 != NULL)
            {
                if((charPtr1[0] == 'I') && (charPtr1[1] == 'S') && (charPtr1[2] == '='))
                    douBuf1 = stripString(charPtr1);
                if((charPtr1[0] == 'B') && (charPtr1[1] == 'F') && (charPtr1[2] == '='))
                    douBuf2 = stripString(charPtr1);
                if((charPtr1[0] == 'B') && (charPtr1[1] == 'R') && (charPtr1[2] == '='))
                    douBuf3 = stripString(charPtr1);
                if((charPtr1[0] == 'T') && (charPtr1[1] == 'E') && (charPtr1[2] == '=') && !flag)
                {
                    douBuf4 = stripString(charPtr1);
                    douBuf5 = Q / (K * douBuf4);
                    flag = true;
                }
                if((charPtr1[0] == 'N') && (charPtr1[1] == '=') && !flag)
                {
                    douBuf5 = stripString(charPtr1);
                    douBuf4 = Q / (K * douBuf5);
                    flag = true;
                }
                charPtr1 = strtok( NULL, " " );
            }
            models[tempcnts1].InitModel(buf2, TtypeBuf,
                douBuf1, douBuf2, douBuf3, douBuf4, douBuf5);
            ++tempcnts1;
        }
        else if(!strcmp(charPtr1, ".nodeset"))
        {
            charPtr1 = strtok(NULL, " ");
            while (charPtr1 != NULL)
            {
                sscanf(charPtr1, "v(%u)=%lf", &intBuf1, &douBuf1);
                nodes[intBuf1].value = douBuf1;
                charPtr1 = strtok(NULL, " ");
            }
        }
        else if(isalpha(*buf))
        {
            strcpy(buf2, "0");
            switch( *buf )
            {
                case 'v':
                case 'V':
                    sscanf(buf, "%s %u %u %s", nameBuf, &conIn[0], &conIn[1], buf2);
                    intBuf1 = 2;
                    typeBuf = VSource;
                    ++tempNum[VSource];
                    intBuf2 = NA;
                    break;
                case 'i':
                case 'I':
                    sscanf(buf, "%s %u %u %s", nameBuf, &conIn[0], &conIn[1], buf2);
                    intBuf1 = 2;
                    typeBuf = ISource;
                    ++tempNum[ISource];
                    intBuf2 = NA;
                    break;
                case 'r':
                case 'R':
                    sscanf(buf, "%s %u %u %s", nameBuf, &conIn[0], &conIn[1], buf2);
                    intBuf1 = 2;
                    typeBuf = Resistor;
                    ++tempNum[Resistor];
                    intBuf2 = NA;
                    break;
                case 'q':
                case 'Q':
                    sscanf(buf, "%s %u %u %u %s", nameBuf, &conIn[0], &conIn[1], &conIn[2], buf2);
                    intBuf1 = 3;
                    typeBuf = BJT;
                    ++tempNum[BJT];
                    for(unsigned a = 0; a < tempcnts1; ++a)
                    {
                        if(!strcmp(models[a].name, buf2))
                        {
                            intBuf2 = a;
                            break;
                        }
                    }
                    break;
            };
            for(unsigned a = 0; a < intBuf1; ++a) ++nodes[conIn[a]].compLinkCnt;
            Counts[typeBuf][tempNum[typeBuf]] = tempcnts2;
            components[tempcnts2].InitComponent(typeBuf, conIn, intBuf1, tempNum[typeBuf], atof(buf2), nameBuf, intBuf2);
            ++tempcnts2;
        }
        inFile.getline(buf, BufLength);
    }
    inFile.close();

    for(unsigned a = 0; a < nodeCount - Counts[VSource][0]; ++a)
    {
        nodes[a].comp = (unsigned *)malloc(sizeof(unsigned) * nodes[a].compLinkCnt);
        nodes[a].con  = (unsigned *)malloc(sizeof(unsigned) * nodes[a].compLinkCnt);
    }
    for(unsigned a = 0; a < componentCount; ++a) // Nodal
    {
        for(unsigned b = 0; b < components[a].nodeLinkCnt; ++b)
        {
            intBuf1 = components[a].con[b];
            nodes[intBuf1].comp[nodeTempNum[intBuf1]] = a;
            nodes[intBuf1].con[nodeTempNum[intBuf1]] = b;
            ++nodeTempNum[intBuf1];
        }
    }
//  Loop to find the datum
    if( datum == NA )
    {
        intBuf1 = 1;
        for(unsigned a = 2; a < nodeCount - Counts[VSource][0]; ++a)
        {
            if(nodes[a].compLinkCnt > nodes[intBuf1].compLinkCnt) intBuf1 = a;
        }
        datum = intBuf1;
    }
// 打印器件和节点信息
    printComponents();
    printNodes();

    outFile << "Title: " << title << endl;
    outFile << "datum = " << datum << "          " << "lastnode = " << nodeCount - Counts[VSource][0] - 1 << endl;

    outFile << endl << "器件值:" << endl;
    for(unsigned a = 0; a < componentCount; ++a) components[a].printValue(outFile);
    outFile << endl << "节点信息:" << endl;
    for(unsigned a = 1; a < nodeCount - Counts[VSource][0]; ++a) nodes[a].Myprint1(outFile);
    NodalFunc(outFile);
    outFile.close();
    Destroy();
    free(nodeTempNum);
    cout << "Finish!" << endl;
    return 0;
}