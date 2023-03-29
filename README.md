
# End to End Detection of Network Compression

On occasion, routers or nodes on the Internet may use link-layer or IP-level compression on traffic flows, without any awareness of the end-hosts. However, if the end-host applications are informed of the compression already provided by an intermediary, they can avoid applying their own compression, saving both time and resources. These savings are particularly advantageous in mobile applications.

In this project two network applications are implemented to detect if network compression is present on a network path and if found locate the compression link. The first application implemented is a client/server application. The second application is a standalone application which works in an uncooperative environment. The standalone application detects network compression without requiring special software to be installed on the other host.


## Configuration File

The program reads configuration information from a file specified as a command line argument. The cJSON library is used to parse and store the information in a struct, which is utilized by the client, server, and standalone applications. The example configuration file includes details like server IP address, UDP and TCP ports, packet payload size, inter-measurement time, number of UDP packets, and packet delay. For the standalone application Destination port for TCP Head SYN and Destination port for TCP Tail SYN

```bash
{   
    "clientIP": "10.0.0.245",
    "serverIP": "10.0.0.27",
    "TCPPort": "2056",
    "sourcePortUDP":"9876",
    "destPortUDP":"8765",
    "destPortTCPHead":"2000",
    "destPortTCPTail":"2049",
    "portTCP": "8787",
    "payload": "1000",
    "interMeasureTime": "15",
    "noOfPackets":"6000",
    "TTL":"255"
}

```

## Requirements

Need to have cJSON.c and cJSON.h files which are included in the zip and which are also included as a header in the code.

# Application

## Pre Probing Phase

The process begins with the client application establishing a TCP connection with the server. If the connection is established successfully, the client transmits the complete contents of the configuration file to the server. The server will utilize this data in future stages to determine if network compression is available or not. The TCP connection is closed after the entire configuration file has been successfully transmitted.

## Probing Phase

During this stage, the sender transmits a series of UDP packets. Initially, it sends n UDP packets containing low entropy data. After sending these packets, the client needs to wait for a specific duration of time known as Inter-Measurement Time (γ) before initiating the next phase. This next phase comprises a packet train containing n UDP packets with high entropy data

## Post Probing Phase

Once all packets reach the server, the server performs the necessary calculations required to detect the presence of network compression on the path between the server and the client. Once the probing phase is complete, the client establishes a new TCP connection. When the connection is established successfully, the server transmits its results to the client.

## Standalone Application

A standalone application is capable of detecting network compression independently without any assistance from the server. The program uses a raw socket implementation and sends a single head SYN packet. After sending the SYN packet, a train of n UDP packets is transmitted, followed by a single tail SYN packet. These SYN packets are sent to two different ports (x and y) that are not commonly used, so they are assumed to be inactive or closed ports with no applications listening to them. When the SYN packets are sent to closed ports, the server is expected to generate RST packets. The application records the arrival time of two RST packets. The difference between the two RST packets' arrival time is then utilized to determine whether network compression is detected or not. In the event that one of the RST packets is lost, or the server fails to respond with an RST packet, the application outputs "Failed to detect due to insufficient information" and terminate.

## Running the Program
To compile the client/server application use the following commands

On Client System

```bash
gcc -g -std=c99 -o client client.c

```

On Server System

```bash
gcc -g -std=c99 -o server server.c

```

To run the client/server application use the following commands

On Client System

```bash
./client myconfig.json

```

On Server System

```bash
./server 8787

```

8787 is the server TCP port number

To compile the standalone application use the following commands

On Client System

```bash
gcc standalone_app.c -o standalone_app

```

To run the standalone application use the following commands

```bash
sudo ./standalone_app myconfig.json

```
For the standalone application to run in your computer you need to give the MAC address of the server VM in the standalone_app.c file. 

## Troubleshooting

If client fails to connect to the server in the pre probing phase, reopening the code editor and rerunning the program will help. I am adding a sleep before calling the post probing phase in the client.c file. If the server fails to connect to the client in the post probing phase, close the code editor at the server side and then open it. Now compile and run the applications. Even if this does not work change the sleep at the client side before the post probing phase and then retry after reopening the code editor of the server application. Also restarting the Virtual Machine might help as well.

In the standalone.c file I am getting the squiggly lines but the code gets compiled by following the instructions in the "Running the Program" section. 

Some times I get compression detected and some times I get compression not detected for both the client/server and the standalone application.

## References

https://www.pdbuchan.com/rawsock/rawsock.html

https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/

https://www.geeksforgeeks.org/udp-client-server-using-connect-c-implementation/

https://www.geeksforgeeks.org/multithreading-in-c/

http://www.cs.kent.edu/~ruttan/sysprog/lectures/signals.html

## Developer 

Soham Kalse