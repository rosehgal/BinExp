# Return 2 Libc attack
`ret2libc` attack is exploitable in case of following scenarios:
    * Memory is **non** executable.
    * Stack protection **disabled**.
    * Stack canaries **disabled**.
    * ASLR independent.

As the name suggest this attack, overflows the buffer and by overflowing the buffer it changes the return address to the address of the function in shared library(`libc`). The main thing to learn in this attack exploitation is that you need to understand the how the stack has to be tickled in case the return instruction has to mimic the actual CALL instruction. And this would require you to understand how the stack is laid during function call and to understand this you can go through this [Lecture](../Lecture1/README.md).

```
                        +   Previous function  |
                        |     Stack frame      |
                        |                      |
                        +----------------------+ <--- previous function stack frame end here
                        |Space for return value|
                        +----------------------+
                        |Arguments for function|
                        +----------------------+
                        |    return address    |
                        +----------------------+
                        |     saved $ebp       |
                        +----------------------+
                        |                      | <---  padding done by compilers
                        +----------------------+
                        |    local variables   |
                        
```
This is how the stack looks like in the when new function frame sets up and when the function will start returning from the this function then, it might look like this.  
```
                   +   Pre^ious function  +  +   Pre^ious function  + 
                   |     Stack frame      |  |     Stack frame      |
                   |                      |  |                      |
Previous fn+---->  +----------------------+  +----------------------+
                   |Space for return ^alue|  |space for return value|
                   +----------------------+  +----------------------+
                   |Arguments for function|  |Arguments for function|
                   +----------------------+  +----------------------+
                   |    return address    |  |                      |
                   +----------------------+  +----------------------+
                   |     sa^ed $ebp       |  |                      |
                   +----------------------+  +----------------------+
                   |                      |  |                      |
                   +----------------------+  |                      |
                   |    local variables   |  |                      |
                  
```
Since the control will not return to previous function but will go to some other function as default behaviour of `CALL` instruction. The call to new function will assume its begin called normally. And since we have already classified instruction before + CALL + after CALL as three different entities in previous lecture. We have to create the effect for the same in current stack.
```
      prev                                                                next
+-----------------+                                               +-----------------+
|                 |                                               |                 |
|1. ret val space |                                               |1. pushes ebp    |
|2. arguments     | +------------->CALL Inst +------------------> |2. updates ebp   |
|                 |                    +                          |                 |
|                 |                    v                          |                 |
+-----------------+       Pushes the return address               +-----------------+
```

What this current function will assume:
    1. Space for return value and argument are already removed.
    2. New function space and return address are already pushed to stack.
    3. New return address are already pushed to stack.
And other instructions will taken care by the next function only, in its code. Since in actual it is a return instruction, anything like this have not happened. So we have to make stack look like the same as if it was a normal function call but we know for now the stack would look like following in case it has returned.
```
                +   Pre^ious function  +
                |     Stack frame      |
                |                      |
                +----------------------+
                |space for return value|
                +----------------------+
                |Arguments for function|
                +----------------------+
                |                      |
                +----------------------+
                |                      |
                +----------------------+
                |                      |
                |                      |
             
```
When the control will go to the  next function, it will push `$ebp`. From the next functions perspective it has to look like this.
```
                +   Pre^ious function  ++   Pre^ious function  +
                |     Stack frame      ||     Stack frame      |
                |                      ||                      |
                +----------------------++----------------------+
                |space for return value||space for return value|
                +----------------------++----------------------+
                |Arguments for function||Arguments for function|
                +----------------------++----------------------+
                |                      ||       $ebp           |
                +----------------------++----------------------+
                |                      ||       padding        |
                +----------------------++----------------------+
                |                      ||                      |
             
```
Next function stack will look like this. Next function knows that on top of `$ebp` return address should be there so it will assume `argument previous function` as the `return address` for this function.
```
                   The actual stack              The way next see stack


                +   Pre^ious function  +      +   Pre^ious function  +
                |     Stack frame      |      |     Stack frame      |
                |                      |      |Space for return value|
                +----------------------+      +----------------------+
                |space for return value|      | Arguments to function|
                +----------------------+      +----------------------+
                |Arguments for function|      |    return address    |
                +----------------------+      +----------------------+
                |       $ebp           |      |       $ebp           |
                +----------------------+      +----------------------+
                |       padding        |      |       padding        |
                +----------------------+      +----------------------+
                |                      |      |                      |
                |                      |      |                      |

```
In comparison of first and last diagram we see that the stack seems to be shifted by the 4 bytes.
```
                    Actual Stack                    How next see it


                +   Pre^ious function  +       +   Pre^ious function  +
                |     Stack frame      |       |     Stack frame      |
                |                      |       |Space for return value|
                +----------------------+       +----------------------+
                |Space for return ^alue|       | Arguments to function|
                +----------------------+       +----------------------+
                |Arguments for function|       |    return address    |
                +----------------------+       +----------------------+
                |    return address    |       |       $ebp           |
                +----------------------+       +----------------------+
                |     sa^ed $ebp       |       |       padding        |
                +----------------------+       +----------------------+
                |                      |       |                      |

```
The buffer overflow has to override stack in such a way that to the next function the stack should look like the 2nd diagram.

#### Executing Return 2 libc.

The way the stack has to be overridden to call `system` will follow the same approach, in that case the `next` function that we were talking about so long will be `system`. In case of graceful exit of the program I will mark the return address of system with the call to `exit` :smile: Another `return2lib` from `system`.  
The exploit payload would look something like this:
``echo -e `python -c "import struct; print 'A'*108 + 'BBBB' + 'CCCC' + struct.pack('<I', <address of system>) + struct.pack('<I', <address of exit>) + <Address of argument to system>"` ``

Next stuff is to find the address of `system`, `exit` and `argument to system`(which would be `"/bin/sh"`) to invoke the shell. We can find the address of `system` and `exit` using `GDB`. But how to get the address of string `"/bin/sh"`, which is not present even in the program.  
Again environment variable can help us in the same as they are also pushed on the top of stack. Create one environment variable like:  
`export SHELL="/bin/sh"`  
Run GDB, and print the values of say 500 string entries from `$esp`.  
`gef> x/100s $esp`  
And see where your string lies. Now we have all these address, fill the exploit template :cool: we did that as well.

#### What we learnt?
The stack will be shifted by one if we have to return from one function to mimic the call to another function. That's why extra 4 bytes of 'CCCC' is added in the exploit pay load.  
Also this exploitation allows use bypass ASLR protection.

