1. First let’s use a tiny address space to translate some addresses. Here’s
a simple set of parameters with a few different random seeds; can
you translate the addresses?

segmentation.py -a 128 -p 512 -b 0 -l 20 -B 512 -L 20 -s 0

segmentation.py -a 128 -p 512 -b 0 -l 20 -B 512 -L 20 -s 1

segmentation.py -a 128 -p 512 -b 0 -l 20 -B 512 -L 20 -s 2

```
python3 segmentation.py -a 128 -p 512 -b 0 -l 20 -B 512 -L 20 -s 0
ARG seed 0
ARG address space size 128
ARG phys mem size 512

Segment register information:

  Segment 0 base  (grows positive) : 0x00000000 (decimal 0)
  Segment 0 limit                  : 20

  Segment 1 base  (grows negative) : 0x00000200 (decimal 512)
  Segment 1 limit                  : 20

Virtual Address Trace
  VA  0: 0x0000006c (decimal:  108) --> PA or segmentation violation?
  VA  1: 0x00000061 (decimal:   97) --> PA or segmentation violation?
  VA  2: 0x00000035 (decimal:   53) --> PA or segmentation violation?
  VA  3: 0x00000021 (decimal:   33) --> PA or segmentation violation?
  VA  4: 0x00000041 (decimal:   65) --> PA or segmentation violation?

For each virtual address, either write down the physical address it translates to
OR write down that it is an out-of-bounds address (a segmentation violation). For
this problem, you should assume a simple address space with two segments: the top
bit of the virtual address can thus be used to check whether the virtual address
is in segment 0 (topbit=0) or segment 1 (topbit=1). Note that the base/limit pairs
given to you grow in different directions, depending on the segment, i.e., segment 0
grows in the positive direction, whereas segment 1 in the negative. 
```

Ans: 
- The virtual address has 7 bits
- VA 0: x6c 
  - topbit = 1 so segment 1 -> without topbit: 0b101100 = 44 
  - largest the virtual address can get (without topbit): 0b111111 = 64 - 1 = 63
  - to be valid: largest_va - actual_va + 1 <= size
  - [63 - 44 + 1 = 20] <= 20 therefore, valid
  - physical address = largest physical address - (largest_va - actual_va) = (512 - 1) - (63 - 44) = 492 
- VA 1: x61 
  - topbit = 1 so segment 1 -> without topbit: 0b100001 = 33
  - largest the virtual address can get (without topbit): 0b111111 = 64 - 1 = 63
  - to be valid: largest_va - actual_va + 1 <= size
  - [63 - 33 + 1 = 31] NOT <= 20 therefore, invalid
- VA 2: x35
  - topbit = 0 so segment 0 -> without topbit: 0b100101 = 37
  - to be valid: va < size
  - [37] NOT < 20 therefore, invalid
- VA 3: x21
  - topbit = 0 so segment 0 -> without topbit: 0b100001 = 33
  - to be valid: va < size
  - [33] NOT < 20 therefore, invalid
- VA 4: x41
  - topbit = 1 so segment 1 -> without topbit: 0b000001 = 1 
  - largest the virtual address can get (without topbit): 0b111111 = 64 - 1 = 63
  - to be valid: largest_va - actual_va + 1 <= size
  - [63 - 1 + 1 = 63] NOT <= 20 therefore, invalid

```
python3 segmentation.py -a 128 -p 512 -b 0 -l 20 -B 512 -L 20 -s 1 -c

Virtual Address Trace
  VA  0: 0x00000011 (decimal:   17) --> VALID in SEG0: 0x00000011 (decimal:   17)
  VA  1: 0x0000006c (decimal:  108) --> VALID in SEG1: 0x000001ec (decimal:  492)
  VA  2: 0x00000061 (decimal:   97) --> SEGMENTATION VIOLATION (SEG1)
  VA  3: 0x00000020 (decimal:   32) --> SEGMENTATION VIOLATION (SEG0)
  VA  4: 0x0000003f (decimal:   63) --> SEGMENTATION VIOLATION (SEG0)
```

Ans: 
- VA 0: x11 
  - topbit = 0 so segment 0 -> without topbit: 0b010001 = 17
  - to be valid: va < size
  - [17] < 20 therefore, valid
  - physical address = smallest physical address (base) + va_without_topbit = 0 + 17 = 17
- VA 1: x6c
  - topbit = 1 so segment 1 -> without topbit: 0b101100 = 44
  - largest the virtual address can get (without topbit): 0b111111 = 64 - 1 = 63
  - to be valid: largest_va - actual_va + 1 <= size
  - [63 - 44 + 1 = 20] <= 20 therefore, valid
  - physical address = largest physical address - (largest_va - actual_va) = (512 - 1) - (63 - 44) = 492 
- ... (the same concept throughout)

2. Now, let’s see if we understand this tiny address space we’ve constructed (using the parameters from the question above). What is
the highest legal virtual address in segment 0? What about the lowest legal virtual address in segment 1? What are the lowest and
highest illegal addresses in this entire address space? Finally, how
would you run segmentation.py with the -A flag to test if you
are right?

- highest legal virtual address in segment 0 = base register + size - 1 = 0 + 20 - 1 = 19
- lowest legal virtual address in segment 1 = highest va possible / base register - size + 1 = (128 - 1) - 20 + 1 = 108
- highest illegal address in entire address space: (highest physical address possible) - (size of segment 1) = (0x200 - 1) - 20 = (512 - 1) - 20 = 491
- lowest illegal address in entire address space: (lowest physical address possible) + (size of segment 0) = 0 + 20 = 20

```
python3 segmentation.py -a 128 -p 512 -b 0 -l 20 -B 512 -L 20 -s 1 -A 19,20,107,108 -c
ARG seed 1
ARG address space size 128
ARG phys mem size 512

Segment register information:

  Segment 0 base  (grows positive) : 0x00000000 (decimal 0)
  Segment 0 limit                  : 20

  Segment 1 base  (grows negative) : 0x00000200 (decimal 512)
  Segment 1 limit                  : 20

Virtual Address Trace
  VA  0: 0x00000013 (decimal:   19) --> VALID in SEG0: 0x00000013 (decimal:   19)
  VA  1: 0x00000014 (decimal:   20) --> SEGMENTATION VIOLATION (SEG0)
  VA  2: 0x0000006b (decimal:  107) --> SEGMENTATION VIOLATION (SEG1)
  VA  3: 0x0000006c (decimal:  108) --> VALID in SEG1: 0x000001ec (decimal:  492)
```

3. Let’s say we have a tiny 16-byte address space in a 128-byte physical
memory. What base and bounds would you set up so as to get
the simulator to generate the following translation results for the
specified address stream: valid, valid, violation, ..., violation, valid,
valid? Assume the following parameters:

```
segmentation.py -a 16 -p 128 -A 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 --b0 ? --l0 ? --b1 ? --l1 ?
```

Ans:
- base of segment 0 should be 0
- limit of segment 0 should be 2
- base of segment 1 should be 16
- limit of segment 1 should be 2 
- 
```
 python3 segmentation.py -a 16 -p 128 -A 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 --b0 0 --l0 2 --b1 16 --l1 2 -c
ARG seed 0
ARG address space size 16
ARG phys mem size 128

Segment register information:

  Segment 0 base  (grows positive) : 0x00000000 (decimal 0)
  Segment 0 limit                  : 2

  Segment 1 base  (grows negative) : 0x00000010 (decimal 16)
  Segment 1 limit                  : 2

Virtual Address Trace
  VA  0: 0x00000000 (decimal:    0) --> VALID in SEG0: 0x00000000 (decimal:    0)
  VA  1: 0x00000001 (decimal:    1) --> VALID in SEG0: 0x00000001 (decimal:    1)
  VA  2: 0x00000002 (decimal:    2) --> SEGMENTATION VIOLATION (SEG0)
  VA  3: 0x00000003 (decimal:    3) --> SEGMENTATION VIOLATION (SEG0)
  VA  4: 0x00000004 (decimal:    4) --> SEGMENTATION VIOLATION (SEG0)
  VA  5: 0x00000005 (decimal:    5) --> SEGMENTATION VIOLATION (SEG0)
  VA  6: 0x00000006 (decimal:    6) --> SEGMENTATION VIOLATION (SEG0)
  VA  7: 0x00000007 (decimal:    7) --> SEGMENTATION VIOLATION (SEG0)
  VA  8: 0x00000008 (decimal:    8) --> SEGMENTATION VIOLATION (SEG1)
  VA  9: 0x00000009 (decimal:    9) --> SEGMENTATION VIOLATION (SEG1)
  VA 10: 0x0000000a (decimal:   10) --> SEGMENTATION VIOLATION (SEG1)
  VA 11: 0x0000000b (decimal:   11) --> SEGMENTATION VIOLATION (SEG1)
  VA 12: 0x0000000c (decimal:   12) --> SEGMENTATION VIOLATION (SEG1)
  VA 13: 0x0000000d (decimal:   13) --> SEGMENTATION VIOLATION (SEG1)
  VA 14: 0x0000000e (decimal:   14) --> VALID in SEG1: 0x0000000e (decimal:   14)
  VA 15: 0x0000000f (decimal:   15) --> VALID in SEG1: 0x0000000f (decimal:   15)
```

4. Assume we want to generate a problem where roughly 90% of the
randomly-generated virtual addresses are valid (not segmentation
violations). How should you configure the simulator to do so?
Which parameters are important to getting this outcome?

Ans: The combined size / limit (-l) of segment 0 and 1 should be around 90% of the virtual address space (-a)

5. Can you run the simulator such that no virtual addresses are valid?
How?

Ans: Let the size / limit (-l, -L) of segment 0 and 1 be 0