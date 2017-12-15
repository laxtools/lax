# key_t의 추가 타잎 정의와 사용

channel :

using key_t = std::string 일 경우와 나중에
using key_t = uint32_t 로 변경할 경우

const key_t& get_key() const {

}

함수의 signature를 어떻게 해야 할까?

uint32_t로 할 경우는 const key_t& get_key() const 대신
key_t get_key()로 충분하다.

using const_ref_key_t = uint32_t 와 같이 길게 정의할 수 있으나
별로 좋은 방법은 아니다.
