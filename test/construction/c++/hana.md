# Hana 

A metaprogramming library.  visual studio 2015, 2017에서 동작하지 않는다.  

발전되면 따라간다. MPL의 끝판왕이자 새로운 계산 모델을 타잎과 실행 시간을 엮어서 제공한다. value 기반 (constexpr) reflection은 표준에 포함될 가능성이 높다.  

## type system 

goal: 

- static type safe type reflection 
  - single, multiple inheritance
- is_a<Type> 
- cast<Type> 
- container 
  - add<Type>()
  - del<Type>()
  - get<Type>()



그래도 다행히 typeid 기반으로 단일 상속 타잎 시스템은 구현 가능하다. visual c++에서 구현하고 component, actor에 사용한다. 이제 actor를 생성할 때 enum 값으로 타잎을 지정하고 캐스팅하지 않아도 된다. 실수할 여지를 줄이고 디버깅 할 때도 타잎을 출력할 수 있게 되었다. 



