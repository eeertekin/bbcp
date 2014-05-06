bbcp
====

Securely and quickly copy data from source to target.

### Installation

Ubuntu 12.04 LTS

```sh
apt-get update
apt-get install libssl-dev build-essential zlib1g-dev git
git clone https://github.com/eeertekin/bbcp.git
cd bbcp/src
make
cp ../bin/amd64_linux/bbcp /bin/bbcp
bbcp --version
```

### Examples
```sh
    > bbcp -P 2 -w 2M -s 10 my.big.file eee@moon:/top-secret-files/
    bbcp: Creating /top-secret-files/my.big.file
    bbcp: At 140506 17:33:18 copy 20% complete; 89998.2 KB/s
    bbcp: At 140506 17:33:28 copy 41% complete; 89910.4 KB/s
    bbcp: At 140506 17:33:38 copy 61% complete; 89802.5 KB/s
    bbcp: At 140506 17:33:48 copy 80% complete; 88499.3 KB/s
    bbcp: At 140506 17:33:58 copy 96% complete; 84571.9 KB/s
```

### Usage

    bbcp [Options] [Inspec] Outspec

    Options: [-a [dir]] [-b [+]bf] [-B bsz] [-c [lvl]] [-C cfn] [-D] [-d path]
         [-e] [-E csa] [-f] [-F] [-h] [-i idfn] [-I slfn] [-k] [-K]
         [-L opts[@logurl]] [-l logf] [-m mode] [-n] [-N nio] [-o] [-O] [-p]
         [-P sec] [-r] [-R [args]] [-q qos] [-s snum] [-S srcxeq] [-T trgxeq]
         [-t sec] [-v] [-V] [-u loc] [-U wsz] [-w [=]wsz] [-x rate] [-y] [-z]
         [-Z pnf[:pnl]] [-4 [loc]] [-$] [-#] [--]

    I/Ospec: [user@][host:]file

    Function: Secure and fast copy utility.
    
    -a dir  append mode to restart a previously failed copy.
    -b bf   sets the read blocking factor (default is 1).
    -b +bf  adds additional output buffers to mitigate ordering stalls.
    -B bsz  sets the read/write I/O buffer size (default is wsz).
    -c lvl  compress data before sending across the network (default lvl is 1).
    -C cfn  process the named configuration file at time of encounter.
    -d path requests relative source path addressing and target path creation.
    -D      turns on debugging.
    -e      error check data for transmission errors using md5 checksum.
    -E csa  specify checksum alorithm and optionally report or verify checksum.
            csa: [%]{a32|c32|md5}[=[<value> | <outfile>]]
    -f      forces the copy by first unlinking the target file before copying.
    -F      does not check to see if there is enough space on the target node.
    -h      print help information.
    -i idfn is the name of the ssh identify file for source and target.
    -I slfn is the name of the file that holds the list of files to be copied.
            With -I no source files need be specified on the command line.
    -k      keep the destination file even when the copy fails.
    -K      do not rm the file when -f specified, only truncate it.
    -l logf logs standard error to the specified file.
    -L args sets the logginng level and log message destination.
    -m mode target file mode as [dmode/][fmode] but one mode must be present.
            Default dmode is 0755 and fmode is 0644 or it comes via -p option.
    -n      do not use DNS to resolve IP addresses to host names.
    -N nio  enable named pipe processing; nio specifies input and output state:
            i -> input pipe or program, o -> output pipe or program
    -s snum number of network streams to use (default is 4).
    -o      enforces output ordering (writes in ascending offset order).
    -O      omits files that already exist at the target node (useful with -r).
    -p      preserve source mode, ownership, and dates.
    -P sec  produce a progress message every sec seconds (15 sec minimum).
    -q lvl  specifies the quality of service for routers that support QOS.
    -r      copy subdirectories and their contents (actual files only).
    -R args enables real-time copy where args specific handling options.
    -S cmd  command to start bbcp on the source node.
    -T cmd  command to start bbcp on the target node.
    -t sec  sets the time limit for the copy to complete.
    -v      verbose mode (provides per file transfer rate).
    -V      very verbose mode (excruciating detail).
    -u loc  use unbuffered I/O at source or target, if possible.
            loc: s | t | st
    -U wsz  unnegotiated window size (sets maximum and default for all parties).
    -w wsz  desired window size for transmission (the default is 128K).
            Prefixing wsz with '=' disables autotuning in favor of a fixed wsz.
    -x rate is the maximum transfer rate allowed in bytes, K, M, or G.
    -y what perform fsync before closing the output file when what is 'd'.
            When what is 'dd' then the file and directory are fsynced.
    -z      use reverse connection protocol (i.e., target to source).
    -Z      use port range pn1:pn2 for accepting data transfer connections.
    -4      use only IPV4 stack; optionally, at specified location.
    -$      print the license and exit.
    -#      print the version and exit.
    --      allows an option with a defaulted optional arg to appear last.
    
    user    the user under which the copy is to be performed. The default is
            to use the current login name.
    host    the location of the file. The default is to use the current host.
    Inspec  the name of the source file(s) (also see -I).
    Outspec the name of the target file or directory (required if >1 input file.
    
    ******* Complete details at: http://www.slac.stanford.edu/~abh/bbcp




######Forked from http://www.slac.stanford.edu/~abh/bbcp/bbcp.git/
