# Protocol Json

 - 관리용 / 조회용 프로토콜 
 - 프로토타잎용 

## Spec 

 { "key": value, "msg" : value }

 - key : used to dispatch 
 - msg : used for data 

## Usage 

 msg_json::build(json::Json& obj);

 session_json::send(std::shared_ptr<msg_json> mp);

 app::on_recv(id, mp);




