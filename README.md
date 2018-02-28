![Image of Lax](lax_logo.png)


# A little axe for game server programmers

Game development must be a fun.  Lax tries to provide a little axe to game server programmers 
 with following features: 

 - simple, fast, and clean networking code

 - task scheduler for efficient processing 

 - channel based message subscription and posting

 - database model
   - integration of open source tools and experience. 

 - game data meta system 
   - csv based
   - RDBMS stored and fetched at runtime
   - list / enum  
   - type safe

 - remote debugging and monitoring 

 - actors with components for game objects


** lax is under construction ** with following todos.

#  TODOs

- [ ] server / service 
  - channel timer subscribe and callback
  - service clustering
    - startup / shutdown / update / TTL and alive message
    - service directory (local)
  - service lookup
    - multiple service instances on different servers 
  - local and remote references
    - send to references
    - sub to service channel
  - broadcast / multicast handling 
    - efficiency / easiness

- [ ] db_service

  - configure with json
  - query runners
  - query execution 
  - query result processing
  - date 
    - https://github.com/HowardHinnant/date
    - wall_timer
      - cron syntax extended
  - sharding / re-hash
    - guid 
      - key for objects 
        - players, items, guilds
      - esstential to enable sharding and tracking
      - rather big (16 bytes)
  - performance testing
  - code generation in c++
    - MPL, macros, compiler

- [ ] convention cleanup 
  - struct types 
  - configuration

- [ ] change project configuration 
  - net
    - util, channel, net
  - server
    - net, actor, service
  - template  
    - instance, matcher, guild, 
    - detour / bullet / octree
    - item, inventory
    - character, pc, npc

- [ ] code template login_service 
  - database based

- [ ] grinder 
  - action flow based
  - functional test
  - stress test

- [ ] argos
   - shell 
     - command line interface
     - authentication with hash key and password per user
       - change key every version
     - encrypted communication
     - string based (json?)
     - cheat console
   - performance counter 
     - macro based recording 
     - realtime report to shell(s)
   - monitor server 
     - embed boost.beast w/ openssl to support web interface 
       - pause / resume with shell commands
     - web monitoring / management console
   - game operations
     - user monitoring
     - game play monitoring
     - game asset management
       - player characters
       - money
       - items

- [ ] network tests

   - stress tests
   - performance
   - thread scalability

- [ ] 30,000 session broadcasting

   - chatting
   - measure the limit of a single server on 8 core x64 windows

- [ ] meta
   - json, excel, csv
     - nlohmann's json library is great
     - excel editing interface is the core
   - conversion flow
     - json -> excel -> binary / text json format
   - schema merge keeping existing data
     - keep backups
     - date based
     - history log

- [ ] c# bitsery

   - c# message generation from c++ code
   - c++ spec serialization / deserialization for c# language
     - reflection based code generation
       - no runtime reflection
   - utf8 string support
   - c# client with async socket

- [ ] documentation
      - doxygen documentation
      - wiki
        - concepts
          - tutorial
        - samples

- [ ] live maintenance
  - maintenance cycle management
    - scheduling
    - state change per service
    - notification per service
      - to services
      - to users
  - file copy (embedded svn client and server)
  - system / server restart
  - service restart 

- [ ] game model 
  - demos of game model
    - board, strategic rpg, morpg
  - support module 
    - detour / bullet / octree
    - 1 bit voxel map : possible? 
  - content coding convention 
    - making abstract concepts
    - using higher level concepts
    - avoid script coding (long scenario based code manipulation)

- [ ] version and configuration management 

   - db schema
   - message
   - code change
   - development model 
   - tools to support

- [ ] cache_service

   - distributed with hash
   - algorithm 
     - TTL
     - check before loading 
     - update on change (loading, modification)
   - rocksdb?
     - embedded storage
   - still useful??? 
     - not sure... it can be. but cons are very strong.

- [x] json 
   - nlohmann's json 
     - just great!
   - debugging. 
     - get values while debugging. 
     - exception handling
   - loading
   - syntax 
     - support for easier syntax 
       - { name with space and without quotation : number without quotation }

- [x] safe static (mostly) type system

- [x] bits_message serialization

- [x] fix macro intellisense error

- [x] clear spdlog allocation error

   ```
    	[*** LOG ERROR ***][system] [bad allocation][2018-01-30 12:56:20]
    	spdlog::logger::_default_err_handler(const std::string &msg)
   ```

- [x] spdlog wchar to utf8 support 
      - test functionality 
      - define usage 
        - log()->info( L"fmt...", L"character")
        - works with log file.


