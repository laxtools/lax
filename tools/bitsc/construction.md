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

This makes bitsery perfect for all languages. 
Traditional approach is fast enough or fastest. 
This makes the ground for future enhancements. 

## Schema 

boost.spirit is a tool to write a parser fairly easily. It is used to develop a IDL parser.

```json
include "path.to.inc.idl"

namespace game.play

enum sample_1 {
    value1 = sample_2.value,
    value2, 
    value3
}

struct position 
{
    float x; 
    float y; 
    float z = 0.0f;
}

message req_move 
{
    topic game.play.move;
    game.play.position dest; 
    int32 array[5];  		// max length 5
    string name[10];	// string length 10
    string vec[];  		// max string size
}
```



## Generation 

Now we can focus on generation of target language code.  
Here we focus on c++ code generation first and then extend to other languages. 

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

generation adds reflection information to each class guarded 
with LAX_ENABLE_MESSAGE_REFLECTION. 

# Implementation 

- parser 
  - parser tree
- generator

generation of target code can be executed during parsing even with parse error.  The other way is to parse the whole IDL and then generate target language file. 

To drive design, test with test code. 



## Test Code 

- parse idl
  - build a parser tree 
    - it is a sequence of type objects 
  - add types as required 
  - validate when possible 
- c++ generator
  - iterate over type objects 




While writing test code, objects to parse entity are added. For generation, a generator interface for each parse entity are required. 

In each language, we have terminal and non-terminal nodes with production rules. Each term is the parse entity. Top-down parsing forces each term become a class to parse itself. 






