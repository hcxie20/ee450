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
        return (1);
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