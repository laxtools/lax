# msgpack IDL spec    

# Goal

 - ���� ��� ����. Ư��, C++ / C# 
 - C#���� reflection ���� �� ��

# ����

 - msgpack c++�� ���� 
 - funz c# Ŭ���̾�Ʈ �ۼ� 
 - spec���� c++ / c# �ڵ� ����

# Example 

## spec file

include "common/base.mpk"    
include "common/enums.mpk"    

namespace shared.npc; 

enum test_1 
{
	v1, 
	v2 = 3, 
	v3
};

class npc 
{
  	int32 			v = 0; 
	vector<int32> 	vs;
	map<string, attribute> as;
};

msg msg_update_npc_info 
{
	npc npc_;
};


## c++ generated file 

file : name_gen.h

#include "common/base_gen.h"
#include "common/enums_gen.h"

namespace shared { namespace npc { 

enum test_1 
{
	v1, 
	v2 = 3, 
	v3 
};

class npc 
{
public:
  	int32_t 				v = 0; 
	std::vector<int32_t> 	vs;
	std::map<std::string, attribute> as;

	MSGPACK_DEFINE(v, vs, as);
};

class msg_update_npc_info 
{
public: 
	npc npc_;

	MSGPACK_DEFINE(npc_);
};

} }


## c# generated file 

file: name_gen.cs

using common.base; 
using common.enums; 

namespace shared { namespace npc {

public class npc 
{
  	public int32 	v { get; set; } 
	public List<int> vs { get; set; }
	public Dictionary<String, attribute> { get; set; }

	// serialization / deserialization code generated
	// 
};

class msg_update_npc_info 
{
public: 
	npc npc_;

	// serialization / deserialization code generated
};

} }
