# actor 

 - schedule
   - �ð� ���� ���
   - on_process protected �Լ����� ó��
 - components
   - ���� ��ġ�� �߰�
 - id
 - ref / ptr 
   - ref used in dir / index 
     - to communicate / to check 
   - ptr used to call functions
 - sub / post
 - childs 
   - owns
 - continuation
   - waiting response w/ timeout
 - light like a feather

## components 

 �� �� ������ ������� ��� �����ϰ� ����


## subclassing 

 game::character : actor 
 game::npc : game::character
 game::pc  : game::character

 game::room : actor 
 game::room_manager : actor

## scheduling 

 process() inline function and interval 

 runner - self thread 

## sub / post 

 �۰� ȿ������ ������ ���� �߿�. 
 ���׵� ���� ����뵵 ������ �Ѵ�. 

## ref 
  
 protocol
  
 - msg_actor_presence
   - startup / shutdown
 - msg_actor_udpate 

 json: 
  - data
  - protocol Ȯ��
   

## sub / post 


