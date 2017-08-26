# net 

## session 기반 

transport를 추가하는 것을 생각해 봤지만 결국 session 과 연결되고 
추가로 생성 해제가 필요하게 되기 때문에 그냥 session에서 처리하는 것으로 한다. 


## transport 

역할: 
 - 패킷 만들기 
 - 암호화 
 - 상위 통지 
 
msgpack_transport : public transport
{
public: 
	msgpack_session(app* app);

	bind(session::ptr ss);
	unbind();

	send(p, len); 
	recv(p, len); --> app::recv(msg)
	
	session::ptr create();
	transport::ptr clone();
}

## net

net::service h(cfg);

h.listen( "127.0.0.1:9999", msgpack_transport());
h.connect( "127.0.0.1:9999", msgpack_transport());

 // destructor cleans up all

