# construction - net 

 roughly in the order of implementation: 

 - unique int32 id over time 
   - big stride... 
   - ?

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

 - resize_buffer (수신처리, 임시 버퍼)
 - segment_buffer (송신용 분할 처리 버퍼)

 memcpy가 가장 느린 호출이 될 때까지 최적화를 했다. 
 check() 매크로 느리기 때문에 Shipping 빌드를 추가해야 한다. 

## Serialization 

msgpack으로 진행할 생각이었으나 생각보다 많이 느리다. 작은 경우 초당 백만개 이상은 가능하나 
수백만 개를 목표로 하고 있어 다른 라이브러리를 추가로 찾고 테스트 했다. 

- flatbuffers 
  - 사용이 좀 불편하다. 
  - 빌드 과정을 포함하면 msgpack보다는 빠르지만 아주 빠르지는 않다. 
  - 여러 언어를 지원하지 않을 lax의 경우는 불필요하다. 
- yas 
  - 깔끔하나 속도가 flatbuffers 정도이다. 
  - 약간 더 빠른 정도 
  - 사용은 괜찮으나 내부 shared_buffer를 교체할 인터페이스는 없다 
- bitsery 
  - 네트워크용 serialization 라이브러리로 개발 되었다 
  - 코드의 스트럭처를 그대로 메세지에 사용하는 개념. 기존의 게임 서버들의 개념이다. 
  - 속도도 msgpack보다 두 배 정도 빠르다. 
  - buffer를 직접 제공할 수 있어 추가 메모리 할당 / 해제가 없다. 

위와 같은 이유로 bitsery를 사용하기로 했다. 코드 품질은 괜찮은 것으로 보인다. 
파일 갯수도 많지 않아 이해나 디버깅이 어렵지는 않을 듯 하다. 
clang도 지원하니 안드로이드나 iOS 빌드도 큰 이슈는 없을 듯 하다. 

### bits_protocol 

bitsery를 사용하는 프로토콜. bitsery는 러시아어로 얇은 조각이란 뜻인데 
익숙하지 않으므로 bits로 부른다. 

bits_message, bits_factory, bits_protocol이 필요. 위를 지원하기 위한 
구조를 단위 테스트를 진행하면서 설계한다. 

 - bits_message
   - serialization을 일일이 작성해야 하는 게 불편하긴 하다. 
   - thrift 같은 경우 관리 구조가 맘에 들지 않기는 하다. 
     - C++에 맞지 않은 메세지 전송 구조와 enum 들이 만들어진다
	 - 서버 기능 개발에 사용한 구조체, 클래스를 메세지에 사용하기 어렵다
   - 최대한 버그 없게, 디버깅이 쉽게 메세지 정의하기 


## cipher 

botan library. configure.py로 빌드가 쉽게 됨. 

## service / protocol / session / acceptor / connector 

한번에 구현해야 한다. 상호 연관이 밀접하다. 
작업은 각 영역에 적는다. 

연결 관리가 꽤 많이 깔끔해졌다. 
close / destroy 개념 분리로 세션 정리도 괜찮고 
적절하게 에러 처리도 되고 있다.

### protocol



### session 

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


### service 

 acceptor / connector only. 

 protocol_msgpack::sub()를 통해서 수신

 protocol::ref를 통해 전송



## actor 

 메세지 송수신이 깔끔하도록 한다.

## interface 

 auto& svc = net::service::inst(); 

 session::sub( topic(bits, new_session), on_new_bits_session );

 svc.accept( ..., "bits");
 svc.connect( ..., "bits"); 

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



