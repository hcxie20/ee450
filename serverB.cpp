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
#include"cmath"
#include"myClass.h"

#define PORT_SA_UDP 21759
#define PORT_SB_UDP 22759
#define PORT_AWS_UDP 23759
#define PORT_CL_TCP 24759
#define PORT_MO_TCP 25759

#define LOCAL_HOST "127.0.0.1"

#define MAX_LEN 100
/*
Args:
    char arg0: x
    char arg1: x
    char arg2: File size, bit
    char arg3: Signal power, db10
    char arg4: x
    char arg5: x
    char arg6: Bandwidth, MHz
    char arg7: Link length, Km
    char arg8: Speed, km/s
    char arg9: Noise power, db10
*/

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
    while(i<10){
        p = strtok(NULL, "&");
        strcpy(buf[i], p);
        i++;
    }
}

float char_to_float(char *c){
    using namespace std;
    float f;
    stringstream ss;
    ss << c;
    ss >> f;
    return f;
}

float float2(float x);
float float_to_char(float f, char *c){
    using namespace std; 
    f = float2(f);
    stringstream ss;
    ss << f;
    ss >> c;
}

float float2(float x){
    float a = round(100*x);
    float b = a/100;
    return b;
}

int main(void)
{
    std::cout<<"The server B is up and running using UDP on port "<<PORT_SB_UDP<<".\n";

    char arg0[20]="";
    char arg1[20]="";
    char arg2[20]="";
    char arg3[20]="";
    char arg4[20]="";
    char arg5[20]="";
    char arg6[20]="";
    char arg7[20]="";
    char arg8[20]="";
    char arg9[20]="";
    char* msg_decode[10];
    msg_decode[0] = arg0;
    msg_decode[1] = arg1;
    msg_decode[2] = arg2;
    msg_decode[3] = arg3;
    msg_decode[4] = arg4;
    msg_decode[5] = arg5;
    msg_decode[6] = arg6;
    msg_decode[7] = arg7;
    msg_decode[8] = arg8;
    msg_decode[9] = arg9;

    socklen_t sin_size = sizeof(struct sockaddr_in);
    socklen_t addr_len = sizeof(struct sockaddr);

    struct sockaddr_in addr_my;
    set_addr_in(addr_my, AF_INET, htons(PORT_SB_UDP), INADDR_ANY);

    struct sockaddr_in addr_their;

    class Socket socket_aws("UDP");
    socket_aws.Bind_(addr_my);

    char msg_recv[MAX_LEN];


    float d_prop;
    float d_tran;
    float d_e2e;

    char d_prop_c[10];
    char d_tran_c[10];
    char d_e2e_c[10];

    while(1){
        memset(msg_recv, 0, MAX_LEN);
        memset(d_prop_c, 0, sizeof(d_prop_c));
        memset(d_tran_c, 0, sizeof(d_tran_c));
        memset(d_e2e_c, 0, sizeof(d_e2e_c));

        // receive
        socket_aws.Recvfrom_(msg_recv, &addr_their, addr_len);
        decode_msg(msg_recv, msg_decode);
        std::cout<<"The Server B received link information: link <"<<msg_decode[1]<<">, file size <"<<msg_decode[2]<<">, and signal power <"<<msg_decode[3]<<">\n";

        // calculate values
        d_prop = 1000*char_to_float(msg_decode[7])/char_to_float((msg_decode[8]));
        d_tran = 1000*char_to_float(msg_decode[2])/(char_to_float(msg_decode[6])*pow(10, 6)*log(1 + pow(10, (char_to_float(msg_decode[3])-char_to_float(msg_decode[9]))/10)));
        // from sec to msec
        d_e2e = float2(d_prop+d_tran);
        d_prop = float2(d_prop);
        d_tran = float2(d_tran);
        // change from float to char
        float_to_char(d_prop, d_prop_c);
        float_to_char(d_tran, d_tran_c);
        float_to_char(d_e2e, d_e2e_c);
        std::cout<<"The server B finished the calculation for link <"<<msg_decode[1]<<">\n";

        // packet message for aws
        memset(msg_recv, 0, MAX_LEN);
        strcat(msg_recv, d_prop_c);
        strcat(msg_recv, "&");
        strcat(msg_recv, d_tran_c);
        strcat(msg_recv, "&");
        strcat(msg_recv, d_e2e_c);
        strcat(msg_recv, "&");
        strcat(msg_recv, "0&0&");
        socket_aws.Sendto_(msg_recv, addr_their, sin_size);
        std::cout<<"The server B finished sending the output to AWS\n\n";
    }




    return 0;
}