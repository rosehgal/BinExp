# Binary Exploitation 

> ## Introduction
> I am quite passionate about exploiting binary files. First time when I came across Buffer Overflow(a simple technique of exploitation) then I was not able to implement the same with the same copy of code on my system. 
The reason for that was there was no consolidated document that would guide me thoroughly to write a perfect exploit payload for the program in case of system changes.  
> Also there are very few descriptive blogs/tutorials that had helped me exploiting a given binary. 
> I have come up with consolidation of Modern exploitation techniques (in the form of tutorial) that will allow you to understand exploitation from scratch.

## Topics

1. **[Lecture 1.](Lecture1/Lecture1.md)**
  	* Memory Layout of C program.
    	* ELF binaries.
  	* Overview of stack during function call.
  	* Assembly code for the function call and return.
  	* Concept of `$ebp` and `$esp`.
  	* Executable memory.

1. **[Lecture 2.](Lecture2/Lecture2.md)**
	* What are stack overflows?
	* **ASLR** (basics), avoiding **Stack protection**.
	* Shellcodes
	* Buffer overflow:
		*  Changing Control of the program to return to some other function
		*  Shellcode injection in buffer and spawning the shell

1. **[Lecture 3.](Lecture3/Lecture3.md)**
	* **Shellcode** injection with **ASLR** enabled.
		* Environment variables.
	* Return to Libc attacks.
		* Spawning `shell` in **non executable stack**
		* Stack organization in case `ret2libc` has to return to some other function.
