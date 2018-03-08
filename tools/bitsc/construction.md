# bits IDL parser and generator 

- include 
- struct 
- enum 
- message
- types 
- c++ / c# 
  - c# runtime 
  - c# networking code 
    - Async Socket 

This makes bitsery perfect for all languages. Traditional approach is fast enough or fastest. This makes the ground for future enhancements. 

## Schema 

Instead of writing a parser, json can be used to define message. This direction change is decided since the same approach can cover game data schema, db schema and other schemas required. 

A simple pair can be used to define name: type sequence making the unique variable name as a key in JSON. 

```json
{

 "include": [ "file1", "file2", ... ], // #include "file1.hpp" ... 

 "namespace" : "game.play",  // namespace game { namespace play { } }

 "update_type" : {

 "type" : "enum", 
 "values" : 
           [
              "type1", 
           	  "type2", 
              "type3=4", 
              "type5", ...  
           ]

 }, // enum update_type { type1, type2, type3 = 4, type5 };

  "position" : { 

    "type": "struct",  

    "fields": { "x=0" : "float", "y=0" : "float", "z=0" : "float" }
   }, // struct position { float x = 0; float y = 0; float z = 0; }

   "req_move" : {

    "type" : "message",
       
    "base" : "game.play.msg_instance_base", 

    "topic" :  "game.req_move", 
    
    "options" : { "bind_user_id_from_session" : true, ... }

    "fields" : {  
        "to" : "game.play.position", 
        "id=0" : "int32", 
        ...   
    } 
  } // struct msg_req_move : public game::play::msg_instance_base { 
    //    game::play::position to; 
	//    int32 id = 0;  
    // }
}
```

- array 
  - [ Type, length(5) ]
- type options
  - slot : "uint8, range(0, 32)"
  - option(value1, value2, ... ) format



## Generation 

Now we can focus on generation of target language code.  Here we focus on c++ code generation first and then extend to other languages. 

- process included files 
  - generate .hpp and .cpp if .bits file is newer than target files 
  - keep type information from the included files
- process each keys 
  - validate type references 
  - validate c++ syntax (avoid c++ keywords)
  - validate default values
- for messages
  - register messages 
  - create topic enums 
  - increase version number

generation adds reflection information to each class guarded with LAX_ENABLE_MESSAGE_REFLECTION. 

