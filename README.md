# MemoryTracingTools

This folder contains 8 memory tracing tools for [Intel&reg; PIN], they implement all combinations of:
 - capture read/write memory access,
 - get address/1-byte-content of memory access, and
 - output in binary/text format.


### Requirements

Since these tools are PIN tools, you will need to [download and install PIN].


### Compile

Copy this directory into `PIN-install-dir/source/tools/MemoryTracingTools`, enter it, and run
```sh
$ make
```


### Use

Run desired tool with desired command, e.g.,
```sh
$ pin -t PIN-install-dir/source/tools/MemoryTracingTools/obj-intel64/read_addr_txt.so -- your_command with possible arguments
```
which acquires memory addresses of all read instructions and saves the trace in text format into the current directory. Filenames of traces follow this format: `rw_ac(.txt)`, where
 - `rw` is either `read` or `write`,
 - `ac` is either `addr` or `cnt`, and
 - `.txt` is used for text format only.


   [Intel&reg; PIN]: <https://software.intel.com/en-us/articles/pin-a-dynamic-binary-instrumentation-tool>
   [download and install PIN]: <https://software.intel.com/en-us/articles/pintool-downloads>
