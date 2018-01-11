# construction - net 

 roughly in the order of implementation: 

 - unique int32 id over time 
   

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

   
 - session / service / acceptor / connector integration 


 - unit tests 

 - samples 
   - performance test 
   - encryption test 


## buffer 


## protocol 


## msgpack 


## integration 


  

 