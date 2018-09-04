//Everton da Silva Coelho  101937
//Gabriel Borin Takahashi 101953
//Pedro Manhez Naresi 105615

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define div 1

#define MAX_PAGINAS 500/div
#define MAX_RAM 250/div

#define TEMPO 1000/div
#define TAU 300/div
#define T_MEDICAO 200/div

typedef struct Item{
    int id_page;
    int hit;
    long time;
}TItem;

typedef struct Celula TApontador;

typedef struct Celula{
    TItem item;
    TApontador *proximo;
    TApontador *anterior;
}TCelula;

typedef struct Lista{
    int tamanho;
    TApontador *frente;
}TLista;

void TLista_Inicia(TLista *lista){
    lista->tamanho=0;
    TCelula *cabeca = (TCelula*) malloc(sizeof(TCelula));
    lista->frente=cabeca;
    cabeca->proximo=lista->frente;
    cabeca->anterior=lista->frente;
}

int TLista_Tamanho(TLista *pLista){
    return pLista->tamanho;
}

int TLista_EhVazia(TLista *pLista){
    if(pLista->tamanho == 0)
        return 1;
    else
        return 0;
}

TApontador* TLista_Busca(TLista *lista, int x){
    TApontador *aux = lista->frente->proximo;
    while(aux!=lista->frente){
        if(aux->item.id_page == x){
            return aux;
        }
        aux=aux->proximo;
    }
    return NULL;
}

int TLista_Insere(TLista *lista, TApontador *p, TItem x){
    if(TLista_Tamanho(lista)==MAX_RAM){
        return 0;
    }
    else{
        TCelula *novo = (TCelula*)malloc(sizeof(TCelula));
    	novo->item=x;
        novo->proximo=p;
        novo->anterior=p->anterior;
        p->anterior->proximo=novo;
        p->anterior=novo;
        lista->tamanho++;
        return 1;
    }
}
/*
int TLista_Retira(TLista *lista, int n, TItem *x){
    if(TLista_EhVazia(lista)){
        printf("A lista nao possui elementos\n");
        return 0;
    }
    else{
        int i;
        TApontador *p;  //e a posicao de insercao é passada como referencia
        p=lista->frente->proximo;  //sempre insere na primeira posicao, dps da cabeca
        for(i=0; i<n; i++){
            p=p->proximo;
        }
        p=p->anterior;
        p->anterior->proximo=p->proximo;
        p->proximo->anterior=p->anterior;
        *x=p->item;
        free(p);
        lista->tamanho--;
        return 1;
    }
}*/

int TLista_Retira(TLista *lista, TApontador *p, TItem *x){
    if(TLista_EhVazia(lista)){
        return 0;
    }
    else{
        p->anterior->proximo=p->proximo;
        p->proximo->anterior=p->anterior;
        *x=p->item;
        free(p);
        lista->tamanho--;
        return 1;
    }
}

void TLista_Printa(TLista *lista){
	TApontador *aux = lista->frente->proximo;
	while(aux!=lista->frente){
		printf("id_page: %d\t", aux->item.id_page);
        printf("page_hit: %d\t", aux->item.hit);
        printf("time_acess: %ld\n", aux->item.time);
		aux=aux->proximo;
	}
}

void Rand(TItem *x, int clock){
    x->id_page=rand() % MAX_PAGINAS;
    x->hit=1;
    x->time=clock;
}

void zeraTudoOsHit(TLista *lista){
	TApontador *aux = lista->frente->proximo;
    while(aux!=lista->frente){
        aux->item.hit=0;
        aux=aux->proximo;
    }
}

TApontador* subsPage(TLista *lista, long clk){
    TApontador *aux = lista->frente->proximo;
    TApontador *max_time;
    long t=9999999;
    while(aux!=lista->frente){
    	if(aux->item.hit==1){
    		aux->item.hit=0;
    	}
        else if(aux->item.hit==0){ //if page is unreferenced
        	if((clk - aux->item.time) > TAU){ //chech if age is bigger than tau
	            return aux;
	        }
	        else if(aux->item.time < t){ //find the oldest one, minimum aux.time
	        	t = aux->item.time;
	        	max_time = aux;
	        }
	    }
        aux=aux->proximo;
    }
    return max_time;
}

void wsClock(TLista *lista){
    int i, count_hit=0, page_miss=0, cont=1;
    long clock=0;
    TItem x;
    TApontador *posicao=lista->frente->proximo;
    TApontador *aux;
    for(i=0; i<TEMPO; i++){
    	clock++;
        Rand(&x, clock);
        aux=TLista_Busca(lista, x.id_page);
        if(aux==NULL){ //if not in memory
            if(TLista_Tamanho(lista)<MAX_RAM){ //check if there's free memory space
                if(TLista_Insere(lista, posicao, x)){
                	printf("nº %d \t Inseriu_id: %d\tclock: %ld\n",cont, x.id_page, clock);
                	cont++;
                }
                else printf("ERRO_0\n");
            }
            else{ //if all pages are referenced, find the oldest one
                TApontador *page_out;
                TItem xx;
                page_out=subsPage(lista, clock);
                if(TLista_Retira(lista, page_out, &xx)){
                	if(TLista_Insere(lista, posicao, x)){
                		printf("Retirou_id: %d e Inseriu_id: %d \tclock: %ld\n", xx.id_page, x.id_page, clock);
                	}
                	else printf("ERRO_1\n");
                }
                else printf("ERRO_2\n");
            }
            page_miss++;
        }
        else{ //if page already exist in memory
            aux->item.hit=1;
            aux->item.time=clock;
            count_hit++;
        }
        if(clock>=T_MEDICAO){
        	zeraTudoOsHit(lista);
        	clock=0;
        	printf("\t\t\t\t\tZEROU CLOCK\n");
        }
    }
    printf("count hit: %d\n", count_hit);
    printf("page miss: %d\n", page_miss);
}

int main(){    
    TLista lista;
    TLista_Inicia(&lista);
    wsClock(&lista);
    //TLista_Printa(&lista);
    return 0;
}