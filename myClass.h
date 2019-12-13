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

#define PORT_SA_UDP 21759
#define PORT_SB_UDP 22759
#define PORT_AWS_UDP 23759
#define PORT_CL_TCP 24759
#define PORT_MO_TCP 25759

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