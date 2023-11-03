/**
 * Idea: Each zone keeps track of the last update ID it recieved. If it recieve an update with a number that is greater than it's number, then must have been offline or missed, and request full update. If less, ignore (attack?)

 On restart, request entire hosts.txt

 If primary is down, designate new primary, and send the new primary information. What if send something primary, and it goes offline before it propogates. Have to log changes in file. To make edit, we login to machine, log edit we want to make, then send the message to primary, if primary is down, choose new primary, find out it's current state, and then send all edits it missed plus new edit. It will propogate those changes, even if not needed to other servers.

A server doesn't need to know if primary or secondary, just needs to know if should propogate or not.

Adding a new server requires letting other servers know about the new server and having it get zone file
- when server comes back from offline, must get list of all other servers (in case additions/deletions) -> include there in zone files
 */
