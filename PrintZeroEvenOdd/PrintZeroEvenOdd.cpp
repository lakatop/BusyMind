/*
1116. Print Zero Even Odd
You have a function printNumber that can be called with an integer parameter and
prints it to the console.

For example, calling printNumber(7) prints 7 to the console.
You are given an instance of the class ZeroEvenOdd that has three functions:
zero, even, and odd. The same instance of ZeroEvenOdd will be passed to three
different threads:

Thread A: calls zero() that should only output 0's.
Thread B: calls even() that should only output even numbers.
Thread C: calls odd() that should only output odd numbers.
Modify the given class to output the series "010203040506..." where the length
of the series must be 2n.

Implement the ZeroEvenOdd class:

ZeroEvenOdd(int n) Initializes the object with the number n that represents the
numbers that should be printed. void zero(printNumber) Calls printNumber to
output one zero. void even(printNumber) Calls printNumber to output one even
number. void odd(printNumber) Calls printNumber to output one odd number.


Example 1:

Input: n = 2
Output: "0102"
Explanation: There are three threads being fired asynchronously.
One of them calls zero(), the other calls even(), and the last one calls odd().
"0102" is the correct output.
Example 2:

Input: n = 5
Output: "0102030405"


Constraints:

1 <= n <= 1000
*/

#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>

class ZeroEvenOdd {
 private:
  int n;
  std::mutex _m;
  std::condition_variable _zeroCv;
  std::condition_variable _evenCv;
  std::condition_variable _oddCv;
  int _state = 0;
  int _print = 1;

 public:
  ZeroEvenOdd(int n) { this->n = n; }

  // printNumber(x) outputs "x", where x is an integer.
  void zero(std::function<void(int)> printNumber) {
    std::unique_lock lock(_m);
    _zeroCv.wait(lock, [this] { return _state == 0; });
    printNumber(0);

    _state = (_print & 1) + 1;
    std::cout << "State " << _state << std::endl;
    if (_state == 1)
      _evenCv.notify_one();
    else
      _oddCv.notify_one();
  }

  void even(std::function<void(int)> printNumber) {
    std::unique_lock lock(_m);
    _evenCv.wait(lock, [this] { return _state == 1; });
    if (_print > n) return;
    printNumber(_print);
    ++_print;

    // modify state
    _state = 0;

    _zeroCv.notify_one();
  }

  void odd(std::function<void(int)> printNumber) {
    std::unique_lock lock(_m);
    _oddCv.wait(lock, [this] { return _state == 2; });
    if (_print > n) return;
    printNumber(_print);
    ++_print;

    // modify state
    _state = 0;

    _zeroCv.notify_one();
  }

  void a() { return; }
};

int main() {
  ZeroEvenOdd z(5);

  std::function<void(int)> customPrint = [](int x) { std::cout << x; };

  std::thread t1(&ZeroEvenOdd::zero, &z, customPrint);
  std::thread t2(&ZeroEvenOdd::even, &z, customPrint);
  std::thread t3(&ZeroEvenOdd::odd, &z, customPrint);

  t1.join();
  t2.join();
  t3.join();

  return 0;
}