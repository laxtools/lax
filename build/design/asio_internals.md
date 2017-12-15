# asio internals

 - design and implementation techniques 
 - c++ 11 features
 - boost like way 

## async_result.hpp

 - details/config.hpp �� ���� �ɼǵ� ���� 
 - comment style. brief�� ///�� �и� 
 - detail namespace�� ���� ���� �и� 
 - template�� verbosity�� macro�� ���� 

### handler_type 

 - type ����� specialization�� ���� 
 - arg 5������ hander Ÿ�ٵ� �߰� 

## basic_deadline_timer 

 - boost �� ���� 

## basic_io_object 

 - service_has_move traits
   - a magic. is it c++ code? 

 - const ������ �׳� ����. 
 
 ���θ� ���� �ؾ� �Ѵ�. 

 - type safety�� �⺻���� ��ǥ
 - reference�� ������ ���� �ְ� �̸� ����ϴ� ����� 
   - raw pointer�� ���� �ʰ� �ڵ� �ϱ� 

## basic_streambuf

 - std::streambuf�� ��� 
   - gptr() (�б�), pptr() (����) 
   - get ����, put �������� ����
 - remove, data �� �ʿ��� ��ɵ��� ���� ���� 
 - streambuf �� ������ ������ �ͼ������� �� 
   - ���� Ư���� �˰� �־�� ��

 - cppreference.com �Ǹ�. 
   - �� ���� �׸����� ��ü�� �� ���� 
   - ���� ����ؼ� �����ϵ��� �� 

 - locale �� �� 


## basic_stream_socket 

 - concept�� �����ϴ� �ɷ��� ���� C++ ���α׷����� �ɷ��̴�. 
 - exception�� ��� 
   - ���� �Ұ����� ������ �ڵ� ��ü�� �����ϱ� ���ٴ� exception�� ����Ѵ�. 
   - �����ϰ� ó���ϴ� �κ��� �־�� �Ѵ�. 
  
 - template function 
   - Ÿ�� �߷� �����ؼ� ���� ����� �� �ִ� generic interface ���� 
   - ��κ� �� �� ��� 

 - type traits�� tag dispatching �߽����θ� ��� 
   - �̹� �ִ� MPL ������ ��Ȱ�� 
   - enable_if�� ���� �͵��� �� �� ���� 
   
## io_service 

 - A perfect example of good api with modern c++ style 

 - stdexcept 
   - �ͼ��ǵ� ������ Ȯ��. �� �� �͵� ���. 
   - parameter, domain, range, overflow �� ���� 
   - ���ٸ� ����ؼ� �߰� ����

 - ������ Ÿ�� ���������� ����ȭ ���� 
   - ������ �ð��� �������� ������ ������ 
   - �������� wrapper�� �ý��� �� ��ġ 
   - �̸� ���� ȿ��ȭ ���� 

### win_iocp_io_service

 - io_service_impl_ �� �� ����
 - template class (i.e concept)
 - implementation class has less documentation 
 - ASIO_DEFINE_HANDLER_PTR
   - ��ũ���� �ڵ� ���� ��ɵ� �Ǹ�. 
   - �� Ȱ���ϸ� �ſ� ����. 

## strand

 - strand_impl�� operation
   - ���ο� ť�� ���� ���� 

 - strand_service::dispatch ȣ�� 

 - callstack�� ������ �ٷ� ȣ�� ������ �� Ȯ�� 
   - callstack ������ ���� ��� 
   - static�� ȣ�� Ŭ������ ��� ��� 

  
