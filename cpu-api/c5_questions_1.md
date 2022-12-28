1. Run ./fork.py -s 10 and see which actions are taken. Can you
predict what the process tree looks like at each step? Use the -c
flag to check your answers. Try some different random seeds (-s)
or add more actions (-a) to get the hang of it.

    ```sh
    python3 fork.py -s 10 -c

                            Process Tree:
                                a

    Action: a forks b
                                a
                                └── b
    Action: a forks c
                                a
                                ├── b
                                └── c
    Action: c EXITS
                                a
                                └── b
    Action: a forks d
                                a
                                ├── b
                                └── d
    Action: a forks e
                                a
                                ├── b
                                ├── d
                                └── e
    ```

2. One control the simulator gives you is the fork percentage, controlled by the -f flag. The higher it is, the more likely the next
action is a fork; the lower it is, the more likely the action is an
exit. Run the simulator with a large number of actions (e.g., -a 100) and vary the fork percentage from 0.1 to 0.9. What do you
think the resulting final process trees will look like as the percentage changes? Check your answer with -c.

        Ans: if there is a higher chance of an exit, the final process tree will be smaller and if higher chance of a fork, the final process tree will be larger  

    ```sh
    python3 fork.py -s 10 -a 100 -f 0.1 -F -c 

    Final Process Tree:
                                a
    ```

    ```sh
    python3 fork.py -s 10 -a 100 -f 0.9 -F -c 

    Final Process Tree:
                               a
                               ├── b
                               │   ├── f
                               │   │   ├── g
                               │   │   │   ├── k
                               │   │   │   ├── B
                               │   │   │   │   └── V
                               │   │   │   │       ├── aa
                               │   │   │   │       │   ├── ai
                               │   │   │   │       │   │   ├── al
                               │   │   │   │       │   │   └── an
                               │   │   │   │       │   └── ar
                               │   │   │   │       └── av
                               │   │   │   └── aq
                               │   │   ├── E
                               │   │   │   └── ag
                               │   │   └── W
                               │   │       └── at
                               │   ├── n
                               │   │   └── J
                               │   │       ├── af
                               │   │       │   ├── ak
                               │   │       │   │   └── aG
                               │   │       │   └── aD
                               │   │       └── az
                               │   │           └── aM
                               │   ├── U
                               │   └── aN
                               ├── c
                               │   ├── i
                               │   │   ├── p
                               │   │   └── x
                               │   │       └── Y
                               │   ├── y
                               │   │   ├── X
                               │   │   │   └── ac
                               │   │   ├── Z
                               │   │   │   ├── ay
                               │   │   │   └── aC
                               │   │   └── ab
                               │   ├── z
                               │   │   ├── F
                               │   │   ├── I
                               │   │   └── R
                               │   │       ├── T
                               │   │       └── aI
                               │   │           └── aL
                               │   └── O
                               │       ├── ae
                               │       │   └── ah
                               │       └── aH
                               ├── e
                               │   ├── h
                               │   │   └── S
                               │   │       └── aE
                               │   ├── j
                               │   │   └── am
                               │   │       └── aJ
                               │   ├── s
                               │   │   ├── C
                               │   │   └── Q
                               │   ├── D
                               │   └── aA
                               ├── q
                               │   ├── u
                               │   │   └── P
                               │   ├── G
                               │   ├── H
                               │   │   └── aO
                               │   └── L
                               ├── o
                               │   ├── ad
                               │   └── ap
                               ├── r
                               │   ├── K
                               │   └── aw
                               ├── v
                               │   ├── N
                               │   │   └── aF
                               │   └── au
                               ├── M
                               │   ├── as
                               │   ├── aB
                               │   └── aK
                               └── ax
    ```

3.  Now, switch the output by using the -t flag (e.g., run ./fork.py
-t). Given a set of process trees, can you tell which actions were
taken?

        Ans: Yes.

4. One interesting thing to note is what happens when a child exits;
what happens to its children in the process tree? To study this, let’s create a specific example: ./fork.py -A a+b,b+c,c+d,c+e,c-.
This example has process ’a’ create ’b’, which in turn creates ’c’,
which then creates ’d’ and ’e’. However, then, ’c’ exits. What do
you think the process tree should like after the exit? What if you
use the -R flag? Learn more about what happens to orphaned processes on your own to add more context.

        Ans: the orphaned children become children of the root process. If we include the R flag (reparent to local parent), the orphaned children go only "1 up" instead.

    ```sh
    python3 fork.py -A a+b,b+c,c+d,c+e,c- -c

                            Process Tree:
                                a

    Action: a forks b
                                a
                                └── b
    Action: b forks c
                                a
                                └── b
                                    └── c
    Action: c forks d
                                a
                                └── b
                                    └── c
                                        └── d
    Action: c forks e
                                a
                                └── b
                                    └── c
                                        ├── d
                                        └── e
    Action: c EXITS
                                a
                                ├── b
                                ├── d
                                └── e
    ```

    ```sh
    python3 fork.py -A a+z,z+b,b+c,c+d,c+e,c- -c

                            Process Tree:
                                a

    Action: a forks z
                                a
                                └── z
    Action: z forks b
                                a
                                └── z
                                    └── b
    Action: b forks c
                                a
                                └── z
                                    └── b
                                        └── c
    Action: c forks d
                                a
                                └── z
                                    └── b
                                        └── c
                                            └── d
    Action: c forks e
                                a
                                └── z
                                    └── b
                                        └── c
                                            ├── d
                                            └── e
    Action: c EXITS
                                a
                                ├── z
                                │   └── b
                                ├── d
                                └── e
    ```

    ```sh
    python3 fork.py -A a+b,b+c,c+d,c+e,c- -R -c

                            Process Tree:
                                a

    Action: a forks b
                                a
                                └── b
    Action: b forks c
                                a
                                └── b
                                    └── c
    Action: c forks d
                                a
                                └── b
                                    └── c
                                        └── d
    Action: c forks e
                                a
                                └── b
                                    └── c
                                        ├── d
                                        └── e
    Action: c EXITS
                                a
                                └── b
                                    ├── d
                                    └── e
    ```

    ```sh
    python3 fork.py -A a+z,z+b,b+c,c+d,c+e,c- -R -c

                            Process Tree:
                                a

    Action: a forks z
                                a
                                └── z
    Action: z forks b
                                a
                                └── z
                                    └── b
    Action: b forks c
                                a
                                └── z
                                    └── b
                                        └── c
    Action: c forks d
                                a
                                └── z
                                    └── b
                                        └── c
                                            └── d
    Action: c forks e
                                a
                                └── z
                                    └── b
                                        └── c
                                            ├── d
                                            └── e
    Action: c EXITS
                                a
                                └── z
                                    └── b
                                        ├── d
                                        └── e
    ```

5. One last flag to explore is the -F flag, which skips intermediate
steps and only asks to fill in the final process tree. Run ./fork.py
-F and see if you can write down the final tree by looking at the
series of actions generated. Use different random seeds to try this a
few times.

        Ans: Ok.

6. Finally, use both -t and -F together. This shows the final process
tree, but then asks you to fill in the actions that took place. By looking at the tree, can you determine the exact actions that took place?
In which cases can you tell? In which can’t you tell? Try some different random seeds to delve into this question.

        Ans: Ok.