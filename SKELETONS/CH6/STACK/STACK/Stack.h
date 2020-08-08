#ifndef _Stack_h_
#define _Stack_h_
#define STACK_MAX 1024
class Stack {

 public:
  
  // Constructor:
  Stack();

  // Push:
  void push(int i);

  // Pop:
  int pop();

  // How big is the stack?
  unsigned int size() const;

  // Print (to std::cout) 
  void print() const;

  // Test if the stack is empty
  bool isEmpty() const;

 private:

  unsigned int       _count;            // Actual length of stack.
  int                _data[STACK_MAX];  // An array to store data.
};
#endif
