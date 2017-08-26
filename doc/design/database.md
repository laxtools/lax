# database 

 - sql db 
   - nanodbc. a good c++ odbc wrapper. 

 - no! no sql
   
## db sharding 

 - 대규모 DB 처리 
  - sql db 기반 위에 구현 
  - db_proxy가 필요. 

요청 처리 
 a -- dp1 
   <-- ok
 a -- dp2
   <-- fail 
 a -- dp1
   --> rollback
   or 
   --> commit
   
a에서 분산 트랜잭션 처리 
위와 같은 구조로는 어렵다. 

세부 개념을 만들어서 처리해야 한다. 
lock 처리가 필요. 

unique index 기반의 분산 트랜잭션 
메모리에서 index 락 처리 

장애 처리도 필요. 

위의 use case가 분산 시스템의 깊은 문제를 함께 갖고 있다. 


