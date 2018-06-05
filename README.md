# Binary Exploitation 

#### Any Doubt...? [Let's Discuss](https://gitter.im/BinaryExploitation/Lobby?utm_source=share-link&utm_medium=link&utm_campaign=share-link)
> ## Introduction
> I am quite passionate about exploiting binary files. First time when I came across Buffer Overflow(a simple technique of exploitation) then I was not able to implement the same with the same copy of code on my system. 
The reason for that was there was no consolidated document that would guide me thoroughly to write a perfect exploit payload for the program in case of system changes.  
> Also there are very few descriptive blogs/tutorials that had helped me exploiting a given binary. 
> I have come up with consolidation of Modern exploitation techniques (in the form of tutorial) that will allow you to understand exploitation from scratch.

I will be using [vagrant file](Vagrantfile) to setup the system on virtual box. To do  the same in your system follow:
1. `vagrant up`
2. `vagrant ssh`

## Topics

1. **[Lecture 1.](Lecture1/README.md)**  
     * Memory Layout of C program.
     * ELF binaries.
     * Overview of stack during function call.
     * Assembly code for the function call and return.
     * Concept of `$ebp` and `$esp`.
     * Executable memory.

1. **[Lecture 1.5.](Lecture1.5/README.md)**
    * How Linux finds the binaries utilis?
    * Simple exploit using Linux $PATH variable.

1. **[Lecture 2.](Lecture2/README.md)**
	* What are stack overflows?
	* **ASLR** (basics), avoiding **Stack protection**.
	* Shellcodes
	* Buffer overflow:
		*  Changing Control of the program to return to some other function
		*  Shellcode injection in buffer and spawning the shell

1. **[Lecture 3.](Lecture3/README.md)**
	* **Shellcode** injection with **ASLR** enabled.
		* Environment variables.

1. **[Lecture 3.5](Lecture3.5/README.md)**
    * Return to Libc attacks.
    * Spawning `shell` in **non executable stack**
	* Stack organization in case `ret2libc` attack.

1. **[Lecture 4.](Lecture4/)**
    * This folder contains the set of questions to exploit binaries on the concept that we have learned so far.

1. **[Lecture 5.](Lecture5/README.md)**
    * What is format string Vulnerability?
    * Seeing the content of stack.
    * Writing onto the stack.
    * Writing to arbitrary memory location.

1. **[Lecture 6.](Lecture6/README.md)**
    * GOT
    * Overriding GOT entry.
    * Spawning shell with format string vuln.

1. **[Lecture 7.](Lecture7/README.md)**
    * Heaps
    * Arena, Bins, Chunks.
    * Use after free exploit.
    * Double free exploit.
