#include "raw_socket.h"
#include "mensagem.h"
#include "operations.h"

int main(int argc, char const *argv[]) {
    int socket = openRawSocket((char *) argv[1]);

    if(socket < 0) {
        cerr << "Erro ao abrir Raw Socket." << endl;
        return 1;
    }

    inicia_socket(socket);
    int *lsArgs = (int*)malloc (3*sizeof(int));
    cout << "Aguardando mensagens." << endl;
    mensagem_t *msg_recebida = NULL;
    aloca_mensagem(&msg_recebida);
    while (1) {        
        if(msg_recebida) {
            if(msg_recebida->dados)
                memset(msg_recebida->dados, 0, msg_recebida->tamanho);
            memset(msg_recebida, 0, sizeof(mensagem_t));
        }
        
        if(recebe_mensagem(socket, msg_recebida)) {
            //DEBUG
            /*cout << endl << "Mensagem recebida: " << endl;
            imprime_mensagem(*msg_recebida);*/

            envia_confirmacao(socket, msg_recebida->sequencia, ACK);
/*==================================================================================================*/
            if (msg_recebida->tipo == CD) {
                errno = 0;
                changeDir (msg_recebida->dados);

                mensagem_t *msg_resposta;
                if (errno == 0) {
                    cout << "CD: ";
                    system("pwd");
                    cout << endl;
                    msg_resposta = monta_mensagem(OK, 0, NULL);
                }
                else {
                    char erro[1];
                    if (errno == EACCES) {
                        cout <<"CD: erro de acesso" << endl;
                        erro[0] = 2;
                        msg_resposta = monta_mensagem(ERRO, 0, erro);
                    }
                    else {
                        cout << "CD: Arquivo Inexistente" << endl;
                        erro[0] = 2;
                        msg_resposta = monta_mensagem(ERRO, 0, erro);
                    }
                }
                 envia_mensagem(socket, &msg_resposta, 1);
            }
/*==================================================================================================*/
            else if (msg_recebida->tipo == LS) {
                errno = 0;

                lsArgs = testOptions(msg_recebida->dados);
                
                mensagem_t *msg_resposta;
                if (errno == 0) {
                    string lsSaida = list(lsArgs[1],lsArgs[0]);
                    if (errno == 0) {
                        char * ls;
                        aloca_str (&ls, lsSaida.size());
                        strcpy(ls, lsSaida.c_str());
                        char_to_msg(socket, ls,lsSaida.size() );

                        cout << "LS: Ok" << endl;
                    }
                    else if (errno == EACCES) {
                        cout << "LS: Erro de acesso" << endl;
                        char erro[1]; erro[0] = 2;
                        msg_resposta = monta_mensagem(ERRO, 0, erro);
                        envia_mensagem(socket, &msg_resposta, 1);
                    }
                    else {
                        cout << "LS: Outro erro" << endl;
                        char erro[1]; erro[0] = 1;
                        msg_resposta = monta_mensagem(ERRO, 0, erro);
                        envia_mensagem(socket, &msg_resposta, 1);
                    }
                }
                else {
                    cout << "LS: Erro" << endl;
                    char erro[1]; erro[0] = 1;
                    msg_resposta = monta_mensagem(ERRO, 0, erro);
                    envia_mensagem(socket, &msg_resposta, 1);
                }
            }
/*==================================================================================================*/
            else if (msg_recebida->tipo == GET) {
                envia_arquivo(socket, msg_recebida->dados, GET);
                cout << "GET: Ok" << endl;
            }
            else if (msg_recebida->tipo == PUT) {
                pede_arquivo(socket, msg_recebida->dados, PUT);
                cout << "PUT: Ok" << endl;
            }
        }
    }

    return 0;
}