# DNS Server

This project is for UC Davis Grad course ECS235A.

The end product of will be a caching DNS server that will split data across several copies to protect again DNS attacks.

## Tasks

- [ ] Implement basic DNS query/response format
- [ ] Implement UDP formatter
- [ ] Implement port monitor
- [ ] Implement inter-server protocol

## System Overview

Set of DNS servers able to handle requests about an internally hosted service. This will simulate a DNS for a webservice, where each DNS in the system would be listed as multiple A-records in the authoritive server containing information about my webservice, or as a single A-record that specifies a front server which forwards requests to one of several servers to load balance. To simulate this, I will spread my DNS server out across several WiFi enabled devices (microprocessors or maybe laptops). Since I don't have funding to pay for registering a domain name, I will instead use a single device to generate request that will be sent to all of the different DNS servers. I will be able to simulate DNS server failures by simply turning off one or more of the servers randomly, or by overloading one of the devices with excess traffic.

The system will have have a single primary DNS server and several secondary DNS servers. When updating the host file, the primary server will be responsible for propogating the changes to the secondary servers. A secure protocol will be used for this propogation. Sever operations will be supported, namely, adding and removing an entry. Editing an entry can be achieved via addition followed by removal. In addition, support will be required for adding additional DNS servers without restarting the entire system. The system must be robust to servers going offline and coming back online. The communcation between the different servers must be secure and robust to man-in-the-middle and replay attacks.
