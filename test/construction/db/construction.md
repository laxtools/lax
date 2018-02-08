# db_service

- odbc 

  - nanodbc enhanced

- query 

  - is a message
  - request function
  - response callback handler 

- db_service

  - multiple sharded serivce
  - uuid and hash range based 
    - main object only, which is usually a player (user, character)

- item 

  - uuid key based 
    - for tracking 
    - for database merge

- db_client_service

  - is a service::remote_ref, a subclass of service::ref 
  - process sharding 
  - process callback 

- concrete example is required

  - complete with teamplte/sharding

