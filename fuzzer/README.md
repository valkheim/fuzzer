# demo

Get some infos about your target:
- snapshot entry/exit point
- checkpoints
- input location

```
target/ $ make dump
target/ $ tail dump.txt
───────────────────────────────────────────────────────────────────────────────────────── trace ────
[#0] 0x5555555552be → print_input(input=0x5555555592a0)
[#1] 0x5555555554db → main(ac=0x2, av=0x7fffffffe3d8)
────────────────────────────────────────────────────────────────────────────────────────────────────
$1 = 0x55555555a4b0 'B' <repeats 20 times>
$2 = {void (char *, int)} 0x5555555552dd <check1>
$3 = {void (char *, int)} 0x555555555319 <check2>
$4 = {void (char *, int)} 0x555555555355 <check3>
$5 = {void (char *, size_t)} 0x555555555391 <vulnerable>
$6 = {<text variable, no debug info>} 0x5555555550e0 <exit@plt>
```

Now let's prepare `fuzzer/srcs/main.c` with those values:
```
[...]
fuzzer->boundaries.begin = 0x5555555552be;
fuzzer->boundaries.end = 0x00005555555550e0; // call exit
fuzzer->input_location = 0x55555555a4b0;
breakpoint_t checkpoints[4] = {
  { 0x5555555552dd, 0 }, // check1
  { 0x555555555319, 0 }, // check2
  { 0x555555555355, 0 }, // check3
  { 0x555555555391, 0 }, // vulnerable
};
[...]
```

Then get some data to feed our `demo` app:
```
data/ $ ./generate_initial.py > initial.bin
data/ $ xxd -g1 initial.bin
00000000: 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42  BBBBBBBBBBBBBBBB
00000010: 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42  BBBBBBBBBBBBBBBB
00000020: 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42  BBBBBBBBBBBBBBBB
00000030: 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42  BBBBBBBBBBBBBBBB
00000040: 42 42                                            BB
```

Ok now we can start fuzzing

```
fuzzer/ $ make
fuzzer/ $ ./fuzzer ../target/demo ../data/some.bin
execs:        171.2 millions
execs:        36415 per second
checkpoints:  4/4
crashes:      1
```

When a crash occurs, an output is available in `fuzzer/crashes/`:
```
/fuzzer $ xxd -g1  crashes/sigabrt.7ffff7e0b614
00000000: d7 41 35 7b 41 41 41 41 41 fd 41 41 7d f0 41 41  .A5{AAAAA.AA}.AA
00000010: 41 41 41 41 db 79 41 41 57 95 18 41 41 74 41 35  AAAA.yAAW..AAtA5
00000020: 08 41 41 41 48 41 45 53 79 41 41 02 cd 62 4b 41  .AAAHAESyAA..bKA
00000030: 41 41 41 41 67 41 3a e9 41 41 41 99 41 41 41 41  AAAAgA:.AAA.AAAA
00000040: a6 a2 41 41 41 57 d8 41 41 2a 41 fc 41 5b 41 41  ..AAAW.AA*A.A[AA
00000050: 35 55 6a ac 41 41 41 d8 44 41 41 41 41 41 41 86  5Uj.AAA.DAAAAAA.
00000060: 41 29 41 41 41 41 b7 41 41 6d ef 41 41 b7 41 41  A)AAAA.AAm.AA.AA
00000070: 41 41 d6 30 fc 41 b4 dd 41 89 f8 41 41 41 f9 41  AA.0.A..A..AAA.A
00000080: 01 41 92 ed 6c 2d 41 41 41 cc 41 d4 41 41 bc 41  .A..l-AAA.A.AA.A
00000090: 41 41 e1 41 0d 01 41 41 41 41 41 41 d8 3e 41 41  AA.A..AAAAAA.>AA
000000a0: a5 41 41 f8 41 72 c8 41 41 20 41 41 0c 03 41 c8  .AA.Ar.AA AA..A.
000000b0: c9 4c 7f ed 7f 41 41 19 e0 fa 41 88 f6 41 1e 41  .L...AA...A..A.A
000000c0: b4 41 41 41 41 41 41 9b 57 41 41 41 41 14 41 41  .AAAAAA.WAAAA.AA
000000d0: 41 5f 41 41 41 41 41 41 41 41 4e 53 41 41 41 be  A_AAAAAAAANSAAA.
000000e0: 41 ea 38 f1 41 41 ae 41 9d 41 41 41 41 ad 41 41  A.8.AA.A.AAAA.AA
000000f0: 41 41 cb 92 41 41 c9 41 f6 41 1b 25 0c 41 41 16  AA..AA.A.A.%.AA.
00000100: ec 41 41 52 41 41 41 69 61 f6 41 41 21 41 35 41  .AARAAAia.AA!A5A
00000110: 41 41 41 41 41 41 41 47 d0 42 bc 41 41 41 41 a8  AAAAAAAG.B.AAAA.
00000120: 96 41 d3 82 41 41 41 41 78 5a 02 41 41 41 41 41  .A..AAAAxZ.AAAAA
00000130: 41 f3 97 41 47 41 41 41 41 41 41 10 41 54 41 41  A..AGAAAAAA.ATAA
00000140: 41 4b 84 41 bc 41 0e d5 41 41 da 40 41 41 41 41  AK.A.A..AA.@AAAA
00000150: 41 70 41 41 41 41 1c 41 41 41 41 4e 41 41 41 8a  ApAAAA.AAAANAAA.
00000160: 41 18 41 41 60 41 26 41 41 0d 74 86 41 41 41 62  A.AA`A&AA.t.AAAb
00000170: 41 2e 41 5a 41 41 d1 41 41 e3 41 41 41 41 3f 41  A.AZAA.AA.AAAA?A
00000180: 41 91 64 41 41 41 e1 8c 41 44 41 31 27 41 41 a5  A.dAAA..ADA1'AA.
00000190: 0a                                               .
```

So we can reproduce the crash:

```
~/workspace/fuzz/target gdb demo
77 commands loaded for GDB 9.2 using Python engine 3.8
Reading symbols from demo...
gef➤  run ../fuzzer/crashes/sigabrt.7ffff7e0b614
Starting program: /home/valkheim/workspace/fuzz/target/demo ../fuzzer/crashes/sigabrt.7ffff7e0b614
AAAAAA>AAAAArAA AAAAWAAtAAAAHAESyAAbKAAAAAgA:AAAAAAAAAAWAA*AA[AA5UjAAADAAAAAAA)AAAAAAmAAAAAA0AAAAAAAl-AAAAAAAAAA
                  ALAAAAAAAAAAAWAAAAAAA_AAAAAAAANSAAAA8AAAAAAAAAAA˒AAAA
tAAAbA.AZAAAAAAAA?AAdAAAADA1'AA                                        AAARAAAiaAA!A5AAAAAAAAGBAAAAAӂAAAAxZAAAAAAAGAAAAAAATAAAKAAAA@AAAAApAAAAAAAANAAAAAA`A&AA
*** stack smashing detected ***: terminated

Program received signal SIGABRT, Aborted.

[...]

───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── threads ────
[#0] Id 1, Name: "demo", stopped 0x7ffff7e0b615 in raise (), reason: SIGABRT
─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────── trace ────
[#0] 0x7ffff7e0b615 → raise()
[#1] 0x7ffff7df4862 → abort()
[#2] 0x7ffff7e4d5e8 → __libc_message()
[#3] 0x7ffff7edda3a → __fortify_fail()
[#4] 0x7ffff7edda04 → __stack_chk_fail()
[#5] 0x555555555473 → vulnerable(buffer=0xbe414141534e4141 <error: Cannot access memory at address 0xbe414141534e4141>, length=0x4141414141415f41)
──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
```
