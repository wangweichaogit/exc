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
            /** �ֶ����߳��м����жϵ㣬�жϵ㲻Ӱ���������ִ�� */  
            boost::this_thread::interruption_point();  
            std::cout << "Processing..." << std::endl;  
            sleep(1);  
        }   
    }  
    catch(...)  
    {   
        std::cout << "Interrupt exception was thrown." << std::endl;   
    }   
     
    /** ͨ�����������ж��߳����˳�����Destroy�������˳� */  
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
      
    /** ���̷߳����ж����� */  
    AThread.interrupt();  
    std::cout << "Joining thread..." << std::endl;      
      
    /** join�����������ǵȴ�ֱ���߳�ִ�н������ɲ��ӣ������ܱ�֤�˳�Destroy����ǰ�̱߳��ս� */  
    AThread.join();  
    /** ͨ�����������ж��߳����˳�����Destroy�������˳� */  
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