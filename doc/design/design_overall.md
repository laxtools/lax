# Design 

funz is an online game server framework with : 

 - fast tcp communication
 - distributed and persistent actors 
 - modules 
 - actor components
 - development and management console 
 
funz is based on : 

 - modern c++
   - 11 / 14 mostly  
 - open source libraries 
   - asio as communication wrapper
   - spdlog for logging
   - caf (c++ actor framework) to borrow concepts and design
   - json for text protocol
   - recast / detour for navigation
   - bullet for collision detection 
 
 
 ## Approach 
 
funz develops with: 

 - tests to explore
 - tests to design  
 - tests to confirm 
 - tests to verify 
  
funz has a console from the beginning: 

 - to view communication
 - to view actors and manage actors 
   - which is to view the whole server 
   
## Stages 

funz has following dev. stages : 

 - log 
 - net
 - actor 
  - heart. core. 
  - clustering
  - messaging
  - continuation
 - game 
  - entity / entity_container 
 - module 
   - database 
   - world 
     - instance / navigation / collision   

  
