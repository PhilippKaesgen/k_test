# k_test
### Intuitive Unit Testing


When I test a function or program, I only want to focus on the input and output - 
nothing in between! Ideally, I can tell my test suite which function to test,
which inputs to use, and what I expect, i.e. in a fashion like

`test(function,input,expected_output)`.

I don't want to care about call-by-reference, call-by-value or explicitly calling 
constructors to test member functions. I'm only willing to give as few information
as necessary to get the job done.

If you feel the same about testing, i.e. you desire a concise way to define a test,
you will find a versatile unit testing framework here. Have a look at the "example.cpp" 
file to see how it's applied and download it to see the report! 

Please, let me know if you have any suggestions or requests.

###### IMPORTANT: 
Compile k_test with C++17 features, e.g. `g++ -std=c++1z example.cpp` for gcc. That said,
the outputstream format is optimized for Linux.
