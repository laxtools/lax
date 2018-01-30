# server 



## server 

- starts net::service
- listens on protocols
- connects to peers
  - send server information
  - fixed id
- starts services from json
  - using task_adaptor
- starts a task_scheduler
- manages service_directory
  - reference to service
    - local or remote
    - acquire 
      - like session::ref
      - sub to state
  - state update
- not a singleton
  - to test two servers in a unittest

## service

- an actor
- update state to peers
  - keepalive_interval
- that's all




## Instance Architecture

A instance oriented game server. Well known examples are FPS, MORPG, board games, and MOBA games. Most games are instance based except some MMORPGs providing seamless world. But even those MMORPGs can be developed with instances if smooth instance transition is possible under game design.

## Server and Services

- front servers running:
  - instance_runner_service(s)
    - runs instance actors from different worlds
  - lobby_service
    - keep users
    - relay backend messages
  - login_service
- world backend servers running:
  - world_service
    - authorize
    - keeps users
    - relays world traffic
  - guild_service
  - instance_control_service
  - world / guild subs to user state messages
    - copied user state
- master backend server running:
  - master_service
    - controls services 
      - start / stop services on front servers

## Stability 

world backend services are failure points 

- recovery process
  - notify users on lobby_service joined the world
  - keep each instance of the world in suspended mode 
  - restart the world server 
  - when services of the world is up 
    - notify users
    - update state of user, guild, instance 
  - when services are up
    - notify users
    - resume instances

# Match Making Service

1 billion match making. 

- match_service
  - 10K match requests per service
  - dynamic partition of ranges with transition
- range partition
  - when flooded
  - (l, u) -> (l, m) (m, u)
  - create a new match_service on any server with (m, u)
  - advertise new range to all related services
- range merge
  - when idle for a long time
    - ask merge of partition
    - if agreed, then extend range and stop one the two 






