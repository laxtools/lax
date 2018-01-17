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

 memcpy�� ���� ���� ȣ���� �� ������ ����ȭ�� �ߴ�. 
 check() ��ũ�� ������ ������ Shipping ���带 �߰��ؾ� �Ѵ�. 

 
## service / protocol / session / acceptor / connector 

�ѹ��� �����ؾ� �Ѵ�. ��ȣ ������ �����ϴ�. 
�۾��� �� ������ ���´�. 

## protocol


## msgpack 


## session 

ä�ΰ� ref ����� session�� service�� �̵�. 
�ͼ��� session �������� �����ϰ� 
���������� factory�� ó����. 
�������� ��Ī�� �ָ� �ش� ���������� ã�Ƽ� �����ϵ��� ��


### channel 

### ref 

close�� ���� �ݹ� ó���� ���� ���� �ϱ� ���ٴ� 
close �޼����� ���� ���� ���̵�� üũ�ؼ� �����ϵ��� �Ѵ�. 
Ư���� subscription�� �����ϵ��� �Ѵ�. 
������ ���� �ʿ��ϴ�. id�� ã�Ƽ� �ش� id�� ��ϵ� �͵鿡 �뺸�Ѵ�. 

�̷��� �ϴ� �� ���� ���δ�. 


## service 

 acceptor / connector only. 

 protocol_msgpack::sub()�� ���ؼ� ����

 protocol::ref�� ���� ����


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
	// ���μ��� 	
	auto sref = net::service::inst().acquire(m->id())

	sref.send( nm );

	sref.close();
 } 

 - ����� 
 - ���� ����  
 - ���� ����  

 ��ü������ ����ϴ�. 

 heartbeat ���� ��? 

 - �شܿ� �� �ʿ� ���� send() �� �� (����� ��) Ȯ���ϰ� ���� ��Ŵ
 - ������ �ϵ��� ��  
 