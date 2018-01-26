# actor 

 모든 것이 액터나 액터에서 사용하는 것들이다. 

 - task_actor 
 - frame_actor 
 - actor_pool : 중요. 타잎별 할당 개수, 풀에 있는 개수 추적. 

## component 

 - building blocks 
 - still design for each problem is important
 - don't put everythin into components

## scheme 

 - create types
   - smaller, concept level 
 - combine types
 - execute data 

## patternization 

 - everything has its own patterns. 
 - find them as concepts and as type.
 - find relations between them 




## server 

서버. service들 실행. 각 service는 자체 액터 관리. 

service는 task_actor로 함. 

xml로 구성 파일 작성. 클러스터 관리.  연결 관리. 서비스 상태 관리. 

service는 자체 채널 보유. service_factory에서 생성. 문자열 이름을 사용. 

로컬 / 리모트 serivce warpping. locational transparency 제공. 

- 통신 프로토콜의 실행

- task_scheduler 포함. 

- service 생성과 실행 

- peer들 연결과 관리 

- 로컬 / 리모트 서비스 상태 관리

- 리모트 서비스의 ref 전달 

- 호스트 상태를 관리기에 전달 

  - 메모리 / io / 세션 등 
  - windows 관리 서비스 연동 

  ?

### service

- server를 통해 peer들에 상태 전파 
- 관리 기능 실행 
  - 통지 시작 / 중단 
  - counter 전달 

























