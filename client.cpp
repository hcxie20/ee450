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

class Socket
{
    int socket_;
    public:
    Socket(const char* type);
    Socket(int s);
    int Set_(int n);
    int Bind_(struct sockaddr_in addr);
    int Listen_(int port);
    int Accept_(struct sockaddr_in *addr_dist, socklen_t sin_size);
    int Connect_(struct sockaddr_in addr_dist);
    int Recv_(char *msg);
    int Send_(const char *msg);
    int Sendto_(const char *msg, struct sockaddr_in addr_their, int tolen);
    int Recvfrom_(char *msg, struct sockaddr_in *addr_their, socklen_t addr_len);
    int Close_(void);
    int Getsockname_(void);
};

Socket::Socket(int s){socket_ = s;}
Socket::Socket(const char* type){
    if(type=="TCP"){
    if((socket_ = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket setup error");
        exit(1);
    }
    }
    else{
        if(type=="UDP"){
            if((socket_ = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
            {
                perror("Socket setup error");
                exit(1);
            }
        }
    }
}

int Socket::Set_(int s){
    socket_ = s;
}

int Socket::Bind_(struct sockaddr_in addr){
    if(bind(socket_, (struct sockaddr*)&addr, sizeof(struct sockaddr))==-1){
        perror("Bind error");
        exit(1);
    }
    else{return(1);}
}

int Socket::Listen_(int port){
    if (listen(socket_, port) == -1){
        perror("Listen error");
        exit(1);
    }
}

int Socket::Accept_(struct sockaddr_in *addr_dist, socklen_t sin_size){
    int sock_dist_;
    if((sock_dist_ = accept(socket_, (struct sockaddr*)addr_dist, &sin_size)) == -1)
    {
        perror("Accept error");
    }
    return sock_dist_;
}

int Socket::Connect_(struct sockaddr_in addr_dist){
    if (connect(socket_, (struct sockaddr*)&addr_dist, sizeof(struct sockaddr)) == -1){
        perror("Connection failed");
        exit(1);
    }
}

int Socket::Recv_(char *msg){
    // char msg_[MAX_LEN];
    if(recv(socket_, msg, MAX_LEN ,0)==-1){
        perror("Recv");
    }
    // msg = msg_;
}

int Socket::Send_(const char* msg){
    if(send(socket_, msg, MAX_LEN, 0) == -1){
        perror("Send error");
    }
}

int Socket::Sendto_(const char *msg, struct sockaddr_in their_addr, int tolen){
    if(sendto(socket_, msg, MAX_LEN, 0, (struct sockaddr *)&their_addr, tolen)==-1){
        perror("Send error");
        return(-1);
    }
}

int Socket::Recvfrom_(char *msg, struct sockaddr_in *addr_their, socklen_t addr_len){
    if(recvfrom(socket_, msg, MAX_LEN, 0, (struct sockaddr *)addr_their, &addr_len)==-1){
        perror("Send error");
        return(-1);
    }
}

int Socket::Close_(void){
    close(socket_);
    return(1);
}

int Socket::Getsockname_(void){
    int result;
    struct sockaddr_in addr;
    socklen_t len = sizeof(struct sockaddr_in);
    result = getsockname(socket_, (sockaddr*)&addr, &len);
    if(result == -1){
        perror("getsockname");
        exit(1);
    }
    return result;
}
int set_addr_in(struct sockaddr_in& p, int family, int port, in_addr_t addr);

int set_random_port(){
    int result;
    class Socket cache("TCP");
    struct sockaddr_in addr;
    set_addr_in(addr, AF_INET, 1, htonl(INADDR_ANY));
    addr.sin_port = 0;
    cache.Bind_(addr);
    result = cache.Getsockname_();
}

int set_addr_in(struct sockaddr_in& p, int family, int port, in_addr_t addr){
    if(port == 0){
        // random port
        port = set_random_port();
    }
    p.sin_family = family;
    p.sin_port = port;
    p.sin_addr.s_addr = addr;
    bzero(&(p.sin_zero),sizeof(p.sin_zero));
}

int decode_msg(const char msg[], char* buf[]){
    char a[MAX_LEN];
    memset(a, 0, sizeof(a));
    strcpy(a, msg);
    // std::cout<<msg<<std::endl<<a<<std::endl;
    char *p = strtok(a, "&");
    int i = 0;
    strcpy(buf[i], p);
    i++;
    while(i<5){
        p = strtok(NULL, "&");
        strcpy(buf[i], p);
        i++;
    }
}

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