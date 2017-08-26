# Object System 

 - object is an actor
 - object / state / component 
 - introspection 
   - code genereated like unreal 
 - thread-safe reference counted pointer 
 - inheritance 
 - container
   - room, instance, managers
   - ex. container<party> : container

## actor 관계 

 - object는 actor 
 - 메세지를 직접 받을 수 있다. 
 - actor::ref를 통해 로컬 / 리모트 통신 
 - actor로 update(time)으로 실행

## object / state / component 

 - comp_movement (cmo)
 - comp_ability (cab) 
 - comp_skill_control (csk)
 - comp_inventory (cin)
 - comp_aggro (cag)

 embedding into subclass to avoid checking pointer every time.

 all actions are written in components. 
 all message handlings are written in components.

 state activates components. 
 state changes message subscriptions.

## introspection 

 - object::id()
 - object::field("name")
   - any 
 - object::func("name").call( arguments )
 - script integration (code generation)
 - debugging 
 - visualization

## thread-safe reference counted 

 object::ref r1 = r2;
 object::ref r = object::parent()->find(id); // find a sibling

 r->send( message );




 
