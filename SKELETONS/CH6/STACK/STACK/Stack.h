#ifndef _Stack_h_
#define _Stack_h_

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

  static constexpr unsigned int STACK_MAX=1024;

  unsigned int       m_count={0};            // Actual length of stack.
  int                m_data[STACK_MAX]={0};  // An array to store data.
};
#endif
