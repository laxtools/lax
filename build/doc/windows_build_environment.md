# Windows Build Environment 

Projects 

 - net 
 - actor
 - test 

## SDK 의존성 

 타겟 플래폼 지정

 - windows 
 - 8.1 SDK 

 VS 2015에서 생성할 경우 정상적으로 동작. 

 각 net / actor는 라이브러리로 생성. 

## net 

Networking with TCP. 

## actor 

Core. 

게임 개발을 위해 작고 가볍고 빠르고 사용하기 쉬운 액터 개발. 

 - 분산 처리 
   - 액터 상태 
   - 액터 위치 
 - 장애 대응 
   - persistent actor 
   - replicated actor 
     - active / active 
	 - active / standby 
 - 액터 수명 관리 
 - 타이머 
 - 이어서 처리 하기
 - 컴포넌트 
   - 게임 로직 구조화 
   - HFSM과 연동 
 - 들여다 보기 
   - 상태 조회 
   - 상태 변경 


## game 

게임 모듈과 개별 게임의 개발. 

