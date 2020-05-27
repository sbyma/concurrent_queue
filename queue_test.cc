
// Concurrent Queue Demo
// Stuart Byma

/* Copyright 2020 Stuart Byma
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <iostream>
#include <thread>
#include "concurrent_queue.h"

struct WorkItem {
  WorkItem() = default;
  int x;
  std::string stuff;

  WorkItem(const WorkItem& other) = delete;
  WorkItem& operator=(const WorkItem& other) = delete;
};

int main(int argc, char* argv[]) {
  bcq::ConcurrentQueue<int> test_queue(5);

  test_queue.Push(42);

  int pop_value = 0;
  test_queue.Pop(pop_value);

  std::cout << "Pushed and popped value: " << pop_value << "\n";

  /*
  * this will fail to compile, because WorkItem is not copyable
  ConcurrentQueue<WorkItem> work_queue(5);
  WorkItem v;
  testvalue_queue.Peek(v);
  */

  return 0;
}