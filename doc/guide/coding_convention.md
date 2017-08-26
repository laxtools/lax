# Coding Convention 

## Layout 

### Header

한정 순서는 다음과 같이 합니다. 

 - public 
 - protected 
 - private 

타잎을 제일 앞에 두고 함수, 변수 순으로 합니다. 

### Implementation 

선언된 순서대로 구현합니다. 

그 외에는 특별한 제한은 없습니다. 

### Indentation

namespace는 들여쓰기를 하지 않습니다. 

{} (중괄호)는 반드시 라인을 변경합니다. 

asio는 공백 두개 들여쓰기를 하고 있는데 4개로 합니다.  
80 컬럼 제한에서 100 컬럼 제한으로 늘립니다. 

## Naming 

asio의 컨벤션을 따릅니다. 

 - 소문자로 _로 연결하는 이름을 사용합니다. 
 - template argument는 Pascal case를 사용합니다. 
 - 멤버 변수는 끝에 _를 둡니다. 

## 주석 

asio의 컨벤션을 따릅니다. 
 
 - 주석의 brief 설명과 상세 설명 규칙도 그대로 따릅니다. 
   - 모든 public 함수는 주석을 둡니다. 
 - javadoc의 tag를 사용합니다. 

## C++ 
 
 - C++ 11/14/17에서 제공 되는 기능이 있으면 적극적으로 사용합니다. 
 - 네트워크 라이브러리 호환의 목표 플래폼은 다음과 같습니다. 
   - windows, linux, android, iphone, ios 


