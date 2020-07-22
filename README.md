## Building

This might take a while

```shell
./waf configure --enable-examples
```

followed by

```shell
./waf
```

in the directory which contains this README file. The files
built will be copied in the build/ directory.

## Running 

On recent Linux systems, once you have built ns-3 (with examples
enabled), it should be easy to run the sample program with the
following command, such as:

```shell
./waf --run="scratch/nw <number-of-nodes>"
```

The program will generate a .pcap file for every node in the network.

Use WireShark to analyze the .pcap file.