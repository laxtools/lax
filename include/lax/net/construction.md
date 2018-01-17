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


## msgpack 


## session 

채널과 ref 기능을 session과 service로 이동. 
익숙한 session 개념으로 진행하고 
프로토콜은 factory로 처리함. 
프로토콜 명칭을 주면 해당 프로토콜을 찾아서 설정하도록 함


### channel 

### ref 

close에 대한 콜백 처리를 위해 뭔가 하기 보다는 
close 메세지에 대해 세션 아이디로 체크해서 전달하도록 한다. 
특별한 subscription을 관리하도록 한다. 
성능을 위해 필요하다. id로 찾아서 해당 id로 등록된 것들에 통보한다. 

이렇게 하는 게 좋아 보인다. 


## service 

 acceptor / connector only. 

 protocol_msgpack::sub()를 통해서 수신

 protocol::ref를 통해 전송


### slot 

### acceptor 

### connector 


## server


### binder / holder 


## actor 


## interface 

 auto& svc = net::service::inst(); 

 MSGPACK_REGISTER(msg_new_msgapck_session); 
 MSGPACK_REGISTER(mreq_login);
 MSGPACK_REGISTER(mres_login);
 MSGPACK_REGISTER(msyn_login);

 session::sub( topic(msgpack, new_session), on_new_msgpack_session );

 svc.accept( ..., "msgpack");
 svc.connect( ..., "msgpack"); 

 on_new_session(m) 
 {
	// 프로세스 	
	auto sref = net::service::inst().acquire(m->id())

	sref.send( nm );

	sref.close();
 } 

 - 디버깅 
 - 상태 보기  
 - 구성 가능  

 전체적으로 깔끔하다. 

 heartbeat 같은 건? 

 - 밑단에 둘 필요 없이 send() 할 때 (사용할 때) 확인하고 종료 시킴
 - 구성만 하도록 함  
 