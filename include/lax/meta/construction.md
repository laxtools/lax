# meta

스키마 기반 Excel, C++ 코드 생성과 csv 변환. csv 로딩과 테이블 관리. 

## 방향 

- 빠른 로딩과 리로딩
  - 리로딩 시 포인터 자동 갱신
    - item_table::row::ptr item_elem
    - item_elem::operator-> 에서 table 로딩 시간 확인 후 다시 얻기
    - 개발 모드에서만 활성화 시킴
  - 자동 리로딩 
    - 파일 변경 시간을 확인하고 자동 리로딩 진행
- 편집과 조회가 편한 excel 
  - 컬러링 
  - 스크롤
  - enum 필드 조회 
  - sheet 분할 
- 스키마 버전 관리  
  - excel 파일의 데이터와 내용을 보존하면서 스키마 변경 가능
  - csv 파일 버전 관리 
    - 자동 생성 및 갱신 기능 



## 스키마  

- column based 
  - Like RDBMS 
  - name, type, range, refer : schema:name
- column sub table
  - a table inside a column 
- header 
  - sequence
  - row / sub
    - sub means sub table entry 
- key column 



## CSV 



- std::wstring for multi-lingual support. 
- language type 
  - main, service attribute 
- use validation c++ code generated from schema 











