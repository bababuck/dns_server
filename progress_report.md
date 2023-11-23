# Progress Report

Ryan Buchner

November 22, 2023

### Project Overview

My project is focused on creating a distributed authoritive DNS server. The main goal of the server must be capable of handling requests via different nodes and synchronizing the `host.txt` file. In addition, it must be able to handle downtime of different nodes, namely if parts of the DNS server, go offline, the system must be able to gracefully handle this failure, and the restart of those servers.

#### Final Report Structure

My final product will include the DNS system as well instructions to build and run it. In addition, I will outline the final design (I suspect aspects of the design described here will change slightly as I find bugs via testing), as well as the performance results of the system. Lastly, a thorough description of all of the tests run will be included.

### System Design

The general system design is outlined in the following sketch, and a in depth breakdown of the individual components as well as their API's are shown below:




#### Generator

#### Router

#### RRL

The RRL module will sit inside the Router, and when a request comes in it 
`rrl.h`
```c
/**
 * Response rate limititing (RRL) module tracks the frequency of requests from different IPs.
 *
 * RRL is used to protect users from DNS amplification attacks, as a given IP
 * address will only be able to recieve so many repsonses for our server.
 *
 * This RRL is implemented quite simply, using a window of 15 seconds and reseting
 * the counter every time the window closes. This is handled internally via callbacks.
 *
 * The counter is impelemented via a hash table, without any locking for multithreading.
 *
 * This module can either sit inside the router or inside individual DNS modules.
 */

#include <boolean.h>

/**
 * Check whether a given IP has been used too frequently.
 *
 * Increments the internal counter for the requested address.
 *
 * @param ip_address: The ip_address to be checked.
 *
 * @returns True if the requested IP address has passed its threshold
 */
bool check_lookup(char* ip_address);
```

####

### Current Status

As of today, I have completed the a base implementation of the individual components of the above system, including the generator and scoreboard modules, and I'm still in the process of unit testing those components. I have not implemented any full scale tests yet, but as I described in the design section, I have brainstormed the set of tests I would like to complete.