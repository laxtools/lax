# game schema generator & loader 

- schema defintion in json 
  - like bitsc idl parser and generator 
  - hcsv 
    - n-level hierarchy 
    - foriegn key reference
- generation 
  - each language object and loader 
    - csv loader 
    - validation 
      - foriegn key 
      - key 
  - excel generator 
    - coloring 
    - foriegn key / enum table lookup 
  - excel csv converter 



## Schema 

```javascript
{
    "skill_type" : {
 		"type" : "enum", 
        "values" : [
        	{ "name" : "unused"}, 
            { "name" : "main_skill" }, 
            { "name" : "active_skill" }             
        ]
    }, 
         
    "skill" : {
        "type" : "table", 
            
    	"fields": [
       		{ "name" : "index", "type": "int32", "primary_key": true }, 
       		{ "name" : "desc_main" }, 
            { "name" : "desc", "index" : true }, // non-unique key
       		{ "name" : "level", "type": "int32" }, 
       		{ "name" : "type", "type": "skill_type" }, 
            { 
                "name" : "timing", "type" : "nested", 
                
                "fields": [
                	{ "name" : "l1", "type" : "float" },     
                    { "name" : "l2", "type" : "float" }, 
                    { "name" : "l3", "type" : "float" } 
                ]
            }
            // ... 
    	]
    }
    
    // struct elem_skill { 
    //   struct timing { float l1; float l2; float l3; }; 
    //   int32_t index; 
    //   std::string desc_main; 
    //   std::string desc; 
    //   int32_t level; 
    //   skill_type type; 
    //   timing timing_in; 
    // };        
    // 
    
    // skill_table class to find with keys 
    // - get ref to skill (not shared_ptr)
    // - automatic reloading support for development
    // 
}
```



