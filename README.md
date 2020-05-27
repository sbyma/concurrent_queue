# Blocking/Non-blocking Concurrent Queue

At some point I couldn't find a concurrent queue implementation with the interface I wanted. 
So I made my own. 
It's probably pretty similar to some other ones out there.
I use this queue design a lot for distributing work in parallel, HPC-oriented systems.

This one supports multiple writers and multiple readers, blocking and non-blocking behavior, and move semantics on push/pop operations.
It has a limited buffer capacity and will block on push to a full queue.
You can peek the top value, but only if your contained type is copyable. 

Header only.

C++11 compatible.

No weird dependencies.

You can build the demo using `bazel build //:queue_test` and run with `./bazel-bin/queue_test`

Pull requests welcome. 