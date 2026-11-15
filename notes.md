## 01_uninit_local.c
Prediction: Compile time error
Why I think that: Because x is declared as a type int, but isn't actually assigned a value
Actual: Warning that x is used uninitialized, but still returns an integer, so still a running program
What I got wrong and why: Reading an uninitialized local variable is what C standard call undefined behavior

## 02_stack_reuse.c
Prediction: Same thing compile time error again
Why I think that: The variable x is again not assigned a value
Actual: Warning that x is used uninitialized, but still returns an integer, so still a running program
What I got wrong and why: Reading an uninitialized local variable is what C standard call undefined behavior

## 03_stack_address.c
Prediction: Two addresses will be the same
Why I think that: Because since x isn't assigned a value, it has no place in memory, so it would point to the next thing in memory which would be secret
Actual: Addresses of secret and x were the same
What I got wrong and why: Prediction was right, reasoning was wrong, declaring int x reserves 4 bytes in peek's stack frame, but nothing is written into those bytes, x is a slot that happens to be the same physical bytes secret used earlier

## 04_clobbered.c
Prediction: peek will still print 211
Why I think that: printf doesn't occupy anything in the stack so the program will still run the same
Actual: Printed hello, and the number returned was not 211
What I got wrong and why: Whenever a function gets called, it takes up memory in the stack so it overwrote the bytes where 211 was stored

## 05_heap_reuse.c
Prediction: I predict that fresh has 8 garbage value, and reused has same 8 garbage values as fresh, same with calloc
Why I think that: They are just pointers with no stored value, so they don't get overwritten
Actual: fresh has 8 0 values so does calloc, reused has 4 211, 2 0's and 2 garbage values
What I got wrong and why: When we malloc, operating system fills it with 0's, reused frees first 16 bytes but last 16 bytes still store same values so they are 211, calloc it's job is to 0 everything out

## 06_leak.c
Prediction: The reply 64 bytes get sent so we'd get OK0000000000m whole password doesn't appear, -Wall does warn
Why I think that: Whole password doesn't appear because we never use it we only use reply isntead of password, -Wall does warn because we aren't dynamically allocating any memory everything is in the stack
Actual: Both passwords came out and -Wall printed nothing
What I got wrong and why: check_login returns, and its 64 bytes are not erased. The password is still on the stack.
send_reply is called and reply lands on the same 64 bytes. strcpy(reply, "OK") overwrites only the first few.
fwrite sends all 64 bytes, including the ones send_reply never wrote, which still hold the old password.

## 07_leak_fixed_length.c
Prediction: send_reply will only send reply
Why I think that: in fwrite instead of using sizeof(reply) i did strlen(reply) so it should only print 3 characters
Actual: Prediction was correct
What I got wrong and why: Nothing

## 08_leak_fixed_init.c
Prediction: still send_reply print reply, but the array won't be filled with parts of password filled with only 0's
Why I think that: we initialized reply to be filled with 0's
Actual: Predicition was correct
What I got wrong and why: Nothing

## 09_wipe.c
Prediction: For -00 password doesn't leak, for -02 password leaks
Why I think that: Honestly I don't know why and I guessed
Actual: Prediction was right
What I got wrong and why: memset is an overwrite and overwrites what's stored with 0's. check_login never reads password again after that line. Now connect it to part 1. In part 1, int secret = 211; was a write that nothing ever read. The optimizer deleted it, and that’s why you got 0 and not 211. Here, the memset is also a write that nothing ever reads. The optimizer deleted it for the same reason. The wipe isn’t in the -O2 program at all. This is called dead store elimination: a “store” is a write to memory, and it’s “dead” if nothing reads the result. By C’s rules the compiler may do this, because a correct program can’t tell the difference. The only code that can see whether the wipe happened is code that reads leftover memory, which is the bug send_reply has. So the compiler removed the safety measure because the thing it guards against isn’t supposed to be possible.