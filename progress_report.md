# Progress Report

Ryan Buchner

November 22, 2023

### Project Overview

My project is focused on creating a distributed authoritive DNS server. The main goal of the server must be capable of handling requests via different nodes and synchronizing the `host.txt` file. In addition, it must be able to handle downtime of different nodes, namely if parts of the DNS server, go offline, the system must be able to gracefully handle this failure, and the restart of those servers.

#### Final Report Structure

My final product will include the DNS system as well instructions to build and run it. In addition, I will outline the final design (I suspect aspects of the design described here will change slightly as I find bugs via testing), as well as the performance results of the system. Lastly, a thorough description of all of the tests run will be included.

### System Design

The general system design is outlined in the following sketch, and a in depth breakdown of the individual components as well as their API's are shown below. The components are designed such that they can sit on the same or different computers. The scoreboard and generator should be linked together. Certain functionality of the testing, however, requires that all of the modules run on the same computer and interact via locally (the backdoors the generator/scoreboard use to probe the other components) (this may change if I have time to make this functionality interact via sockets). I am still in the process of modifying everything to switch from communicating via software to communicating via sockets.

Please note for the following header files embedded I included extra functions that do not belong in then API to show the functionality and testing.

#### Generator

The generator module will be the main hub for testing the DNS system. It will be responsible for generating stimulus as well as setting up the system for a given trial. It is designed to run on the same system and the scoreboard and will interact with the scoreboard locally. In addition, many of it's backdoor funtions will only be available if the components are local to the system.

`generator.h`
```c

```


#### Router

The router is responsible for forwarding requests to an appropriate DNS server. This would normally be done by an actual router, but for the sake of the project we implemented this in software. By implementing in software, it allows us to take performance measurements easier as well.

`router.h`
```c
xxx
```

#### RRL

The RRL module will sit inside the Router, and when a request comes in it will be prompted by the router to decide if the request should be dropped or not.
`rrl.h`
```c
xxx
```

### DNS

This is the library I created to give me basic DNS functionality. It allows for easy creation and parsing of DNS messages. It doesn't deal with any of the socket programming though, see `dns_server.h` for that.

`dns.h`
```c

```

### DNS Server

Each DNS server is responsible for it's own `host.txt` file, a `changelog` and `DNSSEC` information (I have not yet implemented DNSSEC). In addition, it is accessible via the network to be requested to make changes to it's host file. The changes are logged and forwarded to the other servers via a 3-stage commit. In addition, other servers that recently came online may request for either the most recent changes or the entire `hosts.txt`, depending when they came online. All of the data is stored in the communication module, including the hash table used for DNS lookups.

`dns_server.h`
```c

```

### Current Status

As of today, I have completed the a base implementation of the individual components of the above system, including the generator and scoreboard modules, and I'm still in the process of unit testing those components. I have not implemented any full scale tests yet, but as I described in the design section, I have brainstormed the set of tests I would like to complete.