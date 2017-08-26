# Protocol msgpack 

 - MsgPack CSharp 
   - Has built in code generator 
   - ���� 
   - IL2CPP ����

 - msgpack-c has c++ code generation 
   - MSGPACK_DEFINE()

 - A simple IDL


 Ȯ���ϱ� ���� flatbuffers �ٽ� �ѹ� �� ��.
 offset���� �׳� �д� �Ͱ� ������Ʈ�� ���ϰ� serialize �ϴ� �� ������� ���� ������ ���̴µ�......


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

  

  



