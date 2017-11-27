#include "raw_socket.h"
#include "mensagem.h"
#include "operations.h"

void imprimeMenu();

int main(int argc, char const *argv[]) {
    int socket = openRawSocket((char *) argv[1]);

    if(socket < 0) {
        cerr << "Erro ao abrir Raw Socket." << endl;
        return 1;
    }

    inicia_socket(socket);

    /* Socket aberto com sucesso, iniciar cliente: */

    string operacao, comando, args;
    char *args_c = NULL;
    aloca_str(&args_c, TAM_MSG);
    int comando_pfim;
    
    imprimeMenu();
    while(1) {
        cout << endl << "Entre com a operacao desejada: ";
        getline(cin, operacao);

        /* Descobre a posicao em que acaba a substring com o tipo de operacao (comando) */
        comando_pfim = operacao.find_first_of(" ");

        if(comando_pfim < 0)
            comando_pfim = operacao.size();
        
        /* Descobre o comando e os argumentos */
        comando = operacao.substr(0, comando_pfim);
        args = (comando_pfim+1 < (int) operacao.size()) ? operacao.substr(comando_pfim+1, operacao.size()) : "";
        
        /* Executa a operacao, se listada */
/*==================================================================================================*/
        if(comando == "cd") {
            if(args.size() > 0 && args.size() < 31) {
                strcpy(args_c, args.c_str());

                changeDir(args_c);
                if(errno == 0) 
                    system ("pwd");
                else
                    trata_erros(CD_LOCAL, (char) errno);
            }
            else
                cout << "Endereço com tamanho inválido" << endl;
        }
/*==================================================================================================*/
        else if(comando == "cdr") {
            if(args.size() > 0 && args.size() < 31) {
                strcpy(args_c, args.c_str());
                cd_remoto(socket, args_c);
            }
            else
                cout << "Endereço com tamanho inválido" << endl;
        }
/*==================================================================================================*/
        else if(comando == "ls") {
            strcpy(args_c, args.c_str());
            int *lsArgs = (int *) malloc (3*sizeof(int));
            
            lsArgs = testOptions(args_c);
            if (errno == 0) {
                cout << list(lsArgs[1], lsArgs[0]);
                if (errno)
                    cout << "Permissão negada" << endl;
            }
            else 
                cout << "Opção inválida" << endl;
        }
/*==================================================================================================*/
        else if(comando == "lsr") {
            strcpy(args_c, args.c_str());
            cout << "Operacao lsr com argumentos: " << args << endl;
            ls_remoto(socket, args_c);
            mensagem_t **conteudo;
            int tamLs = recebe_conteudo(socket, &conteudo);
            if (tamLs > 0) msg_to_arq (conteudo, "stdout", tamLs);
        }
/*==================================================================================================*/
        else if(comando == "get") {
            arq_to_msg(socket, args);
            cout << "Operacao get com argumentos: " << args << endl;
        }
/*==================================================================================================*/
        else if(comando == "put") {
            cout << "Operacao put com argumentos: " << args << endl;
        }
/*==================================================================================================*/
        else {
            cout << "Opção inválida" << endl;


            int qtd = 1035;
            mensagem_t **msg = (mensagem_t **) malloc(sizeof(mensagem_t *)*(qtd+2));
            
            strcpy(args_c, args.c_str());
            msg[5] = monta_mensagem(20, 0, args_c);
            

            cout << "Mensagem 20 (antes):" << endl;
            imprime_mensagem(*(msg[5]));

            char * teste;
            aloca_str(&teste, TAM_MSG);
            teste = msg_to_cstr(msg[5], teste);
            msg[5] = cstr_to_msg(teste, msg[5]);

            cout << "Mensagem 20 (depois):" << endl;
            imprime_mensagem(*(msg[5]));


            /*
            envia_mensagem(socket, &(msg[5]), 1);

            msg[4] = monta_mensagem(TAMANHO, 0, "4");
            cout << "Mensagem TAM:" << endl;
            imprime_mensagem(*(msg[4]));
            envia_mensagem(socket, &(msg[4]), 1);

            for (int i = 0; i < qtd; ++i) {
                msg[i] = monta_mensagem(IMPRIMA, i%TAM_SEQUENCIA, "");
                cout << "Mensagem " << i << ":" << endl;
                imprime_mensagem(*(msg[i]));
            }
            
            envia_mensagem(socket, msg, qtd);
            */
        }
    }

    return 0;
}

void imprimeMenu() {
    cout << "Operacoes disponiveis:" << endl;
    
    cout << "* cd [endereco local]" << endl;
    cout << "\tAltera diretorio corrente do cliente para [endereco local]." << endl << endl;

    cout << "* cdr [endereco remoto]" << endl;
    cout << "\tAltera diretorio corrente do servidor para [endereco remoto]." << endl << endl;
    
    cout << "* ls [opcao]" << endl;
    cout << "\tLista conteudo do diretorio corrente do cliente." << endl;
    cout << "\t-a: nao ignora arquivos ocultos" << endl;
    cout << "\t-l: usa formato de listagem longa" << endl << endl;

    cout << "* lsr [opcao]" << endl;
    cout << "\tLista conteudo do diretorio corrente do servidor." << endl;
    cout << "\t-a: nao ignora arquivos ocultos" << endl;
    cout << "\t-l: usa formato de listagem longa" << endl << endl;

    cout << "* get [endereco remoto]" << endl;
    cout << "\tCopia o arquivo [endereco remoto] do servidor para o cliente." << endl << endl;

    cout << "* put [endereco local]" << endl;
    cout << "\tCopia o arquivo [endereco local] do cliente para o servidor." << endl << endl;
}