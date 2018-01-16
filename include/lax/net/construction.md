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

### channel 

### ref 

close�� ���� �ݹ� ó���� ���� ���� �ϱ� ���ٴ� 
close �޼����� ���� ���� ���̵�� üũ�ؼ� �����ϵ��� �Ѵ�. 
Ư���� subscription�� �����ϵ��� �Ѵ�. 
������ ���� �ʿ��ϴ�. id�� ã�Ƽ� �ش� id�� ��ϵ� �͵鿡 �뺸�Ѵ�. 

�̷��� �ϴ� �� ���� ���δ�. 

## msgpack 


## session 

## service 

 acceptor / connector only. 

 protocol_msgpack::sub()�� ���ؼ� ����

 protocol::ref�� ���� ����



### slot 


### acceptor 

### connector 


## node 


### binder / holder 


## actor 






  

 