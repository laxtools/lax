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
  - sharding / re-hash
  - date 
    - https://github.com/HowardHinnant/date
    - wall_timer
      - cron syntax extended
  - guid 
    - key for objects 
      - players, items, guilds
    - esstential to enable sharding and tracking
    - rather big (16 bytes)
  - code generation in c++
    - MPL, macros, compiler
  - performance testing

- [ ] Convention cleanup 
      - [ ] all using types are named *_t 
      - [ ] configuration
      - [ ] debug and trace log added

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
     - encrypted communication 
     - string based (json?)
     - cheat console
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

- [ ] - - - ​

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




