![Image of Lax](lax_logo.png)


# A little axe for game server programmers

 Game development must be a fun.  Lax tries to provide a little axe to game server programmers 
 with following features: 
 
 - simple, fast, and clean networking code
 - task scheduler for efficient processing 
 - database usage model
   - integration of open source tools and experience. 
 - game data model
   - csv based
   - RDBMS stored and fetched at runtime
   - list / enum  
   - type safe
 - remote debugging and monitoring 

 
## Networking 

 - tcp only w/ asio 

 - message protocols:
   - msgpack 
   - json 

 - simple abstractions: 
   - bundle for protocol stack of fibers

## Messaging 

 - channel based messaging 
   - subscription management
   - immediate or delayed posting

## Task 

 - task scheduler 
   - use a pool of threads
   - thread affinity 
   - load based scheduling

## Database 

 - nanodbc 
 - database task and task scheduler 
   - integrated with blocking task feature 

## Game Data Model 

 - csv 
 - type verified per column 
 - user provided parser / verifier for each column
 - fast loading 


