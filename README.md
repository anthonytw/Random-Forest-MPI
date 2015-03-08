# Random-Forest-MPI
Random Forest MPI implementation

NOTE: This code is not maintained. I recently dug it up to get it compiling again (now
using cmake) and it seems to work (other than a serialization unit test failing-boo)
but use at your own demise! It may still be useful if you're learning about RF or MPI basics.
The rest of this is more or less copied from my response to an issue posed by another user.

Built on Ubuntu 14.10 with ```cmake libopenmpi libopenmpi-dev libcppunit libcppunit-dev```.

To build, do something like this: (from project root)

```
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake ..
make
```

The binaries are now in build/apps/rf and build/unit_tests/rf-ut. Note the toolchain
specifies the compilers to use. You need to use the MPI compilers mpicc and mpic++ to
build the code.

To run the random forest, you can try running rf directly and it will give you this output:

```
Usage: mpirun -n <1> rf <2> <3> <4> <5>
  <1> - Number of nodes to use
  <2> - Bootstrap divisor
  <3> - Split keys per node
  <4> - Trees per forest
  <5> - Training set sample probability (%)
```

As you can see, you need to call it with the mpirun command. Notice this is where you set
the number of nodes in the cluster, not in the code. The other parameters mentioned are
the RF parameters.

My code here sucks (blame it on laziness, blame it on too many study beers) so actually the
thing will crash if you don't run it from the project root since the locations of the data it's
loading in are hard-coded.

So, from the root, try this: (See above for which parameters these are)

```
mpirun -n 4 build/app/rf 2 20 1000 0.9
```

I get this:

```
<<bunch of debugging junk from the child processes>>
-----------------------------------------------------------------------
Testing Set Classification:
-----------------------------------------------------------------------
tp,fp,tn,fn       : 85, 1, 393, 28
Accuracy          : 94.2801%
Precision         : 98.8372%
True negative rate: 99.7462%
Recall            : 75.2212%
```

This is obviously only looking at one point on the ROC (I suppose the one giving the best
accuracy? I don't remember.) But you can go from there.

Aside from that, I can't provide too much more support on this, as I don't really maintain
the code. But hopefully this helps give you an idea of how to use it and make your own
MPI applications. :-)
