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

int write_link(char* args[]){
    // args:5 args
    using namespace std;
    // set link id
    int link_id_int=0;
    char link_id[10] = "0";
    fstream f;
    f.open("./database.txt", ios::app);
    f.close();
    f.open("./database.txt", ios::in);
    if(f.is_open()){
        char text[100];
        while(!f.eof()){
            memset(text, 0, sizeof(text));
            f.getline(text, 100);
            if(strcmp(text, "")!=0){ 
                // get link id
                char cache_[100];
                memset(cache_, 0, sizeof(cache_));
                strcpy(cache_, text);
                char* p = strtok(cache_, "&");
                memset(link_id, 0, sizeof(link_id));
                strcpy(link_id, p);
            }
        }
    }
    else{
        return 0;
    }
    f.close();
    stringstream s1, s2;
    s1 << link_id;
    s1 >> link_id_int;
    link_id_int += 1;
    s2 << link_id_int;
    s2 >> link_id;

    // write link
    char msg[MAX_LEN];
    memset(msg, 0, sizeof(msg));
    strcat(msg, link_id);
    strcat(msg, "&");
    for(int i=1;i<5;i++)
    {
        strcat(msg, args[i]);
        strcat(msg, "&");
    }
    strcat(msg, "\n");

    f.open("./database.txt", ios::app);
    f<<msg;
    f.close();
    return link_id_int;
    // useless code, for another function, not required
    // fstream f("./links.txt");
    // if(f.is_open())
    // {
    //     fstream f_cache("./cache.txt", ios::out);
    //     char text[100];
    //     while(!f.eof())
    //     {            
    //         memset(text, 0, sizeof(text));
    //         f.getline(text, 100);
    //         if(strcmp(text, "")!=0){
    //             char cache_[100];
    //             memset(cache_, 0, sizeof(cache_));
    //             strcpy(cache_, text);
    //             char* p = strtok(cache_, ",");
    //             char cache[10];
    //             memset(cache, 0, sizeof(cache));
    //             strcpy(cache, p);
    //             if(atoi(cache)>atoi(args[0])){
    //                 f_cache<<msg;
    //             }
    //             f_cache<<text<<endl;
    //         }
    //     }
    //     f_cache.close();
    //     f_cache.open("./cache.txt");
    //     f.close();
    //     f.open("./links.txt", ios::out);
    //     while(!f_cache.eof()){
    //         memset(text, 0, sizeof(text));
    //         f_cache.getline(text, 100);
    //         if(strcmp(text, "")!=0){
    //             f<<text<<endl;
    //         }
    //     }
    //     f_cache.close();
    //     f.close();
    //     remove("./cache.txt");
    //     return(1);
    // }
}

int search_link(char link_id[], char* buf){
    using namespace std;
    fstream f("./database.txt");
    char text[100];
    if(f.is_open()){
        while(!f.eof()){
            memset(text, 0, sizeof(text));
            f.getline(text, 100);
            if(strcmp(text, "")!=0){ 
                // get link id
                char cache_[100];
                memset(cache_, 0, sizeof(cache_));
                strcpy(cache_, text);
                char* p = strtok(cache_, "&");
                if(strcmp(p, link_id)==0){
                    strcpy(buf, text);
                }
            }
        }
    }
    else{return 0;}
}

int main(void)
{
    std::cout<<"The server A is up and running using UDP on port "<<PORT_SA_UDP<<".\n";

    char arg0[20]="write";
    char arg1[20]="1";
    char arg2[20]="2";
    char arg3[20]="3";
    char arg4[20]="4";
    char* msg_decode[5];
    msg_decode[0] = arg0;
    msg_decode[1] = arg1;
    msg_decode[2] = arg2;
    msg_decode[3] = arg3;
    msg_decode[4] = arg4;

    // set up sockets and addresses
    socklen_t sin_size = sizeof(struct sockaddr_in);
    socklen_t addr_len = sizeof(struct sockaddr);

    struct sockaddr_in addr_my;
    set_addr_in(addr_my, AF_INET, htons(PORT_SA_UDP), INADDR_ANY);

    struct sockaddr_in addr_their;

    class Socket socket_aws("UDP");
    socket_aws.Bind_(addr_my);
    char msg_recv[MAX_LEN];
    memset(msg_recv, 0, MAX_LEN);

    while(1){
        // clear args
        memset(arg0, 0, sizeof(arg0)); 
        memset(arg1, 0, sizeof(arg1));
        memset(arg2, 0, sizeof(arg2));
        memset(arg3, 0, sizeof(arg3));
        memset(arg4, 0, sizeof(arg4));  
        memset(msg_recv, 0, MAX_LEN);

        //start receive
        socket_aws.Recvfrom_(msg_recv, &addr_their, addr_len);

        decode_msg(msg_recv, msg_decode);
        if(strcmp(msg_decode[0], "write")==0){
            //write
            std::cout<<"Server A received input for writing"<<std::endl;
            int link_id = write_link(msg_decode);
            std::cout<<"The Server A wrote link <"<<link_id<<"> to database\n\n";
            socket_aws.Sendto_("1", addr_their, addr_len);
        }
        if(strcmp(msg_decode[0], "compute")==0){
            // search
            std::cout<<"Server A received input <"<<msg_decode[1]<<"> for computering"<<std::endl;
            memset(msg_recv, 0, sizeof(msg_recv));
            strcpy(msg_recv, "0&0&0&0&0&");
            if(search_link(msg_decode[1], msg_recv)==0){
                // link not found, first 0
                socket_aws.Sendto_("0&1&2&3&4&", addr_their, addr_len);
                std::cout<<"Link ID not found\n\n";
            }
            else{
                // link found
                socket_aws.Sendto_(msg_recv, addr_their, addr_len);
                std::cout<<"The Server A finished sending the search result to AWS\n\n";
            }
        }

    }

    return 0;
}