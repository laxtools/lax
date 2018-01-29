# Cipher Debugging 

## Loop 1

### Send

- msg_len = 32
- pad_size = 8
- final_block 
  - padding : 248, 55, 100, 75, 178, 211, 226, 86
  - bytes: 210, 0, 186, 88, 12, 91, 16, 36
- key 지정
  - ​

### Recv

- msg_len = 40 
- key 지정
  - 38, 198, 237, 43, 38, 198, 237, 43, 38, 198, 237, 43, 38, 198, ... 



## 원인 

nonce / key 변경 없으면 괜찮음 

sha1으로 해시 변경하고 진행. 

- data: 217, 78, 136, 134, 203, 148, 1, 43, 232, ... 
- data: 177, 137, 51, 46, 106, 72, 172, 230, 154, 40, 245, ... 

수신 시 update_hash()에서 pad_size를 반영 안 해서 

업데이트 되는 값이 달라진 게 원인으로 보임





# checksum debugging 

- 루프 돌면서 에러 



## 1st Loop 

### send 

- msg_len : 28 
- checksum : 135, 95, 5, 198 

### recv

- msg_len : 32
- checksum : 135, 95, 5, 198 



## 2nd Loop 

### send 

- msg_len : 28 
- ​