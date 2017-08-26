# actor 

 - mailbox 
   - sub / post
 - childs 
   - owns
 - schedule
   - timer + os scheduling
   - action (function) 
 - continuation
 - components
 - light like a feather
 - ref / ptr 
   - ref used in dir / index 
     - to communicate / to check 
   - ptr used to call functions

# node 

 - is an actor
 - clustering 
   - has net::service 
   - connect / reconnect 
 - dir(ectory)
   - local and remote  
   - index with type
   - index with ...
 - runs under main thread 
 
# runner : actor 

 - has a thread 
 - thread-safe boundary 

# msg_local 

 - local protocol to trace
 - use messages between threads   

# client : actor 

 - special actor id  
 - ref has session info

# server model 

 - drives use case

## MMORPG 

 - shard server
   - backend only (focus on service)
   - provides service to others 
     - instance actors 
	 - container actors 
	 - sub / post 
	 - replicate user / client info
	   
 - game server 
   - runners
   - instances
   - regions

## Instance games 

 - massive matching 
 - massive instances


