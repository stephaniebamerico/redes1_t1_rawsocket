#ifndef __MENSAGEM__
#define __MENSAGEM__

#define TAM_MSG 64
#define TAM_SEQUENCIA 32
#define TRATADO 4

#define ACK 0
#define CD_LOCAL 1
#define TAMANHO 2
#define OK 3
#define CD 6
#define LS 7
#define GET 8
#define PUT 9
#define FIM 0x000A
#define IMPRIMA 0x000C
#define ERRO 0x000E
#define NACK 0x000F

#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <errno.h>

using namespace std;

typedef struct {
    char inicio;
    unsigned int tamanho : 5;
    unsigned int sequencia : 6;
    unsigned int tipo : 5;
    char *dados;
    char paridade;
} mensagem_t;

void aloca_mensagem(mensagem_t **msg);

void libera_mensagem(mensagem_t *msg);

void copia_mensagem(mensagem_t *m1, mensagem_t **m2);

void aloca_str(char **str, int tam);

void cstr_tam_seq_tipo_dados(mensagem_t msg, char *cstr, int pos_inicial);

char* msg_to_cstr(mensagem_t *msg, char* cstr);

mensagem_t* cstr_to_msg(char *cstr, mensagem_t *msg);

mensagem_t* monta_mensagem(int tipo, int sequencia, char *args);

char calcula_paridade(mensagem_t msg);

void imprime_mensagem(mensagem_t msg);

void trata_erros(int tipo, char parametro);

void cd_remoto(int socket, char *args);

void ls_remoto(int socket, char *args);

#endif