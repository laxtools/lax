# TODOs

- [ ] server / service 

  - service clustering
    - startup / shutdown / update / TTL and alive message
    - service directory (local)
  - service lookup
    - multiple service instances on different servers 
  - local and remote references
    - send to references
    - sub to service channel
- [ ] master_server
  - service directory (global master)
  - controls services 
  - make decisions
  - recover itself getting states from services
- [ ] db
  - query runners
  - query execution 
  - query result processing
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
      - [ ] functional test
      - [ ] stress test
- [ ] argos
  - monitor server w/ shells 
    - embed boost.beast w/ openssl to support web interface 
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
- [ ] meta 

      - json, excel, csv
        - 쉽게 사용 가능한 json 라이브러리 찾기
        - dropbox version 사용성 개선 작업 
      - conversion flow
        - json -> excel -> csv
      - schema merge keeping existing data
        - keep backups
        - date based
        - history log
- [ ] documentation 

      - doxygen documentation 
      - wiki 
        - concepts 
          - tutorial
        - samples 
- [ ] live maintenance
  - maintenance cycle management
    - state change per service
    - notification per service
      - to services
      - to users
  - file copy (svn client)
  - system / server restart


  - service restart 

- [ ] support module 

      - [ ] detour / bullet / octree
      - [ ] ... 

- [ ] game model 

      - [ ] demos of game model
      - [ ] board, strategic rpg, morpg

- [x] safe static (mostly) type system

- [x] bits_message serialization

- [x] fix macro intellisense error

- [x] clear spdlog allocation error

   > 	[*** LOG ERROR ***][system] [bad allocation][2018-01-30 12:56:20]
   > 	spdlog::logger::_default_err_handler(const std::string &msg)

      - called get_desc() after service::error() get bad_alloc exception

- [x] spdlog wchar to utf8 support 
      - test functionality 
      - define usage 
        - log()->info( L"fmt...", L"character")
        - works with log file.




