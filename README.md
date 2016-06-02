# Guildford
Engine Investigation
After Lionhead closed, I needed something to do, so I set about creating a one-thread-per-core engine with support for:
* IO operations using IoCompletionPorts
* Asynchronous functions.
* Taskgraph API

The system uses exceptions, but only as a means of debugging.  My philosophy is to only use exceptions as a way of
shutting down gracefully and explaining the failure.

Multithreaded exceptions are currently supported, and are transported to the Main thread for throwing later.

The taskgraph API is still very much in the prototype stage, and currently does not handle types with destructors.
Further work is underway to fix this by storing a function that calls the object's destructor at the end of the TaskAllocator's
memory range.

