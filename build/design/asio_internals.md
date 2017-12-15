# asio internals

 - design and implementation techniques 
 - c++ 11 features
 - boost like way 

## async_result.hpp

 - details/config.hpp 로 빌드 옵션들 정리 
 - comment style. brief를 ///로 분리 
 - detail namespace로 세부 구현 분리 
 - template의 verbosity를 macro로 줄임 

### handler_type 

 - type 지우는 specialization들 많음 
 - arg 5개까지 hander 타잎들 추가 

## basic_deadline_timer 

 - boost 에 의존 

## basic_io_object 

 - service_has_move traits
   - a magic. is it c++ code? 

 - const 버전과 그냥 버전. 
 
 공부를 많이 해야 한다. 

 - type safety가 기본적인 목표
 - reference로 변수를 갖고 있고 이를 사용하는 기법들 
   - raw pointer를 쓰지 않고 코딩 하기 

## basic_streambuf

 - std::streambuf를 상속 
   - gptr() (읽기), pptr() (쓰기) 
   - get 영역, put 영역으로 정의
 - remove, data 등 필요한 기능들을 갖고 있음 
 - streambuf 내 개념들과 사용법에 익숙해져야 함 
   - 성능 특성도 알고 있어야 함

 - cppreference.com 훌륭. 
   - 한 장의 그림으로 전체를 잘 설명 
   - 실제 사용해서 이해하도록 함 

 - locale 잘 모름 


## basic_stream_socket 

 - concept을 추출하는 능력이 이제 C++ 프로그래머의 능력이다. 
 - exception의 사용 
   - 복구 불가능한 오류를 코드 전체에 전파하기 보다는 exception을 사용한다. 
   - 적절하게 처리하는 부분이 있어야 한다. 
  
 - template function 
   - 타잎 추론 포함해서 쉽게 사용할 수 있는 generic interface 제공 
   - 대부분 이 쪽 기능 

 - type traits와 tag dispatching 중심으로만 사용 
   - 이미 있는 MPL 도구들 재활용 
   - enable_if와 같은 것들은 잘 안 보임 
   
## io_service 

 - A perfect example of good api with modern c++ style 

 - stdexcept 
   - 익센션들 종류들 확인. 이 쪽 것들 사용. 
   - parameter, domain, range, overflow 등 있음 
   - 없다면 상속해서 추가 설정

 - 컴파일 타임 다형성으로 구조화 가능 
   - 컴파일 시간이 느려지는 문제가 있으니 
   - 전통적인 wrapper를 시스템 상에 배치 
   - 이를 통해 효율화 가능 

### win_iocp_io_service

 - io_service_impl_ 의 한 종류
 - template class (i.e concept)
 - implementation class has less documentation 
 - ASIO_DEFINE_HANDLER_PTR
   - 매크로의 코드 생성 기능도 훌륭. 
   - 잘 활용하면 매우 좋다. 

## strand

 - strand_impl은 operation
   - 내부에 큐를 갖고 있음 

 - strand_service::dispatch 호출 

 - callstack을 뒤져서 바로 호출 가능한 지 확인 
   - callstack 구현은 협업 방식 
   - static에 호출 클래스들 등록 방식 

  
