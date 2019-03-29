# Graph's Database

## Required Software 
 * Any code editor or an IDE 

## Required Dependencies
 * **G++ Compiler [c++11>]**

## Socket C
 [Socket](http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html) 

## Compile
```sh
$ cd folder
$ g++ balancer.cpp -o balancer.out
$ g++ client.cpp -o client.out
```

## Running
 * To change the port, it must be modified in both files [balancer-client].
 * To change the ip to whom I am connecting, do it on the client!.
 * for the moment, it only connects and returns to which query server it should go !, for example:
	```sh
	$ |--> q [1] // the query goes to the server 2
	```