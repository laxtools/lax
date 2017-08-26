# Protocol msgpack 

 - MsgPack CSharp 
   - Has built in code generator 
   - 빠름 
   - IL2CPP 지원

 - msgpack-c has c++ code generation 
   - MSGPACK_DEFINE()

 - A simple IDL


 확정하기 전에 flatbuffers 다시 한번 볼 것.
 offset으로 그냥 읽는 것과 오브젝트를 편리하게 serialize 하는 게 상충되지 않을 것으로 보이는데......


## Layout 

| Length(32) | Domain(16) | Type(16) | Payload

## Usage 

msg_example : msg_pack 
{
  int field1; 
  std::vector<int> field2; 

  ... 
  MSGPACK_DEFINE(MSGPACK_BASE(msg_pack), field1, field2, ... );

  virtual result pack( buffer& buf )
  {
		msgpack::pack(buf, *this);	
  }

  virtual result unpack( buffer& buf )
  {
		msgpack::object_handle oh = msgpack::unpack(buf.data(), buf.size());

		msgpack::object obj = oh.get();
		obj.convert(*this); // working? 
  }
}

session_json : session 
{
	result on_recv( const uint8_t* data, std::size_t len)
	{
		buffer buf(data, len);

		// from type, create a message

		auto m = factory::create( category | type );

		auto rc = m->unpack(buf);

		if ( rc )
		{
			get_svc().get_app()->on_recv( m );
		}

		return rc;
	}
}

  

  



