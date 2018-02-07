# folders

 - util 
   - utility classes 
   - sequence, logger, exception 
 - channel
   - message channel 
 - task 
   - task and task scheduler 
 - net 
   - network service 
   - session
   - bits_protocol (c++ serialization protocol)
 - actor 
   - actor 
   - component 
- server 
  - service 
  - argos 
    - monitoring service 
    - command service
      - json
  - beast
    - web server
    - ssl (https)
    - pause, resume with argos commands
- db
  - odbc
  - query : message
  - db_service
    - task_scheduler inside
- meta
  - game data and code generation tool
  - game data management 

# construction / dependency order

 - util 
 - channel, task 
 - net 
 - actor
 - server  
    - db
 - meta 

