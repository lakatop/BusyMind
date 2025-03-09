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

#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>

// Solution with multiple condition variables
class ZeroEvenOddMultipleCv {
 private:
  int n;
  std::mutex _m;
  std::condition_variable _zeroCv;
  std::condition_variable _evenCv;
  std::condition_variable _oddCv;
  std::atomic<int> _state = 0;
  std::atomic<int> _print = 1;

 public:
  ZeroEvenOddMultipleCv(int n) { this->n = n; }

  // printNumber(x) outputs "x", where x is an integer.
  void zero(std::function<void(int)> printNumber) {
    for (int i = 0; i < n; ++i) {
      std::unique_lock lock(_m);
      _zeroCv.wait(lock, [&] { return _state == 0; });
      printNumber(0);

      _state = (_print & 1) + 1;
      if (_state == 1)
        _evenCv.notify_one();
      else
        _oddCv.notify_one();
    }
  }

  void even(std::function<void(int)> printNumber) {
    for (int i = 0; i < n / 2; i++) {
      std::unique_lock lock(_m);
      _evenCv.wait(lock, [&] { return _state == 1; });
      printNumber(_print);
      ++_print;

      // modify state
      _state = 0;

      _zeroCv.notify_one();
    }
  }

  void odd(std::function<void(int)> printNumber) {
    for (int i = 0; i < (n + 1) / 2; i++) {
      std::unique_lock lock(_m);
      _oddCv.wait(lock, [&] { return _state == 2; });
      printNumber(_print);
      ++_print;

      // modify state
      _state = 0;

      _zeroCv.notify_one();
    }
  }
};

// Solution with single condition variable
class ZeroEvenOddSingleCv {
 private:
  int n;
  std::mutex _m;
  std::condition_variable _cv;
  std::atomic<int> _state;
  std::atomic<int> _print;

 public:
  ZeroEvenOddSingleCv(int n) {
    this->n = n;
    _state = 0;
    _print = 1;
  }

  // printNumber(x) outputs "x", where x is an integer.
  void zero(std::function<void(int)> printNumber) {
    for (int i = 0; i < n; ++i) {
      std::unique_lock lock(_m);
      _cv.wait(lock, [this]() { return _state == 0; });
      printNumber(0);
      _state = (_print & 1) + 1;
      _cv.notify_all();
    }
  }

  void even(std::function<void(int)> printNumber) {
    for (int i = 0; i < n / 2; ++i) {
      std::unique_lock lock(_m);
      _cv.wait(lock, [this]() { return _state == 1; });

      printNumber(_print);
      ++_print;

      _state = 0;

      _cv.notify_all();
    }
  }

  void odd(std::function<void(int)> printNumber) {
    for (int i = 0; i < (n + 1) / 2; ++i) {
      std::unique_lock lock(_m);
      _cv.wait(lock, [this]() { return _state == 2; });

      printNumber(_print);
      ++_print;

      _state = 0;

      _cv.notify_all();
    }
  }
};

// --------------- TESTS --------------------
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

TEST_CASE("Test ZeroEvenOddSingleCv - small odd number") {
  ZeroEvenOddSingleCv z(5);

  std::function<void(int)> customPrint = [](int x) { std::cout << x; };

  std::ostringstream oss;
  std::streambuf* p_cout_streambuf = std::cout.rdbuf();
  std::cout.rdbuf(oss.rdbuf());

  std::thread t1(&ZeroEvenOddSingleCv::zero, &z, customPrint);
  std::thread t3(&ZeroEvenOddSingleCv::odd, &z, customPrint);
  std::thread t2(&ZeroEvenOddSingleCv::even, &z, customPrint);

  t1.join();
  t2.join();
  t3.join();

  std::cout.rdbuf(p_cout_streambuf);  // restore

  // test your oss content...
  CHECK(oss.str() == "0102030405");
}

TEST_CASE("Test ZeroEvenOddSingleCv - small even number") {
  ZeroEvenOddSingleCv z(4);

  std::function<void(int)> customPrint = [](int x) { std::cout << x; };

  std::ostringstream oss;
  std::streambuf* p_cout_streambuf = std::cout.rdbuf();
  std::cout.rdbuf(oss.rdbuf());

  std::thread t1(&ZeroEvenOddSingleCv::zero, &z, customPrint);
  std::thread t3(&ZeroEvenOddSingleCv::odd, &z, customPrint);
  std::thread t2(&ZeroEvenOddSingleCv::even, &z, customPrint);

  t1.join();
  t2.join();
  t3.join();

  std::cout.rdbuf(p_cout_streambuf);  // restore

  // test your oss content...
  CHECK(oss.str() == "01020304");
}

TEST_CASE("Test ZeroEvenOddSingleCv - big odd number") {
  ZeroEvenOddSingleCv z(101);

  std::function<void(int)> customPrint = [](int x) { std::cout << x; };

  std::ostringstream oss;
  std::streambuf* p_cout_streambuf = std::cout.rdbuf();
  std::cout.rdbuf(oss.rdbuf());

  std::thread t1(&ZeroEvenOddSingleCv::zero, &z, customPrint);
  std::thread t3(&ZeroEvenOddSingleCv::odd, &z, customPrint);
  std::thread t2(&ZeroEvenOddSingleCv::even, &z, customPrint);

  t1.join();
  t2.join();
  t3.join();

  std::cout.rdbuf(p_cout_streambuf);  // restore

  // test your oss content...
  CHECK(oss.str() ==
        "0102030405060708090100110120130140150160170180190200210220230240250260"
        "2702802903003103203303403503603703803904004104204304404504604704804905"
        "0051052053054055056057058059060061062063064065066067068069070071072073"
        "0740750760770780790800810820830840850860870880890900910920930940950960"
        "9709809901000101");
}

TEST_CASE("Test ZeroEvenOddSingleCv - big even number") {
  ZeroEvenOddSingleCv z(100);

  std::function<void(int)> customPrint = [](int x) { std::cout << x; };

  std::ostringstream oss;
  std::streambuf* p_cout_streambuf = std::cout.rdbuf();
  std::cout.rdbuf(oss.rdbuf());

  std::thread t1(&ZeroEvenOddSingleCv::zero, &z, customPrint);
  std::thread t3(&ZeroEvenOddSingleCv::odd, &z, customPrint);
  std::thread t2(&ZeroEvenOddSingleCv::even, &z, customPrint);

  t1.join();
  t2.join();
  t3.join();

  std::cout.rdbuf(p_cout_streambuf);  // restore

  // test your oss content...
  CHECK(oss.str() ==
        "0102030405060708090100110120130140150160170180190200210220230240250260"
        "2702802903003103203303403503603703803904004104204304404504604704804905"
        "0051052053054055056057058059060061062063064065066067068069070071072073"
        "0740750760770780790800810820830840850860870880890900910920930940950960"
        "970980990100");
}

TEST_CASE("Test ZeroEvenOddMultipleCv - small odd number") {
  ZeroEvenOddMultipleCv z(5);

  std::function<void(int)> customPrint = [](int x) { std::cout << x; };

  std::ostringstream oss;
  std::streambuf* p_cout_streambuf = std::cout.rdbuf();
  std::cout.rdbuf(oss.rdbuf());

  std::thread t1(&ZeroEvenOddMultipleCv::zero, &z, customPrint);
  std::thread t3(&ZeroEvenOddMultipleCv::odd, &z, customPrint);
  std::thread t2(&ZeroEvenOddMultipleCv::even, &z, customPrint);

  t1.join();
  t2.join();
  t3.join();

  std::cout.rdbuf(p_cout_streambuf);  // restore

  // test your oss content...
  CHECK(oss.str() == "0102030405");
}

TEST_CASE("Test ZeroEvenOddMultipleCv - small even number") {
  ZeroEvenOddMultipleCv z(4);

  std::function<void(int)> customPrint = [](int x) { std::cout << x; };

  std::ostringstream oss;
  std::streambuf* p_cout_streambuf = std::cout.rdbuf();
  std::cout.rdbuf(oss.rdbuf());

  std::thread t1(&ZeroEvenOddMultipleCv::zero, &z, customPrint);
  std::thread t3(&ZeroEvenOddMultipleCv::odd, &z, customPrint);
  std::thread t2(&ZeroEvenOddMultipleCv::even, &z, customPrint);

  t1.join();
  t2.join();
  t3.join();

  std::cout.rdbuf(p_cout_streambuf);  // restore

  // test your oss content...
  CHECK(oss.str() == "01020304");
}

TEST_CASE("Test ZeroEvenOddMultipleCv - big odd number") {
  ZeroEvenOddMultipleCv z(101);

  std::function<void(int)> customPrint = [](int x) { std::cout << x; };

  std::ostringstream oss;
  std::streambuf* p_cout_streambuf = std::cout.rdbuf();
  std::cout.rdbuf(oss.rdbuf());

  std::thread t1(&ZeroEvenOddMultipleCv::zero, &z, customPrint);
  std::thread t3(&ZeroEvenOddMultipleCv::odd, &z, customPrint);
  std::thread t2(&ZeroEvenOddMultipleCv::even, &z, customPrint);

  t1.join();
  t2.join();
  t3.join();

  std::cout.rdbuf(p_cout_streambuf);  // restore

  // test your oss content...
  CHECK(oss.str() ==
        "0102030405060708090100110120130140150160170180190200210220230240250260"
        "2702802903003103203303403503603703803904004104204304404504604704804905"
        "0051052053054055056057058059060061062063064065066067068069070071072073"
        "0740750760770780790800810820830840850860870880890900910920930940950960"
        "9709809901000101");
}

TEST_CASE("Test ZeroEvenOddMultipleCv - big even number") {
  ZeroEvenOddMultipleCv z(100);

  std::function<void(int)> customPrint = [](int x) { std::cout << x; };

  std::ostringstream oss;
  std::streambuf* p_cout_streambuf = std::cout.rdbuf();
  std::cout.rdbuf(oss.rdbuf());

  std::thread t1(&ZeroEvenOddMultipleCv::zero, &z, customPrint);
  std::thread t3(&ZeroEvenOddMultipleCv::odd, &z, customPrint);
  std::thread t2(&ZeroEvenOddMultipleCv::even, &z, customPrint);

  t1.join();
  t2.join();
  t3.join();

  std::cout.rdbuf(p_cout_streambuf);  // restore

  // test your oss content...
  CHECK(oss.str() ==
        "0102030405060708090100110120130140150160170180190200210220230240250260"
        "2702802903003103203303403503603703803904004104204304404504604704804905"
        "0051052053054055056057058059060061062063064065066067068069070071072073"
        "0740750760770780790800810820830840850860870880890900910920930940950960"
        "970980990100");
}
