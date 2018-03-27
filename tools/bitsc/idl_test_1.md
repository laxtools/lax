```c++
struct position 
{
	float x = 0.0f;
	float y = 0.0f; 
	float z = 0.0f;
}
```
```c++
message req_move : msg_game_play_base
{
   topic game.play.move; 
   position to; 
   int32 id(10, 100*1000);
   string name;
}
```
