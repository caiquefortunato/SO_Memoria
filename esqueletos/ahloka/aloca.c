// Esqueleto de um código de alocação de memória (novo maloc)
// Feito para a disciplina DCC065 - Sistemas Operacionais (UFMG)
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "aloca.h"

// Strategia de alocação como variável global. Feio mas funciona dado o
// esqueleto do TP.
static char *STRATEGY = NULL;

// Caso você utilize a lista fora da memória que vamos alocar a mesma
// terá que ser acessada como uma var global. Global vars in C.
free_list_t *HEAP = NULL;

/*
 * Sempre aloca até estourar o limite, não implementei libera
 */
void *alwaysGrow(size_t size) {
  assert(HEAP->lastAlloca != NULL);
  free_node_t *lastAlloca = HEAP->lastAlloca;
  //printf("Ultimo free %lu\n", lastAlloca->free);
  // Temos espaço para alocar + o espaço da lista?
  if (lastAlloca->free < sizeof(free_node_t) + size) {
    return NULL;
  }
  // Sim!
  // Novo nó logo após o último aloca.
  // Posicao da alocacao + tamanho alocado + tamanho do cabeçalho
  free_node_t *newNode = (void*)lastAlloca + lastAlloca->size + \
                         sizeof(free_node_t);
  newNode->next = NULL;
  newNode->size = size;
  newNode->free = lastAlloca->free - sizeof(free_node_t) - size;

  // Só crescemos, o espaço anterior não tem memória livre mais.
  lastAlloca->free = 0;
  lastAlloca->next = newNode;
  HEAP->lastAlloca = newNode; // Atualiza ponteiro para última operação

  // Retornamos o inicio do espaço que alocamos tirando o cabeçalho
  return (void*)newNode + sizeof(free_node_t);
}

void *ff(size_t size) {

	free_node_t *idealPlace;
	idealPlace = NULL;

	assert(HEAP->lastAlloca != NULL);	

	//Pegar a cabeça da lista
	free_node_t *listNavigator = HEAP->head;
	//Navegar até o final procurando uma posição
 	while (listNavigator != NULL) {
		//Pergunta se o próximo nó é espaço livre e cabe o bloco q vc quer colocar?
		if (listNavigator->free != 0 && size + sizeof(free_node_t) <= listNavigator->free) {		//Identifica a posicao					
				idealPlace = listNavigator; //guarda o menor espaço no idealPlace;
				break;
		}
		//Aponta pro próximo da lista
		listNavigator = listNavigator->next;
 	}
	//Se a lista chegou ao fim e não existe espaco suficiente, retorna null
	if (idealPlace == NULL) {
		
		return NULL;
	}
	//if (listNavigator->free < size + sizeof(free_node_t)) return NULL;
	
	//Transforma o lugar ideal no navegador para realizar os outros procedimentos
	listNavigator = idealPlace;
	
	//Se achar alocar o tamanho necessário para o novo nó
  	free_node_t *newNode = (void*)listNavigator + size + sizeof(free_node_t);//Mudar isso
  	
	//Aloca posição de memória no fim da lista
	//o newNode aponta para aonde o listNavigator está apontando
	//se estiver no final da lista apotna pra null, caso contrario aponta pro proximo
	if(listNavigator == NULL) newNode->next = NULL;
	else newNode->next = listNavigator->next;
	//o newNode recebe seu tamanho
	newNode->size = size;
	//o newNode recebe o cálculo do restante de memória presente na lista		
	newNode->free = listNavigator->free - sizeof(free_node_t) - size;

	//Como um bloco foi alocado diretamente após, o bloco anterior não tem mais espaço livre na sua frente;
	listNavigator->free = 0;
	listNavigator->next = newNode;

	
	//Se está no final da lista o ponteiro de final é direcionado para ele
	if(listNavigator == NULL)	HEAP->lastAlloca = newNode;
	
	//printf("Alocou, livre: %lu\n", newNode->free);
  	// Retornamos o inicio do espaço que alocamos tirando o cabeçalho
  	return (void*)newNode + sizeof(free_node_t);
}

void *bf(size_t size) {

	size_t idealSize = MEMSIZE;
	free_node_t *idealPlace;
	idealPlace = NULL;

	assert(HEAP->lastAlloca != NULL);	

	//Pegar a cabeça da lista
	free_node_t *listNavigator = HEAP->head;
	//Navegar até o final procurando uma posição
 	while (listNavigator != NULL) {
		//Pergunta se o próximo nó é espaço livre e cabe o bloco q vc quer colocar?
		if (listNavigator->free != 0 && size + sizeof(free_node_t) <= listNavigator->free) {		//Identifica a posicao		
			//idealSize = listNavigator->free;	
			if(listNavigator->free <= idealSize) {//Se é o menor espaço
				
				idealPlace = listNavigator; //guarda o menor espaço no idealPlace;
				idealSize = listNavigator->free;
			}
		}
		//Aponta pro próximo da lista
		listNavigator = listNavigator->next;
 	}
	//Se a lista chegou ao fim e não existe espaco suficiente, retorna null
	if (idealPlace == NULL) {
		return NULL;
	}
	//if (listNavigator->free < size + sizeof(free_node_t)) return NULL;
	
	//Transforma o lugar ideal no navegador para realizar os outros procedimentos
	listNavigator = idealPlace;
	
	//Se achar alocar o tamanho necessário para o novo nó
  	free_node_t *newNode = (void*)listNavigator + size + sizeof(free_node_t);//Mudar isso
  	
	//Aloca posição de memória no fim da lista
	//o newNode aponta para aonde o listNavigator está apontando
	//se estiver no final da lista apotna pra null, caso contrario aponta pro proximo
	if(listNavigator == NULL) newNode->next = NULL;
	else newNode->next = listNavigator->next;
	//o newNode recebe seu tamanho
	newNode->size = size;
	//o newNode recebe o cálculo do restante de memória presente na lista		
	newNode->free = listNavigator->free - sizeof(free_node_t) - size;

	//Como um bloco foi alocado diretamente após, o bloco anterior não tem mais espaço livre na sua frente;
	listNavigator->free = 0;
	listNavigator->next = newNode;
	
	//Se está no final da lista o ponteiro de final é direcionado para ele
	if(listNavigator == NULL)	HEAP->lastAlloca = newNode;
	
	//printf("Alocou, livre: %lu\n", newNode->free);
  	// Retornamos o inicio do espaço que alocamos tirando o cabeçalho
  	return (void*)newNode + sizeof(free_node_t);
}

void *wf(size_t size) {

	size_t idealSize = 0;
	free_node_t *idealPlace;
	idealPlace = NULL;

	assert(HEAP->lastAlloca != NULL);	

	//Pegar a cabeça da lista
	free_node_t *listNavigator = HEAP->head;
	//Navegar até o final procurando uma posição
 	while (listNavigator != NULL) {
		if (listNavigator->free != 0 && size + sizeof(free_node_t) <= listNavigator->free) {		//Identifica a posicao	
			if(listNavigator->free >= idealSize) {//Se é o maior espaço livre
				idealPlace = listNavigator; //guarda o menor espaço no idealPlace;
				idealSize = listNavigator->free;
			}
		}
		//Aponta pro próximo da lista
		listNavigator = listNavigator->next;
 	}
	//Se a lista chegou ao fim e não existe espaco suficiente, retorna null
	if (idealPlace == NULL){
	 return NULL;
	 }
	//if (listNavigator == NULL && listNavigator->free < size + sizeof(free_node_t)) return NULL;
	
	//Transforma o lugar ideal no navegador para realizar os outros procedimentos
	listNavigator = idealPlace;
	
	//Se achar alocar o tamanho necessário para o novo nó
  	free_node_t *newNode = (void*)listNavigator + size + sizeof(free_node_t);//Mudar isso
  	
	//Aloca posição de memória no fim da lista
	//o newNode aponta para aonde o listNavigator está apontando
	//se estiver no final da lista apotna pra null, caso contrario aponta pro proximo
	if(listNavigator == NULL) newNode->next = NULL;
	else newNode->next = listNavigator->next;
	//o newNode recebe seu tamanho
	newNode->size = size;
	//o newNode recebe o cálculo do restante de memória presente na lista		
	newNode->free = listNavigator->free - sizeof(free_node_t) - size;

	//Como um bloco foi alocado diretamente após, o bloco anterior não tem mais espaço livre na sua frente;
	listNavigator->free = 0;
	listNavigator->next = newNode;
	
	//Se está no final da lista o ponteiro de final é direcionado para ele
	if(listNavigator == NULL)	HEAP->lastAlloca = newNode;
	
  	// Retornamos o inicio do espaço que alocamos tirando o cabeçalho
  	return (void*)newNode + sizeof(free_node_t);
}

//Código do alwaysGrow é exatamente o nextFit!
void *nf(size_t size) {
  assert(HEAP->lastAlloca != NULL);
  free_node_t *lastAlloca = HEAP->lastAlloca;
  //printf("Ultimo free %lu\n", lastAlloca->free);
  // Temos espaço para alocar + o espaço da lista?
  if (lastAlloca->free < sizeof(free_node_t) + size) {
    return NULL;
  }
  // Sim!
  // Novo nó logo após o último aloca.
  // Posicao da alocacao + tamanho alocado + tamanho do cabeçalho
  free_node_t *newNode = (void*)lastAlloca + lastAlloca->size + \
                         sizeof(free_node_t);
  newNode->next = NULL;
  newNode->size = size;
  newNode->free = lastAlloca->free - sizeof(free_node_t) - size;

  // Só crescemos, o espaço anterior não tem memória livre mais.
  lastAlloca->free = 0;
  lastAlloca->next = newNode;
  HEAP->lastAlloca = newNode; // Atualiza ponteiro para última operação

  // Retornamos o inicio do espaço que alocamos tirando o cabeçalho
  return (void*)newNode + sizeof(free_node_t);
}

void *aloca(size_t size) {
  if (strcmp(STRATEGY, "ag") == 0) {
    return alwaysGrow(size);
  }

  if (strcmp(STRATEGY, "ff") == 0) {
    return ff(size);
  }

  if (strcmp(STRATEGY, "bf") == 0) {
    return bf(size);
  }

  if (strcmp(STRATEGY, "wf") == 0) {
    return wf(size);
  }

  if (strcmp(STRATEGY, "nf") == 0) {
    return nf(size);
  }

  return NULL;
}


void libera(void *ptr) {

	//free_node_t* fnt = (free_node_t*) ptr;
 
	free_node_t *metaData = (void*)ptr - sizeof(free_node_t);

	assert(HEAP->lastAlloca != NULL);	

	//Pegar a cabeça da lista
	free_node_t *listNavigator = HEAP->head;
	
	while(listNavigator != NULL) {
	
		if(listNavigator->next == metaData) {
			//printf("achei");
			listNavigator->next = metaData->next;
			listNavigator->free += metaData->size + metaData->free + sizeof(free_node_t);
			break;
		}
	
		
		listNavigator = listNavigator->next;
	}
}


void calcula_final() {

	unsigned int maior = 0, total = 0;
	float resultado;
	//EMF = 1 - (Memoria mais larga / Memoria total free)
	
	//Pegar a cabeça da lista
	free_node_t *listNavigator = HEAP->head;

	//Navegar até o final procurando uma posição
 	while (listNavigator != NULL) {
		if(listNavigator->free > maior) {
			maior = listNavigator->free;
		}
				total += listNavigator->free;
		//Aponta pro próximo da lista
		listNavigator = listNavigator->next;
 	}
 	
 	//printf("MAIOR = %d e TOTAL = %d\n", maior, total);
 	if(maior == 0 && total == 0) printf("1\n");
 	else {
		resultado = (double)maior/total;
		printf("Total == %f\n",  1 - resultado);
	}

}

void run(void **variables) {
  // Vamos iniciar alocando todo o MEMSIZE. Vamos split e merges depois.
  // Vou iniciar o HEAP usando NULL, deixa o SO decidir. Podemos usar sbrk(0)
  // também para sugerir o local inicial.
  HEAP = mmap(NULL, MEMSIZE,
              PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
              -1, 0);
  assert(HEAP != NULL);
  assert(HEAP != MAP_FAILED);

  HEAP->head = (void*) HEAP + sizeof(free_list_t);
  HEAP->lastAlloca = HEAP->head;
  HEAP->head->size = 0;
  HEAP->head->free = MEMSIZE - sizeof(free_list_t) - sizeof(free_node_t);
  HEAP->head->next = NULL;

  int opid;    // ID da operação
  int memsize; // Tamanho da alocação
  char optype; // Tipo da operação
  void *addr;
  while (scanf("%d", &opid) == 1) {
    getchar();
    scanf("%d", &memsize);
    getchar();
    scanf("%c", &optype);
    getchar();
    //printf("Alocando %d; %d; %c\n", opid, memsize, optype);
    if (optype == 'a') {         // Aloca!
      addr = aloca(memsize);
      if (addr == NULL) {
        printf("mem full\n");
        munmap(HEAP, MEMSIZE);
        exit(1);
      }
      variables[opid] = addr;
    } else if (optype == 'f') {  // Free!
      addr = variables[opid];
      libera(addr);
    } else {
      printf("Erro na entrada");
      munmap(HEAP, MEMSIZE);
      exit(1);
    }
  }
  
  /*
  free_node_t *node = HEAP->head;
  
  while(node != NULL) {
  	printf("Valor restante %zd\n", node->free);
  	
  	node = node->next;
  	
  }
  */
  
  calcula_final();
  munmap(HEAP, MEMSIZE);
}



int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage %s <algorithm>\n", argv[0]);
    exit(1);
  }
  STRATEGY = argv[1];

  int nops;
  scanf("%d\n", &nops);
  //printf("%d\n", nops);

  char *algorithms[] = {"ff", "bf", "wf", "nf", "ag"};
  int n_alg = 5;
  int valid = 0;
  for (int i = 0; i < n_alg; i++) {
    if (strcmp(STRATEGY, algorithms[i]) == 0) {
      valid = 1;
      break;
    }
  }
  if (valid == 0) {
    printf("Algoritmo inválido: Usage %s <algorithm>\n", argv[0]);
    printf("--onde o algoritmo vem das opções: {ff, bf, wf, nf, ag}\n");
    exit(1);
  }

  // O vetor variables mantem os endereços de ids alocados.
  // É lido ao executarmos uma operação 'f'
  void **variables = (void **) malloc(nops * sizeof(void**));
  assert(variables != NULL);

  for (int i = 0; i < nops; i++)
    variables[i] = NULL;

  run(variables);
  free(variables);
}
