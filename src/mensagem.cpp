#include "mensagem.h"
#include "raw_socket.h"

void aloca_mensagem(mensagem_t **msg) {
    *msg = (mensagem_t *) malloc(sizeof(mensagem_t));

    if(!(*msg)) {
        cerr << "[aloca_mensagem] Erro ao alocar." << endl;
        exit(-1);
    }
}

void libera_mensagem(mensagem_t *msg) {
    if(msg) {
        if(msg->dados) 
            free(msg->dados);
        free(msg);
    }
}

void copia_mensagem(mensagem_t *m1, mensagem_t **m2) {
    if(!m1) {
        cerr << "[copia_mensagem] Parâmetro nulo." << endl;
        exit(-1);
    }
    aloca_mensagem(m2);

    (*m2)->inicio = m1->inicio;
    (*m2)->tamanho = m1->tamanho;
    (*m2)->sequencia = m1->sequencia;
    (*m2)->tipo = m1->tipo;
    (*m2)->dados = m1->dados;
    (*m2)->paridade = m1->paridade;
    m1->dados = NULL;
}

void aloca_str(char **str, int tam) {
    *str = (char *) malloc(tam);

    if(!(*str)) {
        cerr << "[aloca_str] Erro ao alocar." << endl;
        exit(-1);
    }
}

void cstr_tam_seq_tipo_dados(mensagem_t msg, char *cstr, int pos_inicial) {
    cstr[pos_inicial] = msg.tamanho; //000TTTTT
    cstr[pos_inicial] = (cstr[pos_inicial] << 3) | ((msg.sequencia >> 3) & 0x0007); //TTTTTSSS
    cstr[pos_inicial+1] = (msg.sequencia & 0x0007); //00000SSS
    cstr[pos_inicial+1] = (cstr[pos_inicial+1] << 5) | msg.tipo; //SSSTTTTT
    if(msg.tamanho > 0) {
        char c;
        for (int i = 0; i < msg.tamanho; ++i) {
            c = msg.dados[i];
            cstr[pos_inicial+2+i] = c;
        }
    }
}

char* msg_to_cstr(mensagem_t *msg, char* cstr) {
    cstr[0] = msg->inicio;
    cstr_tam_seq_tipo_dados(*msg, cstr, 1);
    cstr[3+msg->tamanho] = msg->paridade;

    /*printf("##### msg_to_cstr:\n");
    printf("##### cstr: ");
    for (int i = 0; i <= msg->tamanho+4; ++i) {
        //printf(" %d ", (int) cstr[i]);
        printf(" %c ", cstr[i]);
    }
    printf("\n");
    printf("##### msg:\n");
    imprime_mensagem(*msg);
    printf("##### fim msg_to_cstr\n");*/

    return cstr;
}

mensagem_t* cstr_to_msg(char *cstr, mensagem_t *msg) {
    msg->inicio = cstr[0];
    msg->tamanho = ((cstr[1] >> 3) & 0x001F); //000TTTTT
    msg->sequencia = ((cstr[1] << 3) & 0x0038) | ((cstr[2] >> 5) & 0x0007); //00SSSSSS
    msg->tipo = (cstr[2] & 0x001F); //000TTTTT
    msg->paridade = cstr[3+(msg->tamanho)];

    
    if(msg->tamanho > 0) {
        aloca_str(&(msg->dados), msg->tamanho);
        char c;       
        for (int i = 0; i < msg->tamanho; ++i) {
            c = cstr[3+i];
            (msg->dados)[i] = c;
        }
    }

    /*printf("##### cstr_to_msg:\n");
    printf("##### cstr: ");
    for (int i = 0; i < msg->tamanho+3; ++i) {
        //printf(" %d ", (int) cstr[i]);
        printf(" %c ", cstr[i]);
    }
    printf("\n");
    printf("##### msg:\n");
    imprime_mensagem(*msg);
    printf("##### fim cstr_to_msg\n");*/

    return msg;
}

mensagem_t* monta_mensagem(int tipo, int sequencia, char* args) {
    mensagem_t *msg = NULL;
    aloca_mensagem(&msg);

    msg->inicio = 0x007E;
    if(args)
        msg->tamanho = strlen(args);
    else
        msg->tamanho = 0;
    msg->sequencia = sequencia;
    msg->tipo = tipo;
    if(msg->tamanho > 0) {
        aloca_str(&(msg->dados), msg->tamanho);
        strcpy(msg->dados, args);
    }
    else
        msg->dados = NULL;
    msg->paridade = calcula_paridade(*msg);

    return msg;
}

char calcula_paridade(mensagem_t msg) {
    char *m = NULL;
    aloca_str(&m, msg.tamanho+2);
    cstr_tam_seq_tipo_dados(msg, m, 0);

    char paridade = 0;
    for (int i = 0; i < msg.tamanho; ++i) {
        paridade = paridade xor m[i];
    }

    //free(m);
    return paridade;
}

void imprime_mensagem(mensagem_t msg) {
    printf("inicio: %c\n", msg.inicio);
    printf("tam: %d\n", msg.tamanho);
    printf("seq: %d\n", msg.sequencia);
    printf("tipo: %d\n", msg.tipo);
    printf("dados: %s\n", msg.dados);
    printf("par: %d\n", (int) msg.paridade);
}

void trata_erros(int tipo, char parametro) {
    switch (tipo) {
        case CD:
            if(parametro == '1')
                cout << "Arquivo ou diretório não encontrado" << endl;
            else
                cout << "Permissão negada" << endl;
        break;
        case CD_LOCAL:
            if(parametro == EACCES)
                cout << "Permissão negada" << endl;
            else if(parametro == ENOENT)
                cout << "Arquivo ou diretório não encontrado"<< endl;
            else if(parametro == ENOTDIR)
                cout << "Arquivo não é um diretório"<< endl;
        break;
        case LS:
            if(parametro == '1')
                cout << "Permissão negada" << endl;
            else
                cout << "Opção inválida" << endl;
        break;
        case GET:
            if(parametro == '1')
                cout << "Arquivo ou diretório não encontrado" << endl;
            else if(parametro == '2')
                cout << "Permissão negada" << endl;
            else
                cout << "Sem espaço em disco" << endl;
        break;
        case PUT:
            if(parametro == '1')
                cout << "Arquivo ou diretório não encontrado" << endl;
            else if(parametro == '2')
                cout << "Permissão negada" << endl;
            else
                cout << "Sem espaço em disco" << endl;
        break;
        default: 
            cout << "Erro"<< endl;
        break;
    }
}

void cd_remoto(int socket, char *args) {
    // Cria mensagem
    mensagem_t *msg = monta_mensagem(CD, 0, args);

    // Envia ao servidor
    envia_mensagem(socket, &msg, 1);
    //libera_mensagem(msg);

    mensagem_t *msg_ok = NULL;
    aloca_mensagem(&msg_ok);
    msg_ok->tipo = NACK;

    // Recebe resposta para requisicao
    while(msg_ok->tipo != OK && msg_ok->tipo != ERRO)
        recebe_mensagem(socket, msg_ok);

    // Envia ACK para resposta da requisicao
    envia_confirmacao(socket, msg_ok->sequencia, ACK);
        
    if(msg_ok->tipo == ERRO)
        cout << "Erro ao executar comando: cdr " << args << endl;
    else
        cout << args << endl;
}

void ls_remoto(int socket, char *args) {
    // Cria mensagem
    mensagem_t *msg = monta_mensagem(LS, 0, args);

    // Envia ao servidor
    envia_mensagem(socket, &msg, 1);
    
    mensagem_t *msg_ok = NULL;
    aloca_mensagem(&msg_ok);
    msg_ok->tipo = NACK;

    // Recebe resposta para requisicao
}