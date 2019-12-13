#include"iostream"
#include"stdio.h"
#include"stdlib.h"
#include"sys/socket.h"
#include"sys/types.h"
#include"sys/wait.h"
#include"errno.h"
#include"netinet/in.h"
#include"string.h"
#include"string"
#include"sstream"
#include"arpa/inet.h"
#include"unistd.h"
#include"fstream"
#include"myClass.h"

#define PORT_SA_UDP 21759
#define PORT_SB_UDP 22759
#define PORT_AWS_UDP 23759
#define PORT_CL_TCP 24759
#define PORT_MO_TCP 25759

#define LOCAL_HOST "127.0.0.1"

#define MAX_LEN 100


int main(void){
    std::cout<<"The monitor is up and running.\n";

    struct sockaddr_in addr_aws;
    set_addr_in(addr_aws, AF_INET, htons(PORT_MO_TCP), inet_addr(LOCAL_HOST));

    class Socket socket_aws("TCP");
    socket_aws.Connect_(addr_aws);
    char msg[1000];

    while(1){
        memset(msg, 0, sizeof(msg));
        socket_aws.Recv_(msg);
        std::cout<<msg<<std::endl;
    }
    socket_aws.Close_();
}