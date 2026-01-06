# dinning_phylosophers
In colaboration with Marlea Martens, we are going to do a variation of the dining philosophers. The idea is we have a table, and we have a set number of philosophers (dpeending on the level). The amount of forks are equivalent to the number of philosophers. The objective is to keep all of the philosophers happy, by feeding them, since they think so hard. Each philosopher has a timer, and when the timer hits 0 for any philosopher, the game ends and the user loses. If the user manages to keep them all alive for the total time (in the center of the table), the user wins. When eating, the forks also operate on a timer, of 2 seconds. No two philosophers can use the same fork at the same time, so it's overall just a game about resource management.

## How It's Made:
**Tech used:** C, Raylib
Using Raylib as the framework for our graphics, we model OS resource allocation in concurrent systems.

## Get raylib going
First we need to grab raylib and build it.

Let's put raylib outside this github repo (we don't want to check in the entire thing).
```
$ cd SOMEWHERE_ELSE
$ git clone https://github.com/raysan5/raylib.git raylib
$ cd raylib
$ mkdir build && cd build
$ cmake -DBUILD_SHARED_LIBS=ON ..
$ make
```
Then we can try some examples:
```
$ examples/core_input_keys
$ examples/core_2d_camera
$ examples/core_2d_camera_platformer
```
Then we can modify it a little, and verify it did indeed change:

```
$ code ../examples/core/core_input_keys.c # change the program
$ make
$ examples/core_input_keys
```
