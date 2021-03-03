#include<iostream>
using namespace std;

// #用来把参数转换成字符串
#define P(A) printf("%s:%d\n", #A, A);
#define XNAME(n) x##n /* ## 这个运算符把两个语言符号组合成单个语言符号*/
#define PXN(n) printf("x"#n" = %d\n",x##n)

#define DEBUG1(format, ...) printf(format, __VA_ARGS__); 
#define DEBUG2(format, args...) printf(format, ##args);
#define DEBUG3(format, ...) printf(format, ##__VA_ARGS__);
 

int main()
{
    int a = 1, b = 2;
    P(a);
    P(b);
    P(a+b);
    cout<<endl;

    int XNAME(1) = 10886; /*宏展开就是：x1 = 10086*/
    PXN(1); /*宏展开就是：printf("x1 = %d\n",x1) */
    cout<<endl;


    printf("hello world.1 \n");
    // DEBUG1("hello world.2\n");//错误 参数为零
    DEBUG1("hello world.2 %d %d\n", 1, 2);
    DEBUG2("hello world.3\n");
    DEBUG2("hello world.3 %d %d %d\n", 1, 2, 3);
    
    DEBUG3("hello world.4\n");
    DEBUG3("hello world.4 %d %d %d %d\n", 1, 2, 3, 4);
    return 0;
}