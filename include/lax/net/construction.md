# construction - net 

 roughly in the order of implementation: 

 - unique int32 id over time 
   - big stride... 
   - 

 - read buffer 
   - get a block of memory from a pool of diverse size 
   - 2^N sized blocks 
   - resize gets memory from the pool 
   - return buffer to OS with configured parameters


 - write buffer
   - current buffer implementation seems fine 
   - equall sized blocks 

 - protocol 
   - handle serialization and deserialization 
   - handle encryption and decryption 
   - handle key exchange and setup 

 - msgpack : protocol 

 - send 
   - handle 
   - remove direct access to session especially with service::get() for performance
   
 - session 
 - service 
 - acceptor 
 - connector integration 
 
 - logging 
   - enable / disable for network (global option)    

 - unit tests 

 - samples 
   - performance test 
   - encryption test 


## buffer 


## protocol 

### channel 

### ref 

## msgpack 


## session 

## service 

### slot 


### acceptor 

### connector 


## node 


### binder / holder 


## actor 






  

 