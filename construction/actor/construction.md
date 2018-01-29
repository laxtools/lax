# actor 

 애플리케이션은 액터나 액터에서 사용하는 것들로 구성된다. 

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

## 타잎 시스템 
 
 여러 액터들을 처리할 때 타잎을 알아야 할 필요가 있다. 
 공통의 인터페이스에 대해 처리할 수 있어야 한다. 
 C++ 동적인 캐스팅을 통해 가능하다. dynamic_cast를 
 사용해도 괜찮은가? is_a( npc ), is_a( boss ) 같은 기능이 필요한가? 
 실제 npc, boss 데이터 실행기와 연관된다. 다른 관점이 필요하다. 


## 정리 

 액터를 엔진 단에서 구현할 부분은 많지 않다. 
 이미 채널을 통해 통신이 가능하고  태스크로 실행 기능을 분리했기 때문이다.

 단지, 매우 중요한 영역인데 편하게 개발할 수 있는 수단은 
 아직 충분하지 않다. 게임을 만들면서 필요한 편의 기능을 확보한다. 

 