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

 - session 
 - service 
 - acceptor 
 - connector integration 

 - send 
   - ref
   - remove direct access to session especially with service::get() for performance
   
 - logging 
   - enable / disable for network (global option)    

 - unit tests 

 - samples 
   - performance test 
   - encryption test 


## buffer 

 - send_buffer 
 - recv_buffer 

 memcpy가 가장 느린 호출이 될 때까지 최적화를 했다. 
 check() 매크로 느리기 때문에 Shipping 빌드를 추가해야 한다. 

 
## service / protocol / session / acceptor / connector 

한번에 구현해야 한다. 상호 연관이 밀접하다. 
작업은 각 영역에 적는다. 

## protocol

### channel 

### ref 

close에 대한 콜백 처리를 위해 뭔가 하기 보다는 
close 메세지에 대해 세션 아이디로 체크해서 전달하도록 한다. 
특별한 subscription을 관리하도록 한다. 
성능을 위해 필요하다. id로 찾아서 해당 id로 등록된 것들에 통보한다. 

이렇게 하는 게 좋아 보인다. 

## msgpack 


## session 

## service 

 acceptor / connector only. 

 protocol_msgpack::sub()를 통해서 수신

 protocol::ref를 통해 전송



### slot 


### acceptor 

### connector 


## node 


### binder / holder 


## actor 






  

 