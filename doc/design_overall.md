# Design 

lax is an online game server engine with : 

 - fast tcp communication
 - distributed and persistent actors 
 - modules 
 - actor components
 - development and management console 
 
lax is based on : 

 - modern c++
   - 11 / 14 mostly  
 - open source libraries 
   - asio as communication wrapper
   - spdlog for logging
   - json for text protocol 
   - crypto++ for encryption 
   - recast / detour for navigation
   - bullet for collision detection 
 
 ## Approach 
 
lax develops with: 

 - tests to research 
 - tests to design  
 - tests to confirm 
 - tests to verify 
  
lax has a console from the beginning: 

 - to view communication
 - to view actors and manage actors 
   - which is to view the whole server 
   
## Stages 

lax has following dev. stages : 

 - log 
 - net
 - actor 
  - heart. core. 
 - game 
   - entity
   - module 
   	 - database 
     - world 
       - instance / navigation / collision   

  
