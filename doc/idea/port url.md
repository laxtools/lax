# sketch

host:port/u/login
host:port/u/logout


cd
 - current
 - multiple function call
 - rpc like

msgpack://host:port/g/i1899/e735/login
json://host:port/g/i1899/e735/turn


http like tcp

# forward chaining

 msg_point
 - has a key. that is a point
 - sub()
 - unsub()
 - recv()
 - post()

 constructs a tree of recv chain.

 tx / rx message queue

## example

 msg_point("g")
  - sub("i1899")

# view as a queue     

 - tx
   -  call subscribed functions

 - rx
   - call subscribed functions

 mq.sub( key, mq::recv, function)
 mq.sub( key, mq::send, function)

 mq.recv(m1)
 mq.send(m2)

## two aspects

 task internal queues
 - tx qs
 - rx qs

 chaining mode
 - constructs a tree

## ADT

 concepts:
 - queue / subscription
   - push / post
   - immediate mode / internal mode
     - when to process it
   - msg_queue
     - policy based configuration
     - immediate rx post
     - immediate tx post
     - rx_q / tx_q

  - path key
    - /g/a/b/c
    - ../k
    - d

  example:
    - msg forwarder tree
    - root (/)          
      - /a
      - /b
         - i3876
      - /c
      root.sub( )

     - recv:
       - root --> /b --> i3876

    - send:
       - i3876.tx( m )

# Message Queue Interface

 여러 가지 다양한 구현들이 있다.

# Message Queue Interface

 여러 가지 다양한 구현들이 있다.
 QCONN, MQCONNX, and MQDISC
 MQOPEN and MQCLOSE
 MQPUT and MQPUT1
 MQGET
 MQSUB, MQSUBRQ
 MQINQ
 MQSET
 MQBEGIN, MQCMIT, and MQBACK
 MQCRTMH, MQBUFMH, MQMHBUF, MQDLTMH
 MQSETMP, MQINQMP, MQDLTMP
 MQCB, MQCB_FUNCTION, MQCTL
 MQSTAT

 위와 유사한 방식의 처리. 어떻게 가능할까?             

 mq:
  - create( path key, mp::ptr parent )
  - post( msg )
    - dispatch with msg->get_topic()
  - sub( )
  - send( )
    - post with 'send' topic

 msg has a key and a topic
  - std::string

 channel 연결
   - sub::ref mq.sub( topic, channel의 전송 함수 );
   - mq.unsub( sub::ref );

topic and path key subscription  
- locally unique
- string key

topic format
- category.message
- ls.login, gs.turn,
- topic class로 처리
- container 키로 사용 가능.
- stream 출력 가능

path
- 유효성 체크
- 절대 / 상대 경로 파싱 지원
- container 키로 사용 가능
- stream 출력 가능

  
