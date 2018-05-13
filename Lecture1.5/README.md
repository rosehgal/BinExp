# Exploiting Linux utils search.
[Video Hindi](https://www.youtube.com/watch?v=VVFnA7yYFqE&t=42s)

In Linux the file under the directory say `/bin/*` are nothing but binary
executable that allows the user to do day to day tasks. All these binaries are
not stored in one location only but many. To see the set of all the locations
where the system binaries are present you print your `$PATH` variable.  
`export $PATH`  
`/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin`  
There is the list of locations we have.

```C
#include <stdio.h>
#include <stdlib.h>

int main()
{
    system("ls /tmp/file.dat");
    return 0;
}
```
Suppose you are asked that what this program does? You would answer that, it
check if file exits or not, if exists, it will print the name again.  
We are interested to exploit this, but to exploit we need to understand the
execution flow about how this program is working.  


When this program is started, the it will call the `system()` system call to
execute `ls /tmp/file.dat`. `System` is the one of the Linux system calls that
would execute the shell and in the same shell it would execute the command
provided.
Since `system("ls ..")` is executing `ls` Linux utility, it would be easy to
guess that, it will start searching the shells `$PATH` variable and will start
exploring the directories in the same variable. If it find it in the first one
will execute that `ls` if not keep it search for the next directory.

### Can I change the `$PATH`?
`$PATH` is an shells environment variable and it can manipulated by the user of that
shell.  
Suppose I create one binary file in the `$PWD`, make it name as `ls` and then
tell the `$PATH` about the current `$PWD` as the first location to search for,
then if the system start searching for `ls` it will end up with my `ls`. :cool:
We have exploited this as well.

### Crafting the actual exploit.
1. Update the `$PATH` to point to current directory first.  
   `export PATH=$PWD:$PATH`
2. Copy some binary say `cat` to as name of `ls` in current directory.  
   ``cp `which cat` ls``
3. [Optional] to check if the path is updated.
   `which ls` and you should get something like : `/home/vagrant` which is my
   current directory.  
4. Run the binary. :metal:

You will notice that binary instead of running the `ls` will run the `cat`.
:smile:

