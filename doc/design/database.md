# database 

 - sql db 
   - nanodbc. a good c++ odbc wrapper. 

 - no! no sql
   
## db sharding 

 - ��Ը� DB ó�� 
  - sql db ��� ���� ���� 
  - db_proxy�� �ʿ�. 

��û ó�� 
 a -- dp1 
   <-- ok
 a -- dp2
   <-- fail 
 a -- dp1
   --> rollback
   or 
   --> commit
   
a���� �л� Ʈ����� ó�� 
���� ���� �����δ� ��ƴ�. 

���� ������ ���� ó���ؾ� �Ѵ�. 
lock ó���� �ʿ�. 

unique index ����� �л� Ʈ����� 
�޸𸮿��� index �� ó�� 

��� ó���� �ʿ�. 

���� use case�� �л� �ý����� ���� ������ �Բ� ���� �ִ�. 


