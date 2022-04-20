# GDB: GNU Debugger

The [GDB website](https://www.sourceware.org/gdb/) says, "GDB, the GNU Project debugger, allows you to see what is going on `inside' another program while it executes -- or what another program was doing at the moment it crashed.

GDB can do four main kinds of things (plus other things in support of these) to help you catch bugs in the act:

1. Start your program, specifying anything that might affect its behavior.
2. Make your program stop on specified conditions.
3. Examine what has happened when your program has stopped.
4. Change things in your program so you can experiment with correcting the effects of one bug and go on to learn about another."

## Why use a debugger?

Most, if not all, of you should be familiar with naive debugging strategies, such as adding `cout` statements all over your code or explaining your code to a rubber duck (rubber duck debugging is a real thing).

There are some key weaknesses of such strategies. For example,
- adding `cout` statements throughout your program clutters your code and your terminal output.
- explaining your code to a rubber duck requires a lot of cognitive overhead to translate program semantics to a natural language. In addition, many semantics are lost in translation.

Therefore, as you transition to larger C++ projects at UCSB, we recommend using GDB.

## What's in this repo?

This repo contains `buggyLinkedList.cpp`, a stripped-down version of lab01. The `main()` function 
- creates an empty linked list on the heap
- prints out the linked list before adding nodes
- adds nodes to the end of the linked list
- prints out the linked list after adding nodes
- destroys the linked list on the heap 

You may be able to identify the bug(s) immediately because the program is small and you have already encountered similar bug(s) in lab01. However, let's just stick with this example for clarity.

## How to use GDB

### Step 0: Compile and run the program normally
```
$ g++ buggyLinkedList.cpp
$ ./a.out
BEFORE: null
Segmentation fault (core dumped)
```

Oh no, looks like we have a segfault 😧.

### Step 1: Compile the program with `-g`

*Note: The `-g` flag keeps useful information for GDB that would otherwise be lost during compilation.*

```
g++ -g buggyLinkedList.cpp
```

### Step 2: Start GDB
```
$ gdb ./a.out
GNU gdb (Ubuntu 9.2-0ubuntu1~20.04.1) 9.2
Copyright (C) 2020 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
Type "show copying" and "show warranty" for details.
This GDB was configured as "x86_64-linux-gnu".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
    <http://www.gnu.org/software/gdb/documentation/>.

For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from ./a.out...
(gdb)
```

### Step 3: Run the program using GDB
```
(gdb) run
```

*Note: If your program takes command-line arguments, do `run <args>`.*

### Step 4: Find and fix what caused the segfault
```
BEFORE: null

Program received signal SIGSEGV, Segmentation fault.
0x00005555555567ab in addIntToEndOfList (list=0x55555556ceb0, value=1) at buggyLinkedList.cpp:47
47	    list->tail->next = p;
(gdb) 
```

We can see that line 47 caused a segfault.

**Print a backtrace**

The backtrace gives us the function call stack that led us to where we are.

```
(gdb) backtrace
#0  0x00005555555567ab in addIntToEndOfList (list=0x55555556ceb0, value=1) at buggyLinkedList.cpp:47
#1  0x0000555555556864 in main (argc=1, argv=0x7fffffffe058) at buggyLinkedList.cpp:61
```

*Note: this means that `main()` called `addIntToEndOfList()`, where we are now.*

**Print an expression**

Let's see what `list->tail` is.

```
(gdb) print list->tail
$1 = (Node *) 0x0
```

*Note: print can also print structs and arrays!*

Aha! `list->tail` is null, so dereferencing it with `->next` causes the segfault!

Let's quit GDB with `quit` and `y` and fix the program as follows:

```
void addIntToEndOfList(LinkedList* list, int value) {
    Node* p = new Node{value, nullptr};
    if(list->tail != nullptr) {
        list->tail->next = p;
    }
    list->tail = p;
}
```

Next, let's recompile the program, restart GDB, and rerun the fixed program. 

*Note: Although we didn't use the commands below in this example, they are useful.*

**Get info about the function's arguments**
```
(gdb) info args
list = 0x55555556ceb0
value = 1
```

**Get info about local variables**
```
(gdb) info locals
p = 0x55555556d2e0
```

### Step 5: Find and fix the next bug
```
(gdb) run
Starting program: /home/ganesh/Desktop/debuggers/a.out 
BEFORE: null
AFTER: null
[Inferior 1 (process 6335) exited normally]
(gdb) 
```

Although the process now exits normally, notice that this is not the output we expect. We expect `AFTER: [1]->[2]->[3]->null`, not `AFTER: null`. We need to stop the program while it is running and see what's going on.

**Set a breakpoint**

A breakpoint is a program location. If a breakpoint is reached, GDB stops the program. Let's set a breakpoint on line 63, right before we add nodes to the list.

```
(gdb) break 63
Breakpoint 1 at 0x2860: file buggyLinkedList.cpp, line 63.
(gdb) run
Starting program: /home/ganesh/Desktop/debuggers/a.out 
BEFORE: null

Breakpoint 1, main (argc=1, argv=0x7fffffffe058) at buggyLinkedList.cpp:63
63	    addIntToEndOfList(list, 1);
(gdb) 
```

*Note: you can also use a function name, like `break addIntToEndOfList`*.

**Step through the code, line by line**

Now that we stopped the program, we can see what's going on line by line. 

```
(gdb) step
addIntToEndOfList (list=0x55555555a040 <std::cout@@GLIBCXX_3.4>, value=21845) at buggyLinkedList.cpp:45
45	void addIntToEndOfList(LinkedList* list, int value) {
(gdb) step
46	    Node* p = new Node{value, nullptr};
(gdb) step
47	    if(list->tail) {
(gdb) step
50	    list->tail = p;
(gdb) step
51	}
(gdb) print list->head
$1 = (Node *) 0x0
```

Aha! `list->head` is null at the end of `addIntToEndOfList`!

Let's confirm that this is the issue by advancing to line 68, where we print out the linked list. 

Advancing using `step` can be tedious because `step` dives into every function. Instead, let's use a similar command, `next`, so that once we return to `main()`, we don't dive into `addIntToEndOfList()` again. 

```
(gdb) step
main (argc=1, argv=0x7fffffffe058) at buggyLinkedList.cpp:64
64	    addIntToEndOfList(list, 2);
(gdb) next
65	    addIntToEndOfList(list, 3);
(gdb) next
68	    cout << "AFTER: " << linkedListToString(list) << endl;
```

Once we reach line 68, let's use `step` to dive into `linkedListToString()`.
```
(gdb) step
linkedListToString[abi:cxx11](LinkedList*) (list=0x7fffffffde80) at buggyLinkedList.cpp:24
24	string linkedListToString(LinkedList *list) {
(gdb) step
25	    string result="";
(gdb) step
26	    for(const Node *  p=list->head; p!=nullptr; p=p->next) {
(gdb) step
29	    result += "null";
```

Aha! We never enter the for loop on line 26 because `head` is still null.

I'll leave you to fix the bug, recompile, and restart GDB!

*Note: Although we didn't use the commands below in this example, they are useful.*

**When using step, advance to the end of the function**
```
45	void addIntToEndOfList(LinkedList* list, int value) {
(gdb) step
46	    Node* p = new Node{value, nullptr};
(gdb) step
47	    if(list->tail) {
(gdb) finish
Run till exit from #0  addIntToEndOfList (list=0x55555556ceb0, value=1) at buggyLinkedList.cpp:47
main (argc=1, argv=0x7fffffffe058) at buggyLinkedList.cpp:68
68	    addIntToEndOfList(list, 2);
```

**When using step or next, continue execution**
```
(gdb) continue
Continuing.
```

### Step 6: Verify that the program works in GDB
Like a breakpoint, a watchpoint is a data item. If the value of the data item changes, GDB stops the program. Let's set a watchpoint on `list->head` to confirm that it changes from null to a valid address. We can't do this at the beginning of the program, though, because GDB doesn't know that `list` is a struct yet. Instead, let's wait until `list` is initialized, and then set the watchpoint.

```
(gdb) break 60
Breakpoint 1 at 0x2805: file buggyLinkedList.cpp, line 60.
(gdb) run
Starting program: /home/ganesh/Desktop/debuggers/a.out 

Breakpoint 1, main (argc=1, argv=0x7fffffffe058) at buggyLinkedList.cpp:60
60	    LinkedList* list = new LinkedList;
(gdb) next
63	    cout << "BEFORE: " << linkedListToString(list) << endl;
(gdb) watch list->head
Hardware watchpoint 2: list->head
(gdb) continue
Continuing.
BEFORE: null

Hardware watchpoint 2: list->head

Old value = (Node *) 0x0
New value = (Node *) 0x55555556d2e0
addIntToEndOfList (list=0x55555556ceb0, value=1) at buggyLinkedList.cpp:55
55	}
```

Yay! We can see that head is updated from null to `0x55555556d2e0`!

## Other Resources
This demo only scratches the surface of GDB. There are many resources online, including
- GDB cheat sheet: https://darkdust.net/files/GDB%20Cheat%20Sheet.pdf
- https://ucsb-cs32.github.io/topics/tools_gdb/ (from CS32)
