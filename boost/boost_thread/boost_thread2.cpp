#include <iostream>   
#include <boost/thread.hpp>  
#include <stdio.h>
  
boost::thread AThread;  
  
void ThreadBody()  
{  
    std::cout << "Thread started." << std::endl;   
      
    try  
    {  
        while( true )  
        {  
            /** 手动在线程中加入中断点，中断点不影响其他语句执行 */  
            boost::this_thread::interruption_point();  
            std::cout << "Processing..." << std::endl;  
            sleep(1);  
        }   
    }  
    catch(...)  
    {   
        std::cout << "Interrupt exception was thrown." << std::endl;   
    }   
     
    /** 通过该语句可以判断线程先退出还是Destroy函数先退出 */  
    std::cout << "Leave Thread." << std::endl;  
}  
  
void Create()  
{  
    AThread = boost::thread( boost::bind( &ThreadBody ) );  
    std::cout << "Thread created with ID: " << AThread.get_id() << std::endl;  
}  
  
void Destroy()  
{  
    std::cout << "Interrupt thread with ID: " << AThread.get_id() << std::endl;  
      
    /** 向线程发送中断请求 */  
    AThread.interrupt();  
    std::cout << "Joining thread..." << std::endl;      
      
    /** join函数，作用是等待直到线程执行结束；可不加，但不能保证退出Destroy函数前线程被终结 */  
    AThread.join();  
    /** 通过该语句可以判断线程先退出还是Destroy函数先退出 */  
    std::cout << "Leave Destroy Function." << std::endl;  
}  
  
int main()  
{  
    Create(); 
    sleep(3);
    std::cout<<"enter to destroy thread"<<std::endl; 
		getchar();
    Destroy();  
    sleep(2);
    return 0;
} 