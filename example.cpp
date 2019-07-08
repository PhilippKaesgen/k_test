#include <iostream>


#include "k_test.h"

using namespace std;
using namespace k_test;

//##############################################################################
class Foo{
	const int divisor;

public:
	Foo(const int d): divisor(d){}

	int bar(const int& arg) const{
		return arg/divisor;
	}

};

//##############################################################################
int foo_1(int arg1, int arg2, int arg3){
  return arg1*arg2*arg3;
}

//##############################################################################
template<size_t S>
int foo_2(unsigned arg){
  return arg*S;
}

//##############################################################################

void foo_3(int& arg){
	arg*=3;
}

//##############################################################################

template<typename T>
int foo_4(T&& arg){
  return arg*4;
}

//##############################################################################

int main(){


  OPEN_TEST_ENV(cout,"k_test demo")

  test(cout,"probing the return type",&foo_1,
    args(1,2,3),6,
    args(2,3,4),24);

  test<std::not_equal_to>(cout,"'not equal'",&foo_1,
    args(1,2,3),5,
    args(2,3,4),20);

  test(cout,"failing test",&foo_1,
    args(1,2,3),6,
    args(2,3,4),20);

  test(cout,"templated function",&foo_2<2>,
    args(2u),4,                         //mind the argument type!
    args(3u),6);

    int var=3;
  test(cout,"call-by-reference assertion",&foo_3,
    args(var),var,9,
    args(var),var,27);

  test(cout,"rvalue reference",&foo_4<int&>,
    args(4),16,
    args(5),20);

  test<Foo>(cout,"class member function",args(5),&Foo::bar,
    args(5),1,
    args(6),1,
    args(10),2);

  CLOSE_TEST_ENV(cout)

  return 0;
}
