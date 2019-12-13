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


int main(void)
{
    std::cout<<"The AWS is up and running.\n";

    char arg0[20]="";
    char arg1[20]="";
    char arg2[20]="";
    char arg3[20]="";
    char arg4[20]="";
    char* msg_decode[5];
    msg_decode[0] = arg0;
    msg_decode[1] = arg1;
    msg_decode[2] = arg2;
    msg_decode[3] = arg3;
    msg_decode[4] = arg4;

    socklen_t sin_size = sizeof(struct sockaddr_in);
    socklen_t addr_len = sizeof(struct sockaddr);

    // addresses for every software
    struct sockaddr_in addr_my_TCP_client;
    set_addr_in(addr_my_TCP_client, AF_INET, htons(PORT_CL_TCP), INADDR_ANY);

    struct sockaddr_in addr_my_UDP;
    set_addr_in(addr_my_UDP, AF_INET, htons(PORT_AWS_UDP), INADDR_ANY);

    struct sockaddr_in addr_serverA;
    set_addr_in(addr_serverA, AF_INET, htons(PORT_SA_UDP), inet_addr(LOCAL_HOST));

    struct sockaddr_in addr_serverB;
    set_addr_in(addr_serverB, AF_INET, htons(PORT_SB_UDP), inet_addr(LOCAL_HOST));
    
    struct sockaddr_in addr_my_TCP_monitor;
    set_addr_in(addr_my_TCP_monitor, AF_INET, htons(PORT_MO_TCP), INADDR_ANY);

    struct sockaddr_in addr_client;
    struct sockaddr_in addr_monitor;


    // set up sockets
    class Socket socket_client("TCP");
    socket_client.Bind_(addr_my_TCP_client);
    socket_client.Listen_(PORT_CL_TCP);

    class Socket socket_monitor("TCP");
    socket_monitor.Bind_(addr_my_TCP_monitor);
    socket_monitor.Listen_(PORT_MO_TCP);

    class Socket socket_client_child(0);
    class Socket socket_monitor_child(0);

    // connect to the monitor
    socket_monitor_child.Set_(socket_monitor.Accept_(&addr_monitor, sin_size));

    char msg_recv[MAX_LEN];

    char msg_monitor[1000];

    char msg_serverB[MAX_LEN];

    char operation[20];
    char link_id[20];
    char length[20];
    char sig[20];

    while(1)
    {
        // clear args
        memset(arg0, 0, sizeof(arg0)); 
        memset(arg1, 0, sizeof(arg1));
        memset(arg2, 0, sizeof(arg2));
        memset(arg3, 0, sizeof(arg3));
        memset(arg4, 0, sizeof(arg4));  
        memset(msg_recv, 0, sizeof(msg_recv));

        // wait to connection from client
        socket_client_child.Set_(socket_client.Accept_(&addr_client, sin_size));
        socket_client_child.Recv_(msg_recv);
 
        // find each args from client
        decode_msg(msg_recv, msg_decode);

        std::cout<<"The AWS received operation <"<<msg_decode[0]<<"> from the client using TCP over port <"<<PORT_CL_TCP<<">\n";

        if(strcmp(msg_decode[0], "write")==0)
        {
            // wrtie request

            // send to monitor
            // set monitor message
            memset(msg_monitor, 0, sizeof(msg_monitor));
            strcat(msg_monitor, "The monitor received BW = <");
            strcat(msg_monitor, msg_decode[1]);
            strcat(msg_monitor, ">, L = <");
            strcat(msg_monitor, msg_decode[2]);
            strcat(msg_monitor, ">, V = <");
            strcat(msg_monitor, msg_decode[3]);
            strcat(msg_monitor, "> and P = <");
            strcat(msg_monitor, msg_decode[4]);
            strcat(msg_monitor, "> from the AWS");

            socket_monitor_child.Send_(msg_monitor);
            std::cout<<"The AWS sent operation <"<<msg_decode[0]<<"> and arguments to the monitor using TCP over port <"<<PORT_MO_TCP<<">\n";

            // send to server A
            class Socket socket_server_a("UDP");
            socket_server_a.Sendto_(msg_recv, addr_serverA, addr_len);
            std::cout<<"The AWS sent operation <"<<msg_decode[0]<<"> to Backend-Server A using UDP over port <"<<PORT_AWS_UDP<<">\n";

            // receive from server A
            socket_server_a.Recvfrom_(msg_recv, &addr_my_UDP, addr_len);
            std::cout<<"The AWS received response from Backend-Server A for writing using UDP over port <"<<PORT_AWS_UDP<<">\n";

            socket_server_a.Close_();

            
            // send to client
            if(strcmp(msg_recv, "1")==0){
                socket_client_child.Send_(msg_recv);
                std::cout<<"The AWS sent result to client for operation <"<<msg_decode[0]<<"> using TCP over port <"<<PORT_CL_TCP<<">\n";  
            }
            else{
                 socket_client_child.Send_(msg_recv); 
            }

            // send to monitor
            // set monitor message
            memset(msg_monitor, 0, sizeof(msg_monitor));
            strcat(msg_monitor, "The write operation has been completed successfully");

            socket_monitor_child.Send_(msg_monitor);
            std::cout<<"The AWS sent write response to the monitor using TCP over port <"<<PORT_MO_TCP<<">\n\n";  
        }
        else{
            if(strcmp(msg_decode[0], "compute")==0){
                // compute command

                memset(msg_serverB, 0, sizeof(msg_serverB));
                strcpy(msg_serverB, msg_recv);

                // send to monitor
                // set monitor message
                memset(msg_monitor, 0, sizeof(msg_monitor));
                strcat(msg_monitor, "The monitor received link ID = <");
                strcat(msg_monitor, msg_decode[1]);
                strcat(msg_monitor, ">, size = <");
                strcat(msg_monitor, msg_decode[2]);
                strcat(msg_monitor, ">, power = <");
                strcat(msg_monitor, msg_decode[3]);
                strcat(msg_monitor, "> from the AWS");

                socket_monitor_child.Send_(msg_monitor);
                std::cout<<"The AWS sent operation <"<<msg_decode[0]<<"> and arguments to the monitor using TCP over port <"<<PORT_MO_TCP<<">\n";

                // send to server A
                class Socket socket_server_a("UDP");
                socket_server_a.Sendto_(msg_recv, addr_serverA, addr_len);
                std::cout<<"The AWS sent operation <"<<msg_decode[0]<<"> to Backend-Server A using UDP over port <"<<PORT_AWS_UDP<<">\n";

                // receive from server A
                socket_server_a.Recvfrom_(msg_recv, &addr_my_UDP, addr_len);
                std::cout<<"The AWS received response from Bacnend-Server A for writing using UDP over port <"<<PORT_AWS_UDP<<">\n";

                socket_server_a.Close_();
                
                // decode msg from server A
                strcpy(operation, msg_decode[0]);
                strcpy(link_id, msg_decode[1]);
                strcpy(length, msg_decode[2]);
                strcpy(sig, msg_decode[3]);
                memset(arg0, 0, sizeof(arg0)); 
                memset(arg1, 0, sizeof(arg1));
                memset(arg2, 0, sizeof(arg2));
                memset(arg3, 0, sizeof(arg3));
                memset(arg4, 0, sizeof(arg4));
                decode_msg(msg_recv, msg_decode);

                
                if(strcmp(msg_decode[0], "0")==0){
                    // Link not found

                    socket_client_child.Send_("Link ID not found");
                    std::cout<<"Link ID not found\n\n";
                    // send to monitor
                    socket_monitor_child.Send_("Link ID not found");
                }
                else{
                    // Link found

                    // send to server B
                    class Socket socket_server_b("UDP");
                    strcat(msg_serverB, msg_recv);
                    socket_server_a.Sendto_(msg_serverB, addr_serverB, addr_len);
                    std::cout<<"The AWS sent link ID = <"<<link_id<<">, size = <"<<length<<">, power = <"<<sig<<">, and link infomation to Backend_Server B using UDP over port <"<<PORT_AWS_UDP<<">\n";

                    // receive from server B
                    socket_server_b.Recvfrom_(msg_recv, &addr_my_UDP, addr_len);
                    std::cout<<"The AWS received outputs from Backend_Server B using UDP over port <"<<PORT_AWS_UDP<<">\n";

                    socket_server_a.Close_();

                    memset(arg0, 0, sizeof(arg0)); 
                    memset(arg1, 0, sizeof(arg1));
                    memset(arg2, 0, sizeof(arg2));
                    memset(arg3, 0, sizeof(arg3));
                    memset(arg4, 0, sizeof(arg4));  
                    decode_msg(msg_recv, msg_decode);
                    
                    // send to client
                    memset(msg_recv, 0, sizeof(msg_recv));
                    strcat(msg_recv, "The delay for link <");
                    strcat(msg_recv, link_id);
                    strcat(msg_recv, "> is <");
                    strcat(msg_recv, msg_decode[2]);
                    strcat(msg_recv, "> ms");
                    socket_client_child.Send_(msg_recv);
                    std::cout<<"The AWS sent result to client for operation <"<<operation<<"> using port <"<<PORT_CL_TCP<<">\n";  


                    // send to monitor
                    // set monitor message
                    memset(msg_monitor, 0, sizeof(msg_monitor));
                    strcat(msg_monitor, "The result for link <");
                    strcat(msg_monitor, link_id);
                    strcat(msg_monitor, ">:\nTt = <");
                    strcat(msg_monitor, msg_decode[1]);
                    strcat(msg_monitor, "> ms\nTp = <");
                    strcat(msg_monitor, msg_decode[0]);
                    strcat(msg_monitor, "> ms\nDelay = <");
                    strcat(msg_monitor, msg_decode[2]);
                    strcat(msg_monitor, "> ms");

                    socket_monitor_child.Send_(msg_monitor);

                    std::cout<<"The AWS sent compute results to the monitor using TCP over port <"<<PORT_MO_TCP<<">\n\n";
                }
            }
        socket_client_child.Close_();
        }
    }
    return 0;
}