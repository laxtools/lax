# subscription network  

## use case 

 actor 
  - hub.sub(message_type, id()) 
  - hub.sub(session_id, id())
  - hub.sub(session_id, message_id, id())

  - hub는 작은 클래스 
  - actor / node / session 등이 상속 받음 

  - hub.pub(message)

  - hub.bind()
	- 전송 네트워크 구성 
	- pub로 넣으면 전송 네트워크로 전달

 post 
  - 받았을 때 tag 생성 
    - tag full_tag( message.src, message.dst, message.type, message.session_id)
    - tag addr_src_tag( message.src );
    - tag addr_dst_tag( message.dst );
    - tag msg_type_tag( message.type );
    - tag recv_session_tag( message.session_id );
	- 각 태그로 찾아서 전달 

  - 보낼 때 tag 생성 
    - 

 
 위의 것 만으로는 부족하다. 좀 장황해져도 모든 케이스를 포괄하는 쉽고 편하고 효율적인 방법을 찾는다. 

 이것이 핵심이다. 


## 개념

  액터 주소, 메세지 타잎, 메세지 내용, 세션 주소 

  메세지 내용에 액터 주소 (출발지, 도착지), 세션 주소, 메세지 타잎, 메세지 내용을 포함할 수 있다. 

  0면 모두 매칭 하는 것으로 태그를 생성해서 매칭되면 전달하게 할 수 있다. 

  hub.sub( tag, id())

  hub.bind( tag, session_id)

  tag는 bit vector로 한다. 
  - bit vector에서 hash를 생성해서 map에서 찾아서 전달 
  - 필드 개수는 제한할 수 밖에 없음
  - 해시 값으로 하고 exact 매칭은 == 로 해야 함

 tag.src().dst().sid(); 
 
 tag.msg<type>().fld("charactor_id", 3)  

 msg.f(0), msg.f(1) --> 값 가져오기 

 tag.dst(id()) --> 목적지만 같으면 됨  
 tag.mid(3) --> 3번 메세지 다 매칭 
 tag.src(id(99, 33)) --> 소스가 99/33 액터 매칭
 tag.msg<msg_req_login>().fld("charactor_id", 3) // msg_req_login의 character_id 필드가 3일 경우 매칭

 다른 것들은 값이라 관계 없고 msg가 가장 중요 


