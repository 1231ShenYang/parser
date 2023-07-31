# parser
## 演示视频位置
```
https://www.bilibili.com/video/BV1vx4y1X7PZ/
```
## run
```
将网表放在../parser/data/中
bash ./run.sh
如: bash ./run.sh
```
## 2023.7.11
```
完成相应输出
Title: * test tran
datum = 1          lastnode = 2
节点1        所连器件数为:2
    类型:VSource    连接端口:0    名称:VCC    value:3
    类型:Resistor    连接端口:0    名称:R1    value:1000
节点0        所连器件数为:2
    类型:VSource    连接端口:1    名称:VCC    value:3
    类型:Capacitor    连接端口:1    名称:C1    value:1e-05
节点2        所连器件数为:2
    类型:Resistor    连接端口:1    名称:R1    value:1000
    类型:Capacitor    连接端口:0    名称:C1    value:1e-05
```
## 2023.7.20
### case1
```
Title: * Schmitt Trigger circuit no. 1
datum = 1          lastnode = 6
节点1        所连器件数为:3
    类型:BJT    连接端口:0    名称:q1    编号:1    value:-1
    类型:Resistor    连接端口:1    名称:rc1    编号:1    value:2000
    类型:Resistor    连接端口:0    名称:r3    编号:3    value:10000
节点5        所连器件数为:2
    类型:BJT    连接端口:1    名称:q1    编号:1    value:-1
    类型:VSource    连接端口:0    名称:vin    编号:2    value:1.5
节点2        所连器件数为:3
    类型:BJT    连接端口:2    名称:q1    编号:1    value:-1
    类型:BJT    连接端口:2    名称:q2    编号:2    value:-1
    类型:Resistor    连接端口:0    名称:re    编号:4    value:100
节点3        所连器件数为:2
    类型:BJT    连接端口:0    名称:q2    编号:2    value:-1
    类型:Resistor    连接端口:1    名称:rc2    编号:2    value:1000
节点4        所连器件数为:2
    类型:BJT    连接端口:1    名称:q2    编号:2    value:-1
    类型:Resistor    连接端口:1    名称:r3    编号:3    value:10000
节点6        所连器件数为:3
    类型:Resistor    连接端口:0    名称:rc1    编号:1    value:2000
    类型:Resistor    连接端口:0    名称:rc2    编号:2    value:1000
    类型:VSource    连接端口:0    名称:vcc    编号:1    value:10
节点0        所连器件数为:3
    类型:Resistor    连接端口:1    名称:re    编号:4    value:100
    类型:VSource    连接端口:1    名称:vcc    编号:1    value:10
    类型:VSource    连接端口:1    名称:vin    编号:2    value:1.5

Functions:
F(1) = Ic1 + (x1 - x6)/rc1 + (x1 - x4)/r3
F(2) = Ie1 + Ie2 + x2/re
F(3) = Ic2 + (x3 - x6)/rc2
F(4) =  - Ic2 - Ie2 + (x4 - x1)/r3
F(5) = x5 - vin
F(6) = x6 - vcc
F(7) = x7  + (x6 - x1)/rc1 + (x6 - x3)/rc2
F(8) = x8  - Ic1 - Ie1
```
### case2
```
Title: * Schmitt Trigger circuit no. 2
datum = 6          lastnode = 6
节点2        所连器件数为:3
    类型:BJT    连接端口:0    名称:q1    编号:1    value:-1
    类型:Resistor    连接端口:1    名称:rc1    编号:1    value:1500
    类型:Resistor    连接端口:0    名称:r1    编号:3    value:10000
节点5        所连器件数为:3
    类型:BJT    连接端口:1    名称:q1    编号:1    value:-1
    类型:Resistor    连接端口:0    名称:r2    编号:5    value:5000
    类型:Resistor    连接端口:0    名称:r3    编号:6    value:1250
节点1        所连器件数为:3
    类型:BJT    连接端口:2    名称:q1    编号:1    value:-1
    类型:BJT    连接端口:2    名称:q2    编号:2    value:-1
    类型:Resistor    连接端口:0    名称:re    编号:4    value:100
节点3        所连器件数为:2
    类型:BJT    连接端口:0    名称:q2    编号:2    value:-1
    类型:Resistor    连接端口:1    名称:rc2    编号:2    value:1000
节点4        所连器件数为:3
    类型:BJT    连接端口:1    名称:q2    编号:2    value:-1
    类型:Resistor    连接端口:1    名称:r1    编号:3    value:10000
    类型:Resistor    连接端口:0    名称:r4    编号:7    value:1e+06
节点6        所连器件数为:4
    类型:Resistor    连接端口:0    名称:rc1    编号:1    value:1500
    类型:Resistor    连接端口:0    名称:rc2    编号:2    value:1000
    类型:Resistor    连接端口:1    名称:r2    编号:5    value:5000
    类型:VSource    连接端口:0    名称:vcc    编号:1    value:10
节点0        所连器件数为:4
    类型:Resistor    连接端口:1    名称:re    编号:4    value:100
    类型:Resistor    连接端口:1    名称:r3    编号:6    value:1250
    类型:Resistor    连接端口:1    名称:r4    编号:7    value:1e+06
    类型:VSource    连接端口:1    名称:vcc    编号:1    value:10

Functions:
F(1) = Ie1 + Ie2 + x1/re
F(2) = Ic1 + (x2 - x6)/rc1 + (x2 - x4)/r1
F(3) = Ic2 + (x3 - x6)/rc2
F(4) =  - Ic2 - Ie2 + (x4 - x2)/r1 + x4/r4
F(5) =  - Ic1 - Ie1 + (x5 - x6)/r2 + x5/r3
F(6) = x6 - vcc
F(7) = x7  + (x6 - x2)/rc1 + (x6 - x3)/rc2 + (x6 - x5)/r2
```
### case3
```
Title: * Chua Circuit
datum = 0          lastnode = 14
节点13        所连器件数为:3
    类型:VSource    连接端口:0    名称:VCC1    编号:1    value:12
    类型:Resistor    连接端口:1    名称:R2    编号:2    value:4000
    类型:Resistor    连接端口:0    名称:R11    编号:11    value:4000
节点0        所连器件数为:7
    类型:VSource    连接端口:1    名称:VCC1    编号:1    value:12
    类型:VSource    连接端口:1    名称:VCC2    编号:2    value:12
    类型:Resistor    连接端口:1    名称:R4    编号:4    value:5000
    类型:Resistor    连接端口:1    名称:R6    编号:6    value:500
    类型:Resistor    连接端口:1    名称:R7    编号:7    value:500
    类型:Resistor    连接端口:1    名称:R9    编号:9    value:10100
    类型:Resistor    连接端口:1    名称:R10    编号:10    value:10100
节点14        所连器件数为:3
    类型:VSource    连接端口:0    名称:VCC2    编号:2    value:12
    类型:Resistor    连接端口:1    名称:R3    编号:3    value:4000
    类型:Resistor    连接端口:0    名称:R12    编号:12    value:4000
节点1        所连器件数为:4
    类型:VSource    连接端口:0    名称:V1    编号:3    value:10
    类型:Resistor    连接端口:0    名称:R4    编号:4    value:5000
    类型:BJT    连接端口:1    名称:Q1    编号:1    value:-1
    类型:BJT    连接端口:1    名称:Q3    编号:3    value:-1
节点2        所连器件数为:2
    类型:VSource    连接端口:1    名称:V1    编号:3    value:10
    类型:Resistor    连接端口:0    名称:R1    编号:1    value:10000
节点11        所连器件数为:2
    类型:VSource    连接端口:0    名称:V2    编号:4    value:2
    类型:Resistor    连接端口:1    名称:R14    编号:14    value:30000
节点3        所连器件数为:3
    类型:VSource    连接端口:1    名称:V2    编号:4    value:2
    类型:Resistor    连接端口:1    名称:R1    编号:1    value:10000
    类型:Resistor    连接端口:1    名称:R13    编号:13    value:30000
节点4        所连器件数为:3
    类型:Resistor    连接端口:0    名称:R2    编号:2    value:4000
    类型:Resistor    连接端口:0    名称:R5    编号:5    value:30000
    类型:BJT    连接端口:0    名称:Q1    编号:1    value:-1
节点5        所连器件数为:3
    类型:Resistor    连接端口:0    名称:R3    编号:3    value:4000
    类型:Resistor    连接端口:0    名称:R8    编号:8    value:30000
    类型:BJT    连接端口:0    名称:Q3    编号:3    value:-1
节点6        所连器件数为:3
    类型:Resistor    连接端口:1    名称:R5    编号:5    value:30000
    类型:Resistor    连接端口:0    名称:R9    编号:9    value:10100
    类型:BJT    连接端口:1    名称:Q2    编号:2    value:-1
节点8        所连器件数为:3
    类型:Resistor    连接端口:0    名称:R6    编号:6    value:500
    类型:BJT    连接端口:2    名称:Q1    编号:1    value:-1
    类型:BJT    连接端口:2    名称:Q2    编号:2    value:-1
节点9        所连器件数为:3
    类型:Resistor    连接端口:0    名称:R7    编号:7    value:500
    类型:BJT    连接端口:2    名称:Q3    编号:3    value:-1
    类型:BJT    连接端口:2    名称:Q4    编号:4    value:-1
节点7        所连器件数为:3
    类型:Resistor    连接端口:1    名称:R8    编号:8    value:30000
    类型:Resistor    连接端口:0    名称:R10    编号:10    value:10100
    类型:BJT    连接端口:1    名称:Q4    编号:4    value:-1
节点10        所连器件数为:3
    类型:Resistor    连接端口:1    名称:R11    编号:11    value:4000
    类型:Resistor    连接端口:0    名称:R13    编号:13    value:30000
    类型:BJT    连接端口:0    名称:Q2    编号:2    value:-1
节点12        所连器件数为:3
    类型:Resistor    连接端口:1    名称:R12    编号:12    value:4000
    类型:Resistor    连接端口:0    名称:R14    编号:14    value:30000
    类型:BJT    连接端口:0    名称:Q4    编号:4    value:-1

Functions:
F(1) = x1 - x2 - V1
F(2) =  - x17 + (x2 - x3)/R1
F(3) =  - x18 + (x3 - x2)/R1 + (x3 - x10)/R13
F(4) = (x4 - x13)/R2 + (x4 - x6)/R5 + Ic1
F(5) = (x5 - x14)/R3 + (x5 - x7)/R8 + Ic3
F(6) = (x6 - x4)/R5 + x6/R9 - Ic2 - Ie2
F(7) = (x7 - x5)/R8 + x7/R10 - Ic4 - Ie4
F(8) = x8/R6 + Ie1 + Ie2
F(9) = x9/R7 + Ie3 + Ie4
F(10) = (x10 - x13)/R11 + (x10 - x3)/R13 + Ic2
F(11) = x11 - x3 - V2
F(12) = (x12 - x14)/R12 + (x12 - x11)/R14 + Ic4
F(13) = x13 - VCC1
F(14) = x14 - VCC2
F(15) = x15  + (x13 - x4)/R2 + (x13 - x10)/R11
F(16) = x16  + (x14 - x5)/R3 + (x14 - x12)/R12
F(17) = x17  + x1/R4 - Ic1 - Ie1 - Ic3 - Ie3
F(18) = x18  + (x11 - x12)/R14
```
### case4
```
Title: * 测试tran的电路
datum = 1          lastnode = 2
节点1        所连器件数为:2
    类型:VSource    连接端口:0    名称:VCC    编号:1    value:3
    类型:Resistor    连接端口:0    名称:R1    编号:1    value:1000
节点0        所连器件数为:2
    类型:VSource    连接端口:1    名称:VCC    编号:1    value:3
    类型:Capacitor    连接端口:1    名称:C1    编号:1    value:1e-05
节点2        所连器件数为:2
    类型:Resistor    连接端口:1    名称:R1    编号:1    value:1000
    类型:Capacitor    连接端口:0    名称:C1    编号:1    value:1e-05

Functions:
F(1) = x1 - VCC
F(2) = (x2 - x1)/R1
F(3) = x3  + (x1 - x2)/R1
```
## 2023.7.30
```
添加了Jacobi函数的输出和N-R迭代法
```