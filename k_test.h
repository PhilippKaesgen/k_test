/*
Copyright 2019 Philipp S. Kaesgen

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software without
   specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


/**
  * Light-weight K_Test-Framework.
  *
  * Compile with C++17 features.
  *
  * Author: Philipp S. Kaesgen
  * Date: July 8th, 2019
  *
  *
  *
  * TODO:
  *
  *
  * o Test generator: Test cases are only defined by a keyword in the comment
  *   section immediately above a function to be tested in the header file, and
  *   pairs of stimuli and reference values. A parser afterwards gathers these
  *   pairs, writes a test program with a main function, sets up the test
  *   environment, uses the test functions accordingly, and finally compiles and
  *   runs the binary.
  *
**/
#ifndef K_TEST_H
#define K_TEST_H

#include <functional>
#include <tuple>
#include <iostream>
#include <iomanip>



/**#############################################################################
  * Test Environment
  *
  * If the user wants to have an overall evaluation of the tests, the tests have
  * to be executed within the test environment, and state to which outputstream
  * the results shall be output. If no test environment is opened, testing
  * can still be executed.
  *
  * Returns error code.
  *
  * Minimal example:
  *
  * //...
  *
  * int main(){
  *   //...
  *
  *   OPEN_TEST_ENV(std::cout,"My Test  Suite")
  *
  *   test(std::cout, "Test 1", &foo, args(1,2), 3)
  *
  *   //...
  *
  *   CLOSE_TEST_ENV(std::cout)
  *   //...
  *
  * }
  *
  *#############################################################################
**/


//Change this constant when your test names are very long-----------------------
#define TEST_NAME_LENGTH 65

//These variables keep track of the test scores---------------------------------
static size_t Test_Counter=0, Passed_Test=0;

//Boolean indicating whether a test environment is opened or not----------------
static bool inTestEnv=false;


//Makro for opening a test environment; first argument is an output stream,-----
//second argument is the user defined name of the test suite
#define OPEN_TEST_ENV(OS, N) OS<<std::endl<<std::setfill('_')<<std::setw(TEST_NAME_LENGTH +6)<<'_'<<std::endl\
  <<N<<std::endl<<std::setfill('~')<<std::setw(TEST_NAME_LENGTH +6)<<'~'<<std::endl;\
  inTestEnv=true;

//Makro for closing a previously opened test environment; the argument is an----
//output stream. The test environment has to be closed in order to get an
//evaluation of your tests!
#define CLOSE_TEST_ENV(OS) OS<<std::setfill('~')<<std::setw(TEST_NAME_LENGTH +6)<<'~'<<std::endl\
  <<"Summary:\033[1;36m "<<Passed_Test<<"/"<<Test_Counter<<" ("\
  <<((float)(Passed_Test*100)/Test_Counter)<<"%) of the tests passed\033[0m"<<std::endl\
  <<std::setfill('\"')<<std::setw(TEST_NAME_LENGTH +6)<<'\"'<<std::endl<<std::endl;\
  Test_Counter=0;\
  Passed_Test=0;\
  inTestEnv=false;


/**#############################################################################
  * K_Test namespace
  *#############################################################################
**/

namespace k_test{

  //------------------------------------------------------------------------------
/*
  template<typename T1, typename T2>
  static constexpr bool is_convertible_v(){ return std::is_convertible_v<T1,T2>;}

  template<typename T1, typename... T1s, typename T2, typename... T2s>
  static constexpr std::enable_if_t<sizeof...(T1s)==sizeof...(T2s),
  bool> is_convertible_v(){
  return std::is_convertible_v<T1,T2>&&is_convertible_v<T1s...,T2s...>();
  }*/


/**#############################################################################
  *  Calls function f with arguments args and compares the result with value r.
  * If the comparison of result and r yields true and the same applies for each
  * consecutive values in rest..., probe returns the tuple with the throwing
  * pair of args and r, along side the index of that pair.
  *
  * Parameters:
  * COMP    function object   binary function yielding a boolean, e.g. std::less
  *
  * Arguments:
  * f       function pointer  ...to function under test
  * args    tuple             ...of arguments for stimulating the FUT; can be
  *                           generated with std::forward_as_tuple() or
  *                           k_test::args(); the size of the tuple has to
  *                           match the number of FUT's arguments
  * r       return value of f reference value for the result of FUT when called
                              with the foregoing tuple of arguments
  * rest... more args & r     more tuples of arguments and reference values
  *
  * Returns tuple             ...of first: number of passed probes; if
  *                           this number is equal to the overall number of
  *                           probes, the probe was successful;
  *                           second: reference value of the failed probe;
  *                           third:  result by f of the failed probe
  *
  * ----------------------------------------------------------------------------
  *
  * Additional Info:
  *
  * ----------------------------------------------------------------------------
  * If function foo of type F accepts templated rvalue references, e.g.
  *
  * template<typename ARG> foo(ARG&& arg);
  *
  * call this function with a lvalue reference specialization, e.g.
  *
  * probe(&foo<int&>, std::forward_as_tuple(4),5);
  *
  *#############################################################################
**/

template<template<typename> typename COMP=std::equal_to, typename F, typename... F_ARGS, typename... T>
static std::enable_if_t<
  !std::is_same_v<void,std::result_of_t<F(F_ARGS&...)>>&&sizeof...(T)==0,
  std::tuple<size_t,std::result_of_t<F(F_ARGS&...)>,std::result_of_t<F(F_ARGS&...)>>>
  probe(F&& f, std::tuple<F_ARGS&...>&& args, std::result_of_t<F(F_ARGS&...)>&& r){
  return std::make_tuple( std::apply ( COMP<typename std::result_of_t< F(F_ARGS&...) > >(),                                   //C++17!!!
  std::forward_as_tuple( std::apply( std::forward<F>(f), std::move(args) ),std::move(r) )),
   std::apply( std::forward<F>(f), std::move(args) ),
   std::move(r) );
}


template<template<typename> typename COMP=std::equal_to, typename F, typename... F_ARGS, typename... T>
static std::enable_if_t<
  sizeof...(T)!=0,
  std::tuple<size_t,std::result_of_t<F(F_ARGS&...)>,std::result_of_t<F(F_ARGS&...)>>>
  probe(F&& f, std::tuple<F_ARGS&...>&& args, std::result_of_t<F(F_ARGS&...)>&& r, T&&... rest){
  if (std::apply(COMP<typename std::result_of_t< F(F_ARGS&...) > >( ),                                   //C++17!!!
  std::forward_as_tuple( std::apply( std::forward<F>(f), std::move(args) ),std::move(r)
  ))) {
  auto ret = probe<COMP> (std::forward<F>(f), std::forward<T>(rest)... );
  std::get<0>(ret)++;
  return ret;
  }
  else
  return std::make_tuple( 0 ,std::apply( std::forward<F>(f), std::move(args) ),std::move(r));
}


template<template<typename> typename COMP=std::equal_to, typename F, typename R, typename... F_ARGS, typename... T>
static inline std::enable_if_t<
  !std::is_same_v<R,std::result_of_t<F(F_ARGS&...)>>&&std::is_convertible_v<R,std::result_of_t<F(F_ARGS&...)>>,
  std::tuple<size_t,std::result_of_t<F(F_ARGS&...)>,std::result_of_t<F(F_ARGS&...)>>
  >
  probe(F&& f, std::tuple<F_ARGS&...>&& args, R&& r, T&&... rest){
  return probe<COMP>(std::forward<F>(f),
  std::forward<std::tuple<F_ARGS&...>>(args),
  std::forward<std::result_of_t<F(F_ARGS&...)>>(r),
  std::forward<T>(rest)...);
}

//------------------------------------------------------------------------------

template<template<typename> typename COMP=std::equal_to, typename F, typename R, typename... F_ARGS, typename... T>
static std::enable_if_t<
  std::is_lvalue_reference<R&>::value&&sizeof...(T)==0,
  std::tuple<size_t,R,R>>
  probe(F&& f, std::tuple<F_ARGS&...>&& args, R& p, R&& r){
  std::apply( std::forward<F>(f), std::move(args) );

  return std::make_tuple( std::apply ( COMP<R >(), std::forward_as_tuple( p,std::move(r) )),                                  //C++17!!!
   p,
   std::move(r) );
}


template<template<typename> typename COMP=std::equal_to, typename F, typename R, typename... F_ARGS, typename... T>
static std::enable_if_t<
  std::is_lvalue_reference<R&>::value&&sizeof...(T)!=0,
  std::tuple<size_t,R,R>>
  probe(F&& f, std::tuple<F_ARGS&...>&& args, R& p, R&& r, T&&... rest){
  std::apply( std::forward<F>(f), std::move(args) );

  if (std::apply(COMP<R>( ),std::forward_as_tuple( p,std::move(r)))) {
  auto ret = probe<COMP> (std::forward<F>(f), std::forward<T>(rest)... );
  std::get<0>(ret)++;
  return ret;
  }
  else
  return std::make_tuple( 0 ,p,std::move(r));
}



//------------------------------------------------------------------------------

//Function assert() based on function probe()
template<template<typename> typename COMP=std::equal_to, typename F, typename R, typename... F_ARGS, typename... T>
static inline bool assert(F&& f, std::tuple<F_ARGS&...>&& args, R&& r, T&&... rest){
  auto t = probe(std::forward<F>(f), std::forward<std::tuple<F_ARGS&...>> (args), std::forward<R>(r), std::forward<T>(rest)...);
  return std::get<0>(t)==(sizeof... (rest)/2+1);
}


//Function args() for basically converting an arbitrary number of arguments into
//a tuple
template<typename... Types>
static inline std::tuple<Types&&...> args(Types&&... args) noexcept{
  return std::forward_as_tuple(std::move(args)...);
}




/**
  * Function test()
  * For testing a given function with tuples of arguments and comparing the
  * results with the respective reference values.
  *
  * Parameters:
  * COMP    function object    binary function returning a boolean, e.g.
  *                            std::less, std::not_equal_to
  *
  * Arguments:
  * os      output stream      only effective when OPEN_TEST_ENV has been called
  *                            before
  * name    string             name of the test being performed; only effective
  *                            when OPEN_TEST_ENV has been called before
  * f       function pointer   function under test (FUT)
  * args    tuple              ...of arguments for stimulating the FUT; can be
  *                            generated with std::forward_as_tuple() or
  *                            k_test::args(); the size of the tuple has to
  *                            match the number of FUT's arguments
  * r       result type of FUT reference value for the result of FUT when called
                               with the foregoing tuple of arguments
  * rest... more args & r      more tuples of arguments and reference values
**/

/*template<typename... T>
static int test(std::ostream& os, const char* name, T...){
  std::string header("\033[90mTest\033[0m ");
  header+=name;
  os<<std::setfill('.')<<std::setw(TEST_NAME_LENGTH+9)<<std::left<<header
  <<std::setw(9)<<"\033[1;31mfailed\nerror: wrong call of function 'test'\n  probably the argument types in function 'args' don't match the\n  function-under-test's signature."<<std::endl;
  return 1;
}*/

template<template<typename> typename COMP=std::equal_to, typename F, typename R, typename... F_ARGS, typename... T>
static std::enable_if_t<
  std::is_convertible_v<R,std::result_of_t<F(F_ARGS&...)>>,
  int
  >
  test(std::ostream& os, const char* name, F&& f, std::tuple<F_ARGS&...>&& args, R&& r, T&&... rest){
  std::string header("\033[90mTest\033[0m ");
  header+=name;
  if (inTestEnv)  Test_Counter++;
  if (header.length()>TEST_NAME_LENGTH) {header.resize(TEST_NAME_LENGTH-3); }
  auto ret_tup = probe<COMP>(std::forward<F>(f), std::forward<std::tuple<F_ARGS&...>>(args),std::forward<R>(r), std::forward<T>(rest)...);
  if (std::get<0>(ret_tup)==(sizeof... (rest)/2+1)){
  os<<std::setfill('.')<<std::setw(TEST_NAME_LENGTH+9)<<std::left<<header<<std::setw(9)<<"\033[1;32mpassed\033[0m"<<std::endl;
  if (inTestEnv)  Passed_Test++;
  return 0;
  }
  else{
  os<<std::setfill('.')<<std::setw(TEST_NAME_LENGTH+9)<<std::left<<header
  <<std::setw(9)<<"\033[1;31mfailed\n(test case "<<std::get<0>(ret_tup)
  <<": "<<std::get<1>(ret_tup)<<" returned instead of "<<std::get<2>(ret_tup)
  <<")\033[0m"<<std::endl;
  return 1;
  }
}



/*##############################################################################
  #Class handling
##############################################################################*/

/**
  * as from https://stackoverflow.com/questions/19691934/passing-member-function-with-all-arguments-to-stdfunction
  * last access: July 2nd, 2019
**/
template<typename F, typename C>
auto smart_bind(F f, C&& c){
  return [c, f](auto&&... args) {
  return (std::move(c).*f)(std::forward<decltype(args)>(args)...);
  };
}


/**
  * Function test() overload
  * This overload works similarly as the "test()" above, but on member methods
  * of classes. Of course, the class has to be instantiated first.
  *
  * Example:
  * test<Foo>(cout, "Class Foo", args(2), &Foo::bar, args(4), 2);
  *
  * "Foo" is the class of which the member method "bar" is to be tested. The
  * first "args(2)" contains the arguments which are passed to the constructor
  * of Foo. The remaining part works as with the standard test().
**/
template<typename CUT, template<typename> typename COMP=std::equal_to, typename F, typename INIT_TUPLE, typename... T>
static int test(std::ostream& os, const char* name, INIT_TUPLE&& it, F&& f, T&&... rest){
  auto fp = smart_bind(std::forward<F>(f), std::make_from_tuple<CUT>(std::move(it))); //instantiate CUT and bind the instance's member function f to fp
  return test<COMP>(std::forward<std::ostream&>(os), std::move(name), std::move(fp), std::forward<T>(rest)...);
}


/*##############################################################################
#void function handling
##############################################################################*/

/**
  * Function test() overload
  * This overload works similarly as the "test()" above, but is intended for
  * functions which manipulate an argument called by reference and you want to
  * make sure the referred variable changed the way you wanted.
  *
  * Example:
  * int a = 8;
  * test(cout,"call-by-reference",&foo,args(a),a,24);
  *
  * foo is a function which simply multiplies its argument with 3. For the test,
  * a third argument between the args() and the reference value has to be passed
  * to test which is a reference to the variable to be compared with the
  * reference value.
**/
template<template<typename> typename COMP=std::equal_to, typename F, typename R, typename... F_ARGS, typename... T>
static int test(std::ostream& os, const char* name, F&& f, std::tuple<F_ARGS&...>&& args, R& p, R&& r, T&&... rest){
  std::string header("\033[90mTest\033[0m ");
  header+=name;
  if (inTestEnv)  Test_Counter++;
  if (header.length()>TEST_NAME_LENGTH) { header.resize(TEST_NAME_LENGTH-3); }
  auto ret_tup = probe<COMP>(std::forward<F>(f), std::forward<std::tuple<F_ARGS&...>>(args), p, std::forward<R>(r), std::forward<T>(rest)...);
  if (std::get<0>(ret_tup)==(sizeof... (rest)/2+1)){
  os<<std::setfill('.')<<std::setw(TEST_NAME_LENGTH+9)<<std::left<<header<<std::setw(9)<<"\033[1;32mpassed\033[0m"<<std::endl;
  if (inTestEnv)  Passed_Test++;
  return 0;
  }
  else{
  os<<std::setfill('.')<<std::setw(TEST_NAME_LENGTH+9)<<std::left<<header
  <<std::setw(9)<<"\033[1;31mfailed\n(test case "<<std::get<0>(ret_tup)
  <<": "<<std::get<1>(ret_tup)<<" returned instead of "<<std::get<2>(ret_tup)
  <<")\033[0m"<<std::endl;
  return 1;
  }

}

};

//End of namespace k_test

#endif
