# TODOs

- [ ] server / service 

  - service clustering
  - service lookup
  - local and remote references
    - send to references
    - sub to service channel

- [ ] master_service

      - controls all services 
      - final decision maker 
      - recover itself getting states from services

- [ ] argos
  - monitor server w/ shells (console, web, etc)

- [ ] network tests
  - stress tests
  - performance 
  - thread scalability

- [ ] change project configuration 
  - lax_server 
    - all lax and boost
  - lax_client 
    - util, channel, net 

- [x] db

- [x] login_service 

  - database based

- [x] code template instance

  - instance domain 


  - instance_control_service
  - instance_runner_service
  - instance actor

- [x] code template match

  - match domain
  - match_maker_service

- [ ] embed boost.beast w/ openssl to support web interface 

- [ ] safe static (mostly) type system  : **done**

      - actor_container 
      - actor for components

- [ ] bits_message serialization : **done**

      - enable base class serialization 
        - test 

- [ ] fix macro intellisense error : **done**

      - put cpp.hint at root 
      - delete VC.db, ipch, SDL files 
      - clear asserts : **done**
        - serveral assertion errors 
        - clear each w/ care

- [ ] clear spdlog allocation error - **done**

      - ```[*** LOG ERROR ***][system] [bad allocation][2018-01-30 12:56:20]```
      - spdlog::logger::_default_err_handler(const std::string &msg)
      - 이미 지워진 session에 대해 처리됨
      - service::error()를 get_desc() 전에 호출해서 생긴 문제

- [ ] spdlog wchar to utf8 support - **done**

      - test functionality 
      - define usage 
        - log()->info( L"fmt...", L"character")
        - console은 utf8이라 출력이 제대로 안 됨
        - 파일은 출력이 정확하게 됨

      ​