# actor 

 - schedule
   - 시간 간격 기반
   - on_process protected 함수에서 처리
 - components
   - 안전 장치들 추가
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

 좀 더 안전한 방식으로 사용 가능하게 만듦


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

 작고 효율적인 구현이 가장 중요. 
 버그도 없고 디버깅도 쉬워야 한다. 

## ref 
  
 protocol
  
 - msg_actor_presence
   - startup / shutdown
 - msg_actor_udpate 

 json: 
  - data
  - protocol 확장
   

## sub / post 


