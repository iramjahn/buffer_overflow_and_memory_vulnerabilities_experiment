# Uninitialized Memory in C

COMP 211 Checkoff 3: Buffer Overflows and Memory Vulnerabilities, sub-topic Uninitialized Memory

## What this is

uninitialized memory is memory your program has been given but hasn’t written to yet, 
like a local variable you declared without a value or a buffer you just got from malloc. 
it isn’t empty, it still holds whatever the last thing that used that spot left behind. 
that’s a security problem and not just a bug because the leftover can be something sensitive 
like a password or a pointer, so if your program prints or sends that memory, it’s handing 
out data that the person on the other end was never supposed to see.

## What I explored

I started with a pretty basic question: what actually happens in C if you use a variable you 
never gave a value to? I figured it would be a compile error, and when it wasn’t, that turned 
into where does that number come from. that led to the next question, which was whether leftover
memory could ever be something that matters, like a password, and then once we saw it could,
whether wiping the memory before returning actually fixes it. for each program I wrote down 
what I thought would happen before running it, and I was wrong on most of them, which is 
basically where the three discoveries came from. the predictions and what actually happened
are all in notes.md.

## Most interesting discoveries

### Discovery 1

so we declared an int x but didn't assign it any value (`01_uninit_local.c`), 
and I expected a compile time error. instead the program compiled fine and printed 
out a number. what actually happened is the compiler set aside a spot in memory for x, 
but since we never put anything in it, whatever was in that spot before was still sitting 
there, and that's what got printed. in `02_stack_reuse.c` the number was 211, and that's
because set_secret had just used that same spot and left 211 behind when it returned.
same kind of thing happens with malloc (`05_heap_reuse.c`): the first malloc gave us zeros, 
but after we freed it and called malloc again, some of the old values were still in there.

### Discovery 2

here (`06_leak.c`) check_login has an array of size 64 that stores our password. 
then after it returns, send_reply makes its own array of size 64 but only puts 3 characters
in it. so when we sent the second array I expected to just see those 3 characters. instead 
we got the 3 characters and then the rest of the password leaked along with them. the reason is 
that when a function returns, its memory isn't in use anymore but it's still there, nobody 
clears it. the second array ended up in the same place in memory as the password, so the
3 characters overwrote the first 3 characters of the password and the rest just stayed.
and the actual bug is that send_reply only wrote 3 bytes but told fwrite to send 
sizeof(reply), which is the whole 64. two ways to fix it are in `07_leak_fixed_length.c`
and `08_leak_fixed_init.c`.

### Discovery 3

this one (`09_wipe.c`) is the same program as discovery 2 except we tried to fix it,
so at the end of check_login there's a memset that overwrites the password with zeros.
you'd expect that to stop the leak, and when we compile normally with -O0 it does. 
but when we compile with -O2, which turns on optimization and is how released software 
is usually built, the password leaks again. 
what's happening is the compiler looks at the memset and sees that nothing ever reads the
password after it, so it figures writing those zeros is pointless and just skips it. so the 
compiler basically threw out our fix. to get around that we used explicit_bzero instead 
(`10_wipe_explicit.c`), which the compiler isn't allowed to skip, and with that the 
password doesn't leak even at -O2. the output still wasn't completely clean though, because 
send_reply was still sending bytes it never wrote, so the real fix is the one in 
`07_leak_fixed_length.c` and `08_leak_fixed_init.c`.

## Running the experiments

Environment used: GCC 13.3.0 on Ubuntu 24.04, x86-64. Output depends on the
compiler, operating system and optimization level, so results may differ
elsewhere.

All commands are run from the repository root.

| File | Command | What it shows |
|---|---|---|
| `01_uninit_local.c` | `gcc -O0 -Wall -o prog experiments/01_uninit_local.c && ./prog` | prints leftover data on stack even when x is not assigned value |
| `02_stack_reuse.c` | `gcc -O0 -Wall -o prog experiments/02_stack_reuse.c && ./prog` | prints 211, even though printing x that has no value |
| `03_stack_addresses.c` | `gcc -O0 -Wall -o prog experiments/03_stack_addresses.c && ./prog` | prints address of secret and x, and it's the same address |
| `04_clobbered.c` | `gcc -O0 -Wall -o prog experiments/04_clobbered.c && ./prog` | prints hello and x prints leftover data on stack even when not assigned value, and printf reuses stack space |
| `05_heap_reuse.c` | `gcc -O0 -Wall -o prog experiments/05_heap_reuse.c && ./prog` | prints contents of three dynamically allocated arrays, reused has leftover values from fresh, calloc 0's everything out |
| `06_leak.c` | `gcc -O0 -Wall -o prog experiments/06_leak.c && ./prog \| cat -v` | prints password with 3 characters overwritten by reply |
| `07_leak_fixed_length.c` | `gcc -O0 -Wall -o prog experiments/07_leak_fixed_length.c && ./prog \| cat -v` | prints right length of reply, sending only bytes that were written |
| `08_leak_fixed_init.c` | `gcc -O0 -Wall -o prog experiments/08_leak_fixed_init.c && ./prog \| cat -v` | initializes reply to all 0's so password doesn't leak |
| `09_wipe.c` | `gcc -O2 -Wall -o prog experiments/09_wipe.c && ./prog hunter2-my-real-password \| cat -v` | -O0 password wipe works at -O2 it doesn't |
| `10_wipe_explicit.c` | `gcc -O2 -Wall -o prog experiments/10_wipe_explicit.c && ./prog hunter2-my-real-password \| cat -v` | -O2 password disappears, swap memset with explicit_bzero |

Other commands used:

- Same source, optimized: `gcc -O2 -o prog experiments/02_stack_reuse.c && ./prog`
- Valgrind: `gcc -O0 -g -o prog experiments/06_leak.c && valgrind ./prog`
- Automatic zero-initialization: `gcc -O0 -ftrivial-auto-var-init=zero -o prog experiments/06_leak.c && ./prog | cat -v`

## Files

- `notes.md`: my prediction, why I predicted what I did, the actual result, and what I got wrong for each experiment
- `experiments/`: the C programs

## Resources

- https://claude.ai/share/1aeb6a38-3109-4c49-8e42-cfee166043dc
