#include <iostream>

using namespace std;

template <class T>
class malloc_allocator
{
public:  
  
      typedef T                 value_type;  
  
      typedef value_type*       pointer;  
  
      typedef const value_type* const_pointer;  
  
      typedef value_type&       reference;  
  
      typedef const value_type& const_reference;  
  
      typedef std::size_t       size_type;  
  
      typedef std::ptrdiff_t    difference_type;  
  		pointer allocate(size_type n, const_pointer = 0) {  
  
        void* p = std::malloc(n * sizeof(T));  
  
        if (!p)  
  
          throw std::bad_alloc();  
  
        return static_cast<pointer>(p);  
  
      }  
  
      void deallocate(pointer p, size_type) {  
  
        std::free(p);  
  
      }  
}