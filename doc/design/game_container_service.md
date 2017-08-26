# game container service 

docker에서 영감을 받았다. 

기본 서비스를 포함해서 구성된 플래폼 상에 게임 서비스가 연결되어 동작하도록 한다. 
동일 엔진이라면 프로토콜 맞추고 구성 가능할 것으로 보인다. 

asio가 openssl wrapping해서 ssl을 지원. 
여기에 http를 태워서 https를 만들고 전체 서비스가 https 기반으로 구성되도록 한다. 

python을 각 서버에 설치하지는 않고 모니터링, 개발 등에 사용할 수 있도록 제공. 
pyqt로 예쁜 UI를 만든다. 

laxo 서비스를 통해 클러스터를 구성하고 
여기를 통해 각 서버의 파일 전송, 설정, 실행, 종료, 예약 종료를 할 수 있도록 한다. 

laxo도 laxi를 포함하고 있도록 한다. 
파일 전송 기능도 갖고 있도록 한다. 
OS의 상태 전송 기능을 포함. PerfMon의 파라미터 조회 기능을 내부적으로 구현 

laxo를 emacs 처럼 os 수준의 container로 발전 시킨다. 

## 보안 

인증 수행. 
- oneway hash로 생성한 계정 / 비번 등록 
  - hash 값이 비번 
  - laxi에서 plain password --> hash 생성 기능 지원
- ip 등록 
- laxi / web 에서 접근 가능 

## 기능 

파일 / 폴더 업데이트 
- rsync 기능 
  - librsync 사용 

lax 설치 / 실행 / 삭제 
- laxi에서 config 한 후 구성 전송 후 실행 가능 
- 버전 확인 
  - 설치 / 갱신  
- 예약 실행 
- 예약 종료 
- lax 소켓 연결 및 상태 모니터링 
  - 기본 parameter json
  - laxi 연결(들)이 있으면 보내줌

시스템 정보 보기 
- perfmon의 내용을 전달 가능 
- https://askldjd.com/2011/01/05/a-pdh-helper-class-cpdhquery/

## laxi 기능 

모드 
 - shell 모드 
 - gui 모드 











