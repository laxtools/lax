# concepts for distributed systems

## local

### message

 - message has a topic
 - topic has a story (payload)

### channel

 - channel has a queue
 - channel has subscriptions
 - messages are in a mx for some time
 - messages can be push / pop from a queue

 - interface:
   - sub() / unsub()
   - push( msg )
   - push( topic, msg )
   - post()

 - subscription map
   - topic: [ cond, func ]
   - shared lock 사용
   - map 분리할 지는 나중에 보고 판단

 - immediate / delayed  
   - 즉시는 push 할 때 처리
   - 나중은 post() 호출에서 처리

### topic subscriptions

 - subscriptions to consume messages
 - subscriptions to chain message pipeline

 - condition check:
   - sub( topic, condition, callback )
   - sub( topic, callback )

 - sub / subs
   - smart subscription handling
   - sub.reset(), ~sub()

## distributed

### bundle

 - create(protocol::ptr)
   - key registered to net::service
 - destroy()
 - has a channel
 - topics:
   - accepted / connected, disconnected
   - open, closed, recv,

### fiber

recv:
 - bundle topic recv
   - callback handles the message

send:
 - fiber::ref
    - has a bundle key
    - has a unique key for fiber
    - 연결 시 상위 프로토콜로 알림
    - channel::ptr을 ref에서 얻음
      - ref.settle()

close:
  - closer per each bundle
  - 알리고, 대기하고, 완전히 끊음
  - 상위 프로토콜 처리

### protocol

 - message factory
 - serialization
 - negotiation
 - periodic processing
 - encryption / decryption
