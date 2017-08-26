# Communication 

## asio 

 - Wrapper for socket layer 
 - tcp only

## interface 

### ideas from other key players 

 - node.js 
 - zeromq 
 - actor based systems 
   - akka 
   - CAF (C++ actor framework)
 - restful 

#### node.js 

 - listen / connect / send / recv
 - asynchronous 

this.on('data', function(data){
    console.log("Read from server: " + data.toString());
    ...
    this.end();
});

node.js delivers raw bytes to application.

node.js runs in a connection context. 

#### zeromq 

0mq has a higher level connection concept. 

PUB, SUB on communication layer.

It is a messaging framework with:
 - publishing and subscription on contexts. 

#### CAF 

CAF claims:
 - lightweight 
 - locational transparency 
 - simple 
 - adaptive
 - lockfree

Its encapsulation uses template magic. 

CAF is very powerful runtime and api for distributed computing including GPGPU actors. 

This power makes it a little heavy to use for games 
especially client implementation in multiple language and plaforms. 

But the concept and implementation quality are great. 

#### REST api 

Node.js has rest api with express. 

app.get('/action', function(req, res) {
        res.send('Hello /');    
});

Behavior is set per URI or path. 

Request and response are from HTTP protocol. 
It can be extended to meet most communication patterns. 

Core concepts are: 
 - Request point (URL) 
 - Request and response object 

To begin communication, server needs to listen on a port. 

Operations are or can be asynchronous. 


##### Concepts

Representational State Transfer (ReST)

Scrict ReST requires stateless server. 

This is not possible for game servers. 
Games have lots of states on client and server. 
Update of game server states is essential.

Resource identifier can be used to make server flexible. 
Request and response representation with minimal state can be adopted. 

2002-REST-TOIT.pdf explains the original web architecture. 



### Direction 

Rules to follow are: 
 - Simple and light  
 - Easy to use

Goal: Find ReST for game server anc client networking.


 
