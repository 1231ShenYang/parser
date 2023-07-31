cmake .
make -j 24
# ./parser ../parser/data/$1 ../parser/data/$1.out
./parser ../parser/data/case1 ../parser/data/case1.out
clear
./parser ../parser/data/case2 ../parser/data/case2.out
clear
./parser ../parser/data/case3 ../parser/data/case3.out