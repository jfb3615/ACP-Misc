// This is a program to test the stack.  It will work
// as soon as you have finished implementing the stack
// class, which means filling in all of the function
// definitions in Stack.cpp

#include <iostream>
#include "STACK/Stack.h"
int main (int argc, char **argv) {

  // Create a new Stack called a;
  Stack a;
   
  // Fill it with the powers of 2, through 128
  a.push(1);
  a.push(2);
  a.push(4);
  a.push(8);
  a.push(16);
  a.push(32);
  a.push(64);
  a.push(128);

  // OK now how big is it?

  std::cout << "Size of stack " << a.size() << std::endl;

  // Print the stack out:
  a.print();
  
  // Now pop of two element from the stack:
  std::cout << "Popping " << a.pop() << " from the stack" << std::endl;
  std::cout << "Popping " << a.pop() << " from the stack" << std::endl;


  // See what it looks like now:
  a.print();

  // Create another stack:
  Stack b;
  while (!a.isEmpty()) b.push(a.pop());
  
  // Now a is empty...
  a.print();
  
  // And b contains a, in reverse order, right? 
  b.print();
  
  // But now let's assign the stack b to a:
  a=b;

  // and finally we end up with identical stacks:
  a.print();

  // Cool, huh?
  return 0;

}
