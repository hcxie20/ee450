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

int decode_msg10(const char msg[], char* buf[]){
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
        decode_msg10(msg_recv, msg_decode);
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
