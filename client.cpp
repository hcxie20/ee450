#include"iostream"
#include"stdio.h"
#include"stdlib.h"
#include"sys/socket.h"
#include"sys/types.h"
#include"sys/wait.h"
#include"errno.h"
#include"netinet/in.h"
#include"string.h"
#include"arpa/inet.h"
#include"unistd.h"
#include"netdb.h"
#include"myClass.h"

#define PORT_AWS 24759
#define LOCAL_HOST "127.0.0.1"
#define MAX_LEN 100


int main(int argc, char *argv[])
{
    // TO DO 输入检查
    // an easy input check
    if(argc<4){
        std::cout<<"More args needed."<<std::endl;
        exit(1);
    }
    std::cout<<"The client is up and running.\n";

    // decode message to argvs
    char msg_send[MAX_LEN];
    memset(msg_send, 0, MAX_LEN);
    if(strcmp(argv[1], "write")==0){
        for(int i=1;i<6;i++)
        {
            strcat(msg_send, argv[i]);
            strcat(msg_send, "&");
        }
    }
    else{
        for(int i=1;i<5;i++)
        {
            strcat(msg_send, argv[i]);
            strcat(msg_send, "&");
        }
        strcat(msg_send, "0");
        strcat(msg_send, "&");
    }

    // open socket to connect to aws
    class Socket sock_aws("TCP");
    struct sockaddr_in aws_addr;
    set_addr_in(aws_addr, AF_INET, htons(PORT_AWS), inet_addr(LOCAL_HOST));

    sock_aws.Connect_(aws_addr);

    sock_aws.Send_(msg_send);

    if(strcmp(argv[1], "write")==0){
        std::cout<<"The client sent write operation to AWS"<<std::endl;
    }
    else if(strcmp(argv[1], "compute")==0){
        std::cout<<"The client sent ID="<<argv[2]<<", size="<<argv[3]<<", power="<<argv[4]<<" to AWS\n";
        }

    // receive from aws
    char msg_recv[MAX_LEN];
    memset(msg_recv, 0, MAX_LEN);
    sock_aws.Recv_(msg_recv);

    if(strcmp(argv[1], "write")==0){
        std::cout<<"The write operation has been completed successfully"<<std::endl;
    }
    else{
        std::cout<<msg_recv<<std::endl;
    }
    sock_aws.Close_();
    return 0;
}