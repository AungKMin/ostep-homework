1. First run with the flags -n 10 -H 0 -p BEST -s 0 to generate a few random allocations and frees. Can you predict what alloc()/free() will return? Can you guess the state of the free list after
each request? What do you notice about the free list over time?

```
python3 malloc.py -n 10 -H 0 -p BEST -s 0 
seed 0
size 100
baseAddr 1000
headerSize 0
alignment -1
policy BEST
listOrder ADDRSORT
coalesce False
numOps 10
range 10
percentAlloc 50
allocList 
compute False

ptr[0] = Alloc(3) returned ? (1000, searched 1 element)
List? ([ Size 1 ]: [ addr: 1003 sz:97 ])

Free(ptr[0]) returned ? (0 - success)
List? ([ Size 2 ]: [addr: 1000 sz:3 ] [ addr: 1003 sz:97 ])

ptr[1] = Alloc(5) returned ? (1003, searched 2 elements)
List? ([ Size 2 ]: [addr: 1000 sz:3 ] [ addr: 1008 sz:92 ])

Free(ptr[1]) returned ? (0 - success)
List?  ([ Size 3 ]: [addr: 1000 sz:3 ] [ addr: 1003 sz:5 ] [ addr: 1008 sz:92 ] )

ptr[2] = Alloc(8) returned ? (1008, searched 3 elements)
List?  ([ Size 3 ]: [addr: 1000 sz:3 ] [ addr: 1003 sz:5 ] [ addr: 1016 sz: 84 ])

Free(ptr[2]) returned ? (0 - success)
List? ([ Size 4 ]: [addr: 1000 sz:3 ] [ addr: 1003 sz:5 ] [ addr: 1008 sz: 8 ] [ addr: 1016 sz: 84 ])

ptr[3] = Alloc(8) returned ? (1008, searched 4 elements)
List? ([ Size 3 ]: [addr: 1000 sz:3 ] [ addr: 1003 sz:5 ] [ addr: 1016 sz: 84 ])

Free(ptr[3]) returned ? (0 - success)
List?  ([ Size 4 ]: [addr: 1000 sz:3 ] [ addr: 1003 sz:5 ] [ addr: 1008 sz: 8 ] [ addr: 1016 sz: 84 ])

ptr[4] = Alloc(2) returned ? (1000, searched 4 elements)
List? ([ Size 4 ]: [addr: 1002 sz:1 ] [ addr: 1003 sz:5 ] [ addr: 1008 sz: 8 ] [ addr: 1016 sz: 84 ])

ptr[5] = Alloc(7) returned ? (1008, searched 4 elements)
List? ([ Size 4 ]: [addr: 1002 sz:1 ] [ addr: 1003 sz:5 ] [ addr: 1015 sz: 1 ] [ addr: 1016 sz: 84 ])
```

2. How are the results different when using a WORST fit policy to search the free list (-p WORST)? What changes?

```
python3 malloc.py -n 10 -H 0 -p WORST -s 0
seed 0
size 100
baseAddr 1000
headerSize 0
alignment -1
policy WORST
listOrder ADDRSORT
coalesce False
numOps 10
range 10
percentAlloc 50
allocList 
compute False

ptr[0] = Alloc(3) returned ? (1000, searched 1 elements)
List? ([ Size 1 ]: [ addr: 1003 sz:97 ])

Free(ptr[0]) returned ? (0 - success)
List? ([ Size 2 ]: [ addr: 1000 sz:3 ] [ addr: 1003 sz:97 ])

ptr[1] = Alloc(5) returned ? (1003, searched 2 elements)
List? ([ Size 2 ]: [ addr: 1000 sz:3 ] [ addr: 1008 sz:92 ])

Free(ptr[1]) returned ? (0 - success)
List? ([ Size 3 ]: [ addr: 1000 sz:3 ] [ addr: 1003 sz:5 ] [ addr: 1008 sz:92 ])

ptr[2] = Alloc(8) returned ? (1008, searched 3 elements)
List? ([ Size 3 ]: [ addr: 1000 sz:3 ] [ addr: 1003 sz:5 ] [ addr: 1016 sz:84 ])

Free(ptr[2]) returned ? (0 - success)
List? ([ Size 4 ]: [ addr: 1000 sz:3 ] [ addr: 1003 sz:5 ] [ addr: 1008 sz:8 ] [ addr: 1016 sz:84 ])

ptr[3] = Alloc(8) returned ? (1016, searched 4 elements)
List? ([ Size 4 ]: [ addr: 1000 sz:3 ] [ addr: 1003 sz:5 ] [ addr: 1008 sz:8 ] [ addr: 1024 sz:76 ])

Free(ptr[3]) returned ? (0 - success)
List? ([ Size 5 ]: [ addr: 1000 sz:3 ] [ addr: 1003 sz:5 ] [ addr: 1008 sz:8 ] [ addr: 1016 sz:8 ] [ addr: 1024 sz:76 ])

ptr[4] = Alloc(2) returned ? (1024, searched 5 elements)
List? ([ Size 5 ]: [ addr: 1000 sz:3 ] [ addr: 1003 sz:5 ] [ addr: 1008 sz:8 ] [ addr: 1016 sz:8 ] [ addr: 1026 sz:74 ])

ptr[5] = Alloc(7) returned ? (1026, searched 5 elements)
List? [ addr: 1000 sz:3 ] [ addr: 1003 sz:5 ] [ addr: 1008 sz:8 ] [ addr: 1016 sz:8 ] [ addr: 1033 sz:67 ])
```

3. What about when using FIRST fit (-p FIRST)? What speeds up
when you use first fit?

```
python3 malloc.py -n 10 -H 0 -p FIRST -s 0 -c
seed 0
size 100
baseAddr 1000
headerSize 0
alignment -1
policy FIRST
listOrder ADDRSORT
coalesce False
numOps 10
range 10
percentAlloc 50
allocList 
compute True

ptr[0] = Alloc(3) returned 1000 (searched 1 elements)
Free List [ Size 1 ]: [ addr:1003 sz:97 ]

Free(ptr[0])
returned 0
Free List [ Size 2 ]: [ addr:1000 sz:3 ][ addr:1003 sz:97 ]

ptr[1] = Alloc(5) returned 1003 (searched 2 elements)
Free List [ Size 2 ]: [ addr:1000 sz:3 ][ addr:1008 sz:92 ]

Free(ptr[1])
returned 0
Free List [ Size 3 ]: [ addr:1000 sz:3 ][ addr:1003 sz:5 ][ addr:1008 sz:92 ]

ptr[2] = Alloc(8) returned 1008 (searched 3 elements)
Free List [ Size 3 ]: [ addr:1000 sz:3 ][ addr:1003 sz:5 ][ addr:1016 sz:84 ]

Free(ptr[2])
returned 0
Free List [ Size 4 ]: [ addr:1000 sz:3 ][ addr:1003 sz:5 ][ addr:1008 sz:8 ][ addr:1016 sz:84 ]

ptr[3] = Alloc(8) returned 1008 (searched 3 elements)
Free List [ Size 3 ]: [ addr:1000 sz:3 ][ addr:1003 sz:5 ][ addr:1016 sz:84 ]

Free(ptr[3])
returned 0
Free List [ Size 4 ]: [ addr:1000 sz:3 ][ addr:1003 sz:5 ][ addr:1008 sz:8 ][ addr:1016 sz:84 ]

ptr[4] = Alloc(2) returned 1000 (searched 1 elements)
Free List [ Size 4 ]: [ addr:1002 sz:1 ][ addr:1003 sz:5 ][ addr:1008 sz:8 ][ addr:1016 sz:84 ]

ptr[5] = Alloc(7) returned 1008 (searched 3 elements)
Free List [ Size 4 ]: [ addr:1002 sz:1 ][ addr:1003 sz:5 ][ addr:1015 sz:1 ][ addr:1016 sz:84 ]
```

4. For the above questions, how the list is kept ordered can affect the
time it takes to find a free location for some of the policies. Use
the different free list orderings (-l ADDRSORT, -l SIZESORT+,
-l SIZESORT-) to see how the policies and the list orderings interact.

- For BEST and WORST, the allocation shouldn't change. But for FIRST, could change.
- FIRST policy with SIZESORT+ allocates the same way as BEST
- FIRST policy with SIZESORT- allocates the same way as WORST

5. Coalescing of a free list can be quite important. Increase the number
of random allocations (say to -n 1000). What happens to larger
allocation requests over time? Run with and without coalescing
(i.e., without and with the -C flag). What differences in outcome do
you see? How big is the free list over time in each case? Does the
ordering of the list matter in this case?

- Without coalescing, over time, larger memory requests do not get satisfied. 
- ADDRSORT performs better than SIZESORT- and SIZESORT+ because only adjacent memory blocks which are in address order can be coalesced.  

6. What happens when you change the percent allocated fraction -P
to higher than 50? What happens to allocations as it nears 100?
What about as the percent nears 0?
- Eventually, there is no more space to allocate without freeing.

7. What kind of specific requests can you make to generate a highlyfragmented free space? Use the -A flag to create fragmented free lists, and see how different policies and options change the organization of the free list.
```
// When sorted by SIZESORT+ and allocated memory sizes increase to the right of the adddress space, when freeing the memory, the chunks are ordered in the opposite way as they should be to be coalesced peroperly.
python3 malloc.py -H 0 -p FIRST -c -C -l SIZESORT+ -A +4,+3,+2,+1,-0,-1,-2,-3
// Same idea for SIZESORT- but the memory sizes are reveresed
python3 malloc.py -H 0 -p FIRST -c -C -l SIZESORT- -A +1,+2,+3,+4,-0,-1,-2,-3
```