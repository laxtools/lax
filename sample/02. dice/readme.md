# dice 

 - a full featured room based game
 - matching, persistence, turns are implemented. 

## game 

 - two players are matched
 - when there is only odd number of players, then a bot is used to begin
 - when game starts, a room is created. 
 - then users join the room
 - then one player rolls a dice, shows the number
 - then the otehr player or bot rolls a dice, shows the number
 - a rolled dice with a higher number wins. 
 - result is saved as win or lose

 rule is simple, but the overall architecture is not easy to implement.


## architecture

 - login
 - matcher
 - directory
 - game 
 - db sharding

 players changes game server if the room to join is on another server.

 it becomes to big to be a sample.

 make it simple: 

 - one server with following tasks: 
   - login
   - matcher
   - game 

 - and login and save db queries


