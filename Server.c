#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
pthread_mutex_t lock;

#define ADDRESS "0.0.0.0"
#define PORT 8080
#define CONCURRENT_CONNECTION 200
#define QUEUE_CONNECTION 100
#define BUFFER_SIZE 65356
#define THREAD_STACK_SIZE 524288

pthread_mutex_t connection_mutex = PTHREAD_MUTEX_INITIALIZER;

int connection = 0;

char *copystring(char *dest, char *orig, int qtd)
{
    int i;
    for(i=0 ; i < qtd ; i++)
        dest[i] = orig[i];

    dest[i]='\0';

    return dest;
}


void *thread_func(void *sock_fd) {
    int conn_id = *((int *)sock_fd);
	free(sock_fd);
	
	clock_t start, end;
	start = clock();
	
    char buffer[BUFFER_SIZE] = {0};
    
    char resp[BUFFER_SIZE] = {0};

    while (recv(conn_id, buffer, BUFFER_SIZE, 0) > 0) {
    	printf("[RECEIVED] %s\n", buffer);
        pthread_mutex_lock(&lock);
        if(strstr(buffer, "addcar") != NULL) {
            MPI_Send(buffer, BUFFER_SIZE, MPI_UNSIGNED_CHAR, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(&resp, BUFFER_SIZE, MPI_UNSIGNED_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
        } else if(strstr(buffer, "compra") != NULL) {
            MPI_Send(buffer, BUFFER_SIZE, MPI_UNSIGNED_CHAR, 2, 0, MPI_COMM_WORLD);
            MPI_Recv(&resp, BUFFER_SIZE, MPI_UNSIGNED_CHAR, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else if(strstr(buffer, "cadastro") != NULL) {
            MPI_Send(buffer, BUFFER_SIZE, MPI_UNSIGNED_CHAR, 3, 0, MPI_COMM_WORLD);
            strcat(resp,"Item cadastrado\n");
        } else if(strstr(buffer, "veritem") != NULL) {
            MPI_Send(buffer, BUFFER_SIZE, MPI_UNSIGNED_CHAR, 4, 0, MPI_COMM_WORLD);
            MPI_Recv(&resp, BUFFER_SIZE, MPI_UNSIGNED_CHAR, 4, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else if(strstr(buffer, "criapedido") != NULL) {
            MPI_Send(buffer, BUFFER_SIZE, MPI_UNSIGNED_CHAR, 5, 0, MPI_COMM_WORLD);
            MPI_Recv(&resp, BUFFER_SIZE, MPI_UNSIGNED_CHAR, 5, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else if(strstr(buffer, "verpedidos") != NULL) {
            MPI_Send(buffer, BUFFER_SIZE, MPI_UNSIGNED_CHAR, 6, 0, MPI_COMM_WORLD);
            MPI_Recv(&resp, BUFFER_SIZE, MPI_UNSIGNED_CHAR, 6, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else if(strstr(buffer, "vercarrinho") != NULL) {
            MPI_Send(buffer, BUFFER_SIZE, MPI_UNSIGNED_CHAR, 7, 0, MPI_COMM_WORLD);
            MPI_Recv(&resp, BUFFER_SIZE, MPI_UNSIGNED_CHAR, 7, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        pthread_mutex_unlock(&lock);
        memset(buffer, 0, BUFFER_SIZE);

        if (send(conn_id, resp, strlen(resp), 0) > 0) {
            printf("[SEND] %s\n", resp);
        } else {
            printf("[WARNING][SEND] %s\n", strerror(errno));
        }
    }

    close(conn_id);
    
    printf("[INFO] CONNECTION CLOSED\n");
    
    pthread_mutex_lock(&connection_mutex);
    connection--;
    pthread_mutex_unlock(&connection_mutex);
   
    printf("[INFO] THREAD TERMINATED\n");
    
    end = clock();
    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;
    printf("[TIME] PROCESS COMPLETE IN %.5f SEC\n", time_taken);
    printf("------------------------\n");
    
    pthread_exit(NULL);
}

int main(int argc, char** argv) {
    // Initialize the MPI environment
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    if (provided != MPI_THREAD_MULTIPLE) {
        fprintf(stderr, "Warning MPI did not provide MPI_THREAD_MULTIPLE\n");
    }

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Print off a hello world message
    // printf("Hello world from processor %s, rank %d out of %d processors\n",
    //        processor_name, world_rank, world_size);
    
    if (pthread_mutex_init(&lock, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 
    if(world_rank == 0) {
        pthread_t thread_id;
        
        pthread_attr_t attr;
        
        if (pthread_attr_init(&attr) != 0) {
            printf("[ERROR][THREAD][INIT] %s\n", strerror(errno));
            return 1;
        }
        
        if (pthread_attr_setstacksize(&attr, THREAD_STACK_SIZE) != 0) {
            printf("[ERROR][THREAD][STACK] %s\n", strerror(errno));
            return 1;
        }
        
        if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0) {
            printf("[ERROR][THREAD][DETACH] %s\n", strerror(errno));
            return 1;
        }
        
        int master_socket, conn_id;
        struct sockaddr_in server, client;
        
        memset(&server, 0, sizeof(server));
        memset(&client, 0, sizeof(client));
        
        signal(SIGPIPE, SIG_IGN);
        
        if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            printf("[ERROR] CAN'T CREATE TO SOCKET\n");
            return 1;
        } else {
            printf("[NOTE] SOCKET CREATED DONE\n");
        }
        
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = inet_addr(ADDRESS);
        server.sin_port = htons(PORT);
        
        socklen_t addrlen = sizeof(struct sockaddr_in);
        
        if (bind(master_socket, (struct sockaddr *)&server, sizeof(server)) == -1) {
            printf("[ERROR][BIND] %s\n", strerror(errno));
            return 1;
        } else {
            printf("[NOTE] BIND %s:%d\n", ADDRESS, PORT);
        }
        
        if (listen(master_socket, QUEUE_CONNECTION) == -1) {
            printf("[ERROR][LISTEN] %s\n", strerror(errno));
            return 1;
        } else {
            printf("[INFO] WAITING FOR INCOMING CONNECTIONS\n");
        }
        
        while (1) {
            conn_id = accept(master_socket, (struct sockaddr *)&client, (socklen_t *)&addrlen);
            
            if (conn_id == -1) {
                printf("[WARNING] CAN'T ACCEPT NEW CONNECTION\n");
            } else {
                if (connection >= CONCURRENT_CONNECTION) {
                    printf("[WARNING] CONNECTION LIMIT REACHED\n");
                    close(conn_id);
                } else {
                    int *conn_id_heap = (int*)malloc(sizeof(int));
                    
                    if (conn_id_heap == NULL) {
                        perror("[ERROR] Memory allocation failed");
                        close(conn_id);
                        continue;
                    }
                    
                    *conn_id_heap = conn_id;
                    
                    printf("[INFO] NEW CONNECTION ACCEPTED FROM %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
                    
                    if (pthread_create(&thread_id, &attr, thread_func, (void*)conn_id_heap) == -1) {
                        printf("[WARNING] CAN'T CREATE NEW THREAD\n");
                        close(conn_id);
                        free(conn_id_heap);
                    } else {
                        printf("[INFO] NEW THREAD CREATED\n");
                        connection++;
                    }
                }
            }
        }
        
        return 0;

    } else if(world_rank == 1){ // Add item no Carrinho de compra
        while(1){
            int i;
            char buffer[BUFFER_SIZE] = { 0 };
            char write[BUFFER_SIZE] = "";
            char usuario[11] = "XXXXXXXXXX\0";
            char nome[11] = "YYYYYYYYYY\0";
            char qtd[5] = "ZZZZ\0";
            char arq[50] = "Carrinho";
            char p[1] = ".";
            char resp[BUFFER_SIZE] = "Item adicionado ao carrinho!\n";
            MPI_Recv(&buffer, BUFFER_SIZE, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            copystring(usuario,buffer+6,10);
            copystring(nome,buffer+16,10);
            copystring(qtd,buffer+26,4);
            for (i = 0; i < strlen(usuario); i++) {
                if (usuario[i] != p[0]){
                    arq[i+8] = usuario[i];
                }
            }
            strcat(arq,".txt");
            printf("Arq: %s\n", arq);
            FILE *f = fopen(arq, "a");
            if (f == NULL) {
                printf("ERRO! O arquivo não foi aberto!\n");
            } else {
                fputs(nome, f);
                fputs(" ", f);
                fputs(qtd, f);
                fputs("\n", f);
            }
            fclose(f);
            MPI_Send(resp, BUFFER_SIZE, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD);
        }
    } else if(world_rank == 2){ // Compra
        while(1){
            char buffer[BUFFER_SIZE] = { 0 };
            int i;
            char nome[11] = "XXXXXXXXXX\0";
            char valor[4] = "YYY\0";
            char estoque[5] = "ZZZZ\0";
            char qtd[5] = "ZZZZ\0";
            int qtdI;
            int estoqueI;
            int sem = 1;
            char arq[20] = "";
            char p[1] = ".";
            char resp[BUFFER_SIZE] = { 0 };
            MPI_Recv(&buffer, BUFFER_SIZE, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            copystring(nome,buffer+6,10);
            copystring(qtd,buffer+16,4);
            sscanf(qtd, "%d", &qtdI);
            for (i = 0; i < strlen(nome); i++) {
                if (nome[i] != p[0]){
                    arq[i] = nome[i];
                }
            }
            strcat(arq,".txt");
            printf("Arq: %d\n", arq);
            FILE *f = fopen(arq, "r");
            if (f == NULL) {
                printf("ERRO! O arquivo não foi aberto!\n");
                strcat(resp,"Falha na requisicao!");
            } else {
                while (fscanf(f, "%s %s %d", nome, valor, &estoqueI) == 3) {
                    estoqueI -= qtdI;
                    if (estoqueI >= 1000){
                        sprintf(estoque, "%d", estoqueI);
                    } else if (estoqueI >= 100 && estoqueI < 1000) {
                        sprintf(estoque, "0%d", estoqueI);
                    } else if (estoqueI >= 10 && estoqueI < 100) {
                        sprintf(estoque, "00%d", estoqueI);
                    } else if (estoqueI >= 0 && estoqueI < 10) {
                        sprintf(estoque, "000%d", estoqueI);
                    }
                    if (estoqueI < 0) {
                        strcat(resp,"Item: ");
                        strcat(resp,nome);
                        strcat(resp," sem estoque disponível!\n");
                        sem = 0;
                    } else {
                        strcat(resp,"Item comprado: ");
                        strcat(resp,nome);
                        strcat(resp,"\tValor: ");
                        strcat(resp,valor);
                        strcat(resp,"\n");
                    }
                }
                printf("resp: %s", resp);
            }
            fclose(f);
            if (sem == 1){
                FILE *f2 = fopen(arq, "w");
                if (f2 == NULL) {
                    printf("ERRO! O arquivo não foi aberto!\n");
                } else {
                    fputs(nome, f);
                    fputs(" ", f);
                    fputs(valor, f);
                    fputs(" ", f);
                    fputs(estoque, f);
                    fputs("\n", f);
                }
                fclose(f2);
            }
            
            MPI_Send(resp, BUFFER_SIZE, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD);
        }
    } else if(world_rank == 3){ // Cadastro de item
        while(1){
            char buffer[BUFFER_SIZE] = { 0 };
            int i;
            char nome[11] = "XXXXXXXXXX\0";
            char valor[4] = "YYY\0";
            char estoque[5] = "ZZZZ\0";
            char arq[20] = "";
            char p[1] = ".";
            MPI_Recv(&buffer, BUFFER_SIZE, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // char *f = strstr(buffer, "cadastro");
            copystring(nome,buffer+8,10);
            copystring(valor,buffer+18,3);
            copystring(estoque,buffer+21,4);
            for (i = 0; i < strlen(nome); i++) {
                if (nome[i] != p[0]){
                    arq[i] = nome[i];
                }
            }
            strcat(arq,".txt\0");
            printf("Rank 3, nome: %s, valor: %s, estoque: %s, arq: %s\n", nome, valor, estoque, arq);
            FILE *f = fopen(arq, "w");
            if (f == NULL) {
                printf("ERRO! O arquivo não foi aberto!\n");
            } else {
                fputs(nome, f);
                fputs(" ", f);
                fputs(valor, f);
                fputs(" ", f);
                fputs(estoque, f);
                fputs("\n", f);
            }
            // printf("Rank 3, nome: %s\n", nome);
            fclose(f);
            FILE *f2 = fopen("itens.txt", "a");
            if (f2 == NULL) {
                printf("ERRO! O arquivo não foi aberto!\n");
            } else {
                fputs(arq, f2);
                fputs("\n", f2);
            }
            // printf("Rank 3, nome: %s\n", nome);
            fclose(f2);
        }
    } else if(world_rank == 4){ // Ver item
        while(1){
            char buffer[BUFFER_SIZE] = { 0 };
            int i;
            char nome[11] = "XXXXXXXXXX\0";
            char valor[4] = "YYY\0";
            char estoque[5] = "ZZZZ\0";
            // int valor;
            // int estoque;
            char resp[BUFFER_SIZE] = { 0 };
            char arq[20] = "";
            MPI_Recv(&buffer, BUFFER_SIZE, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // printf("Rank 3, nome: %s, valor: %s, estoque: %s\n", nome, valor, estoque);
            FILE *f = fopen("itens.txt", "r");
            if (f == NULL) {
                printf("ERRO! O arquivo não foi aberto!\n");
            } else {
                while (fscanf(f, "%s", arq) == 1) {
                    FILE *f2 = fopen(arq, "r");
                    if (f2 == NULL) {
                        printf("ERRO! O arquivo não foi aberto!\n");
                    } else {
                        while (fscanf(f2, "%s %s %s", nome, valor, estoque) == 3) {
                            strcat(resp,nome);
                            strcat(resp,",");
                            strcat(resp,valor);
                            strcat(resp,",");
                            strcat(resp,estoque);
                            strcat(resp,";");
                            printf("%s %s %s %s\n", nome, valor, estoque, arq);
                        }
                    }
                    // printf("Rank 3, nome: %s\n", nome);
                    fclose(f2);
                }
                strcat(resp,"\n");
                printf("Resp: \n%s", resp);
            }
            // printf("Rank 3, nome: %s\n", nome);
            fclose(f);
            MPI_Send(resp, BUFFER_SIZE, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD);
        }
    } else if(world_rank == 5){ // Cria pedido
        int numPedido = 1;
        while(1){
            int i;
            char buffer[BUFFER_SIZE] = { 0 };
            char write[BUFFER_SIZE] = "";
            char usuario[11] = "XXXXXXXXXX\0";
            char nome[11] = "YYYYYYYYYY\0";
            char qtd[5] = "ZZZZ\0";
            char arqcar[50] = "Carrinho";
            char arqped[50] = "";
            char p[1] = ".";
            char resp[BUFFER_SIZE] = "Pedido criado!\n";
            char erro[BUFFER_SIZE] = "Pedido não criado!\n";
            MPI_Recv(&buffer, BUFFER_SIZE, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            copystring(usuario,buffer+10,10);
            for (i = 0; i < strlen(usuario); i++) {
                if (usuario[i] != p[0]){
                    arqcar[i+8] = usuario[i];
                }
            }
            strcat(arqcar,".txt");
            printf("Arqcar: %s\n", arqcar);
            FILE *f = fopen(arqcar, "r");
            if (f == NULL) {
                printf("ERRO! O arquivo não foi aberto!\n");
                copystring(resp,erro,20);
            } else {
                while (fscanf(f, "%s %s", nome, qtd) == 2) {
                    strcat(write,usuario);
                    strcat(write," ");
                    strcat(write,nome);
                    strcat(write," ");
                    strcat(write,qtd);
                    strcat(write,"\n");
                }
                remove(arqcar);
            }
            fclose(f);
            sprintf(arqped, "Pedido%d.txt", numPedido);
            printf("Arqped: %s\n", arqped);
            FILE *f2 = fopen(arqped, "w");
            if (f2 == NULL || f == NULL) {
                printf("ERRO! O arquivo não foi aberto!\n");
                copystring(resp,erro,20);
            } else {
                fputs(write, f2);
            }
            fclose(f2);
            char numpedidos[100];
            sprintf(numpedidos, "%d", numPedido);
            FILE *f3 = fopen("numPedidos.txt", "w");
            if (f3 == NULL || f == NULL) {
                printf("ERRO! O arquivo não foi aberto!\n");
                copystring(resp,erro,20);
            } else {
                fputs(numpedidos, f3);
            }
            fclose(f3);
            MPI_Send(resp, BUFFER_SIZE, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD);
            numPedido++;
        }
    } else if(world_rank == 6){ // Visualiza pedidos
        while(1){
            int i;
            char buffer[BUFFER_SIZE] = { 0 };
            int numPedido;
            // char write[BUFFER_SIZE] = "";
            char usuario[11] = "XXXXXXXXXX\0";
            char nome[11] = "YYYYYYYYYY\0";
            char qtd[5] = "ZZZZ\0";
            // char arqcar[50] = "Carrinho";
            char arq[50] = "";
            // char p[1] = ".";
            char resp[BUFFER_SIZE] = "";
            MPI_Recv(&buffer, BUFFER_SIZE, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            FILE *f2 = fopen("numPedidos.txt", "r");
            if (f2 == NULL) {
                printf("ERRO! O arquivo não foi aberto!\n");
            } else { while (fscanf(f2, "%d", &numPedido) == 1) {} }
            fclose(f2);
            for (i = 1; i <= numPedido; i++) {
                sprintf(arq, "Pedido%d.txt", i);
                printf("Arqp: %s\n", arq);
                FILE *f = fopen(arq, "r");
                if (f == NULL) {
                    printf("ERRO! O arquivo não foi aberto!\n");
                } else {
                    char num[100];
                    sprintf(num, "%d", i);
                    strcat(resp,num);
                    strcat(resp,":");
                    while (fscanf(f, "%s %s %s", usuario, nome, qtd) == 3) {
                        strcat(resp,usuario);
                        strcat(resp,",");
                        strcat(resp,nome);
                        strcat(resp,",");
                        strcat(resp,qtd);
                        strcat(resp,"-");
                    }
                    strcat(resp,";");
                }
                fclose(f);
            }
            strcat(resp,"\n");
            MPI_Send(resp, BUFFER_SIZE, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD);
        }
    } else if(world_rank == 7){ // Visualiza carrinho
        while(1){
            int i;
            char buffer[BUFFER_SIZE] = { 0 };
            char write[BUFFER_SIZE] = "";
            char usuario[11] = "XXXXXXXXXX\0";
            char nome[11] = "YYYYYYYYYY\0";
            char qtd[5] = "ZZZZ\0";
            char arq[50] = "Carrinho";
            char p[1] = ".";
            char resp[BUFFER_SIZE] = "";
            MPI_Recv(&buffer, BUFFER_SIZE, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            copystring(usuario,buffer+11,10);
            for (i = 0; i < strlen(usuario); i++) {
                if (usuario[i] != p[0]){
                    arq[i+8] = usuario[i];
                }
            }
            printf("Arq: %s\n", arq);
            strcat(arq,".txt");
            FILE *f = fopen(arq, "r");
            if (f == NULL) {
                printf("ERRO! O arquivo não foi aberto!\n");
                strcat(resp,"Carrinho não existe, adicione algum item antes de comprar!\n");
            } else {
                strcat(resp,usuario);
                strcat(resp,":");
                while (fscanf(f, "%s %s", nome, qtd) == 2) {
                    strcat(resp,nome);
                    strcat(resp,",");
                    strcat(resp,qtd);
                    strcat(resp,";");
                    printf("%s %s %s\n", nome, qtd, arq);
                }
            }
            strcat(resp,"\n");
            fclose(f);
            MPI_Send(resp, BUFFER_SIZE, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD);
        }
    } else {
        printf("BBBBBB\n");
    }

    pthread_mutex_destroy(&lock); 
    // Finalize the MPI environment.
    MPI_Finalize();
}