#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#define BUF_SIZE 100
#define NAME_SIZE 20
    
void *send_msg(void *arg);
void *recv_msg(void *arg);
void error_handling(char *msg);
    
char name[NAME_SIZE] = "[DEFAULT]";
char msg[BUF_SIZE];
char new_connect[BUF_SIZE];
char bye_disconnect[BUF_SIZE];
char client_array_name[BUF_SIZE]; // 접속자 ID 넣을 배열
int array_count = 0;
    
int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in serv_addr;
    pthread_t snd_thread, rcv_thread;
    void *thread_return;
    
    if(argc!=4) {
        printf("Usage : %s <IP> <port> <name>\n", argv[0]);
        exit(1);
     }
    
    sprintf(name, "[%s]", argv[3]);
    sock = socket(PF_INET, SOCK_STREAM, 0);
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));
      
    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error");

    write(sock, argv[3], strlen(argv[3])); // client to server
    
    pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);
    pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);
    pthread_join(snd_thread, &thread_return);
    pthread_join(rcv_thread, &thread_return);

    
    close(sock);
    return 0;
}
    
void *send_msg(void *arg) {
    int sock = *((int*)arg);
    char name_msg[NAME_SIZE + BUF_SIZE];

    // new connect msg
    sprintf(new_connect, "%s 님이 새로 접속하셨습니다!\n", name);
    write(sock, new_connect, strlen(new_connect));
   
    // now connected ID list
    read(sock, client_array_name, strlen(client_array_name));
    printf("현재 접속자 목록입니다 %s\n", client_array_name);
    
    while(1) {
        fgets(msg, BUF_SIZE, stdin);
    if(!strcmp(msg, "@\n")) {
        printf("now connected ID %s\n", client_array_name);
    }
        else if(!strcmp(msg,"q\n") || !strcmp(msg,"Q\n")) {
            sprintf(bye_disconnect, "%s 님의 연결이 끊어졌습니다.\n", name);
            write(sock, bye_disconnect, strlen(bye_disconnect));
            close(sock);
            exit(0);
        }
        sprintf(name_msg,"%s %s", name, msg);
        write(sock, name_msg, strlen(name_msg));
    }
    return NULL;
}
    
void *recv_msg(void *arg) {
    int sock = *((int*)arg);
    char name_msg[NAME_SIZE + BUF_SIZE];
    int str_len;
    
    while(1) {
        str_len = read(sock, name_msg, NAME_SIZE+BUF_SIZE-1);
        if(str_len == -1)
            return (void*) - 1;
        name_msg[str_len] = 0;
        fputs(name_msg, stdout);
    }
    return NULL;
}
    
void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}
