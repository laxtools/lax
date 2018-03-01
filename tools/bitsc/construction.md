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

## IDL spec. 

EBNF based definition for top down LL(1) parser.  message is the bitsery message. struct is a structure type to be used. key type in message field is used to construct domain / type pairs. 



schema = include* ( namespace_decl | type_decl | enum_decl  )*

include = `include` string_constant `;`

namespace_decl = `namespace` ident ( `.` ident )* `;`

type_decl = ( `message` | `struct` ) ident  `{` field_decl+ `}`

enum_decl = ( `enum` ident [ `:` type ] ) `{` commasep( enumval_decl ) `}`

field_decl = ident `:` type [ `=` scalar ] `;`

type = `bool` | `byte` | `ubyte` | `short` | `ushort` | `int` | `uint` | `float` | `long` | `ulong` | `double` | `int8` | `uint8` | `int16` | `uint16` | `int32` | `uint32`| `int64` | `uint64` | `float32` | `float64` | `string` | `[` type `]` | ident

enumval_decl = ident [ `=` integer_constant ]

scalar = integer_constant | float_constant | key_constant

single_value = scalar | string_constant

value = single_value | `[` commasep( value ) `]`

commasep(x) = [ x ( `,` x )* ]

integer_constant = `-?[0-9]+` | `true` | `false`

float_constant = `-?[0-9]+.[0-9]+((e|E)(+|-)?[0-9]+)?`

string_constant = `\".*?\\"`

ident = `[a-zA-Z_][a-zA-Z0-9_]*`

## Parser 

- understand flatbuffers idl_parser.cpp
- write a parser for new idl 





## Generator

- understand flatbuffers idl_gen_general.cpp 
- write a generator for new idl 





## Automation

- group / type management 
- message factory registration 





