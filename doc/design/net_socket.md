# net 

## session ��� 

transport�� �߰��ϴ� ���� ������ ������ �ᱹ session �� ����ǰ� 
�߰��� ���� ������ �ʿ��ϰ� �Ǳ� ������ �׳� session���� ó���ϴ� ������ �Ѵ�. 


## transport 

����: 
 - ��Ŷ ����� 
 - ��ȣȭ 
 - ���� ���� 
 
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

