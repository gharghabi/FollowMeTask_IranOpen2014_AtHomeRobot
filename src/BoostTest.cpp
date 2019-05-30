#include <boost/thread.hpp>
#include <stdio.h>

void server_getpicture()
{
 
 std::cout<<"client start"<<std::endl;
       boost::this_thread::sleep(boost::posix_time::milliseconds(30));
}

int main()
{
  boost::thread client_Process(&server_getpicture); 
}
