# Shell 379

Shell program written in C for CMPUT 379 - Operating Systems concepts  
by Charles Ancheta - cancheta 1581672

## Submission

Source code and header files located in the `src` directory and the makefile are part of the assignment.  
Source code in the `util` directory and the `dev.mk` makefile were used for development and testing.

## Building

Run `make` to create the `shell379` executable with the minimal, submitted makefile.  
Run `make -f dev.mk` to use the development makefile. This makefile also builds the utility programs ending in `.util`.

## Utility Programs

**Testing piping**  
`logger.util` - receives input from stdin and logs output to stdout  
`piper.util` - pipes "HELLO WORLD!" to `logger.util`'s stdin and prints output to a file named `output`

**Testing user time**  
`runner.util`- runs bubble sort

**Testing sys time**  
`writer.util`- repeatedly writes to`/dev/null` (testing sys time)

**Others**
`sleeper.util`- sleeps for the given seconds
