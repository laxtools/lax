# subscription network  

## use case 

 actor 
  - hub.sub(message_type, id()) 
  - hub.sub(session_id, id())
  - hub.sub(session_id, message_id, id())

  - hub�� ���� Ŭ���� 
  - actor / node / session ���� ��� ���� 

  - hub.pub(message)

  - hub.bind()
	- ���� ��Ʈ��ũ ���� 
	- pub�� ������ ���� ��Ʈ��ũ�� ����

 post 
  - �޾��� �� tag ���� 
    - tag full_tag( message.src, message.dst, message.type, message.session_id)
    - tag addr_src_tag( message.src );
    - tag addr_dst_tag( message.dst );
    - tag msg_type_tag( message.type );
    - tag recv_session_tag( message.session_id );
	- �� �±׷� ã�Ƽ� ���� 

  - ���� �� tag ���� 
    - 

 
 ���� �� �����δ� �����ϴ�. �� ��Ȳ������ ��� ���̽��� �����ϴ� ���� ���ϰ� ȿ������ ����� ã�´�. 

 �̰��� �ٽ��̴�. 


## ����

  ���� �ּ�, �޼��� Ÿ��, �޼��� ����, ���� �ּ� 

  �޼��� ���뿡 ���� �ּ� (�����, ������), ���� �ּ�, �޼��� Ÿ��, �޼��� ������ ������ �� �ִ�. 

  0�� ��� ��Ī �ϴ� ������ �±׸� �����ؼ� ��Ī�Ǹ� �����ϰ� �� �� �ִ�. 

  hub.sub( tag, id())

  hub.bind( tag, session_id)

  tag�� bit vector�� �Ѵ�. 
  - bit vector���� hash�� �����ؼ� map���� ã�Ƽ� ���� 
  - �ʵ� ������ ������ �� �ۿ� ����
  - �ؽ� ������ �ϰ� exact ��Ī�� == �� �ؾ� ��

 tag.src().dst().sid(); 
 
 tag.msg<type>().fld("charactor_id", 3)  

 msg.f(0), msg.f(1) --> �� �������� 

 tag.dst(id()) --> �������� ������ ��  
 tag.mid(3) --> 3�� �޼��� �� ��Ī 
 tag.src(id(99, 33)) --> �ҽ��� 99/33 ���� ��Ī
 tag.msg<msg_req_login>().fld("charactor_id", 3) // msg_req_login�� character_id �ʵ尡 3�� ��� ��Ī

 �ٸ� �͵��� ���̶� ���� ���� msg�� ���� �߿� 


