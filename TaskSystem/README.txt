Overview

The source code is organized into a series of folders, each of which defines a specific
object or sub-system.

At the top of the source directory, you will find a CMakeLists.txt file.
This is the cmake configuration file. If you are not familiar with
cmake, it is a build generation system. Typically, this means that it is used to
construct (good) makefiles. If you haven't used it before, I highly recommend that
you become familiar with it. I have provided a very simple cmake file, just to get
the code constructed and running. cmake can do much more than this, however.
   
To use cmake, you simply need cmake installed on your system. If it's not there 
already, a package can be downloaded. Then just go to the main source folder and type
   
   "cmake CMakeLists.txt"
   
This will build the makefiles, including a "Makefile" in the main folder. When you type
"make", a clean compile will produce an executable called test.exe




System Logic:

As we discussed, our goal is to construct an intuitive message passing mechanism that
can be layered on top of the OS-level thread facilities. The model must also be
compatible with the Object Oriented style of the language framework that I am developing.

If you build and run the test.exe application, you will get output that might
look like this (depending on the OS scheduling):



*****************

Simple Task test...

sending a Text message [Hello World] from Task 1 to Task 2...
sending a Bar message [44] from Task 1 to Task 3...
sending a Bar message [99] from Task 1 to Task 2...
sending a Text message [Goodbye World] from Task 1 to Task 3...
sending a Bar message [11] from Task 1 to Task 4...
sending a Text message [me too] from Task 1 to Task 4...

Task 4 No Handler for tag = 1, dropping message! 

Task 4 Text message handler: me too

Task 3 Bar message handler, value: 44

Task 3 Text message handler: Goodbye World

Task 2 Text message handler: Hello World

Task 2 Bar message handler, value: 99

...successful test!!


*********************

In short, a parent task (called PapaTask) has been instantiated in the main method of the program.
In turn, the PapaTask has generated three child tasks: two FooTasks and a BazTask. It then
sends two messages to each of these three child tasks. Each message consists of a message
object. In the current test program, I am using TextMsg objects (contains a string) and
BarMsg objects (contains an int). But Message objects can be defined in any way that the
programmer likes.

In any case, PapaTask sends the 6 messages to the child Tasks (it uses a system-provided
send method to do this). In turn, each of the child tasks checks for its messages. 
It does this by calling its receive method twice. The receive method provides "handler"  
methods for the various types of objects that it is willing to accept. In the current case, 
FooTasks can accept both TextMsgs and BarMsgs. The BazTask can only accept TextMsgs.

In the output, you can see that the child Tasks respond as expected. Each of the messages
is received (as discussed, this is a copy of the original message) and the contents are
unpacked and printed. The one exception is the BarMsg sent to the BazTask. It does not have
a BarMsg handler, so it drops (i.e., ignores) the BarMsg.



System Structure: 

The prototype consists of the following elements:

1] TaskTest.c

The main "driver" file for the application. It doesn't do much except 
create the PapaTask. Once created, tasks start executing immediately. The driver
file also goes to sleep for a couple of seconds just to let the child threads finish what
they are doing. If this is not done, the main method will finish before the sub Tasks and the
whole program will terminate.

One other important feature of the driver program is that it is where the Communication 
sus-system (Comm) is initialized. More on this below.


2] PapaTask

The parent task in this application. Note that "PapaTask" is just a name used for this example.
You can name Tasks anything you like, as long as they use the same structure.

The PapaTask.c file contains the main code for the Task. Near the top of the file you will
notice the following line:

#include "generated.h"

In fact, you will see this in a number of files. The generated.h file contains code the programmer
would not actually write themselves. Instead, the logic in this file would be generated
automatically by the compiler/language system (my language framework will do something like
this as well). So we put this in a separate file so that we can focus on what the programmer will
actually see.

That said, you must be very familiar with the contents of the generated.h files since YOU are
the system generator in this project. So in the PapaTask generated file you will see:

i] function prototypes that must be visible by all parts of the file.
ii] The PapaTask "class" structure. Right now, it just contains a reference to the underlying
   pthread, as well as a TaskID. Other things may be added in the future.
iii] The PapaTask_create() "constructor"method . Here, we create the underlying pthread, as well as 
   its associated TaskID and Message Queue. We also indicate that the Task will use its
   "run" method as its starting point.
iv] The "run" method. In practice, the initial pthread method is invoked with a fairly
   ugly syntax and exposes the Pthread API. We don't want this. So instead, run is just used
   here as a wrapper to provide a more intuitive "start" method for the programmer.
v] send method. Because "send" is very simple, we provide it automatically (this may or may
   not change in the future).
   
   
In the main PapaTask.c file, we now have just a few components:

i] an enum that defines message tags. Because the C compiler does not allow method 
   overloading (i.e., using the same function name with different arguments), we need
   a way to distinguish messages from one another on the receiving side. MPI uses
   message tags for this purpose so we will start with this strategy.
   
ii]  The start method. This is the "main" method for the thread. It receives a
     "this" pointer as a paramater. The "this" pointer is important because multiple
     instances of this task can be created. So the start method (or any method) must
     know which Task instance it should be operating upon.
     
     In our small example, the start method is used to create other Tasks and to send
     messages to them. It is possible, of course, to call other methods from start. 
     
iii] PapaTask inclues an empty receive method. It is not used right now, but this is 
     where the receive handlers would go (FooTask and BazTask have examples of this).
   
   
We also have a PapaTask.h file. This is the header file that is included by other
objects that would like to create a PapaTask. Apart from the constructor, no other
Task methods should be visible since it would not be possible to invoke them directly, given
that they "exist" within other execution threads. For this reason, you will see that the
send/receive methods are declared static, making them invisible outside of the
PapaTask.c file. 
 


3] FooTask

Another Task class, similar in structure to PapaTask. Because it is used as a child
Task, however, it provides receive "handlers" to process incoming messages.

You can see that the receive model is based upon the idea of matching the handlers
to incoming messages. In some sense, the receieve logic is the "heart" of the 
message passing framework, so this is something that deserves a lot of attention
going forward.



4] BazTask

Another Task class. It differs from FooTask only in the number of message types that
it is expecting.


5] System

This is the underlying framework for delivering messages. In short, each Task is associated
with its own message queue. The queues are not actually part of the task object itself. This
would create a very disjointed system, with queues spread across many different threads.

Instead, the System "object" maintains a table of Message Queues, one for each Task. The
message queues are indexed by TaskID, so it is possible to find the proper queue in constant
time, once we are given the TaskID. TaskIDs themselves are also managed by the system,
which protects the TaskID generator with a mutex to ensure that a Task always gets the right
TaskID.

In addition to creating and protecting the queues, the System object also provide the main
send/receive methods. Ultimately, when Tasks use their own send/receive methods, they
are calling the System send/receive methods. In turn, the System manipulates the relevant
message queues by pushing or popping messages, as required.


6] Errors

Just contains a header that defines a simple error routine. We can make this more sophisticated
if we like.


7] Message

This is the message "Interface". In order to handle messages of any type, the system needs to know
what API all messages support. This is defined in Messages.h. It is possible for new message types
to add additional fields, but they must always support the basic model defined in Message.h.

Note there is no actual Message.c file. This is not necessary. Instead, the message structure
defined in Message.h can be used as a method parameter in other files, and then cast to the
proper type as necessary.



8] MsgQueue

This defines the structure of the individual queue objects associated with each Task. The 
queue data structure itself is adapted from Mark Weiss's online sources. In order
to use it in this context, however, we must be able to protect the queues from concurrent access
or modification. 

To do this, each queue has been augmented with its own mutex lock.
Whenever an access is made, the lock must be obtained before any modifications can be made.
Basically, this is a crude version of Java's "synchronized" model. Note that I have not
made any attempt to optimize performance here since this will be a target of future work.

Also note that Weiss's queue is actually built on top of an array, rather than a list.
This is something we will probably need to change in the future since we want the size
to be very flexible, without requiring a reconstruction of the array when it fills up.


10] TextMsg

This is an example of a message object. As with the Tasks the "system" code has been
moved into a generated.h file. One thing to note about the constructor in the
generated.h file is how it assigns methods to the public API for the object.
When using C in in OOP style, this is a fairly typical approach. So, basically, the methods
defined by the programmer in the main TextMsg.c file will be mapped to pointers in the 
TextMsg structure.

Note that when you are invoking methods this way, there is a "double" use of the 
object variable. For example, if you have a TextMsg called abc and you want to
invoke its getTag() method, you would say the following:

abc->getTag(abc)

The first abc is a pointer to the TextMsg structure, while the second abc represents
the "this" pointer. BOTH are required in order for this model to work.

In terms of the methods themselves, perhaps the most important is the clone() method. This
will be invoked automatically by the System.send() method to create copies of the
message before insertion into the target message queue.


11] BarMsg

Just another message object. In this case, it contains an int value, but you can 
put anything into a message object, as long as you provide a way to get it out.







Going forward:

The current prototype should represent a good starting point. I wanted to provide you with
enough detail so that you could get a good sense of where we are trying to go.

That said, there are many things to work on:

1] We must do proper unit testing to see where the system fails (e.g., timing 
   problems, empty message queues, deadlocks)
   
2] Performance testing: How does this kind of model compare to the other methods
that you are looking at? Where are the bottlenecks? 


3] The receive functionality must be made as flexible and as powerful as possible.
What does erlang do? What does Java's Akka do? Right now, the receive logic is very
basic: It just executes the receive and it finds something useful at the head of the
queue or it doesn't. Should we look beyong the head? Should we be able to wait/block
for the message. Would that make deadlocks too likely? Maybe a wait with a 
timeout (I think Erlang does that). Could you be signalled when a message arrives?
Is that even a good idea. And so on...

4] Task Registry. Right now, I am using the simple parent/child model. In other
words, the parent always knows the TaskID of its spawnwd childern. This works but
is of course very limited. We need to be able to communicate between random Tasks.

5] Communication protocols. This is the nanoMsg stuff. Once the core framework is 
working, we want to layer more expressive protocols on top of the system.

6] Extension to inter-process communication. We would like to take the same API
and use it to support communication between independent processes (and maybe DNS
stuff eventually).

7] Lockfree algorithms. How much of a performance improvement would they give us.

8] Demo the system with implementations of standard algorithms (producer/consumer,
dining philosophers, etc)

9] Error handling: What happens when things go wrong. If a single Task fails,
can we recover gracefully?

9] ... and probably lots of other stuff.

So the prototype is a nice place to start, but there is alot of work still to be done.





