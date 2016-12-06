#include <stdio.h>
#include <stdlib.h>

struct node{
  int x;
  int y;
  struct node *next;
};

void stack_push(struct node **top,int x,int y){
  struct node *aux=(struct node *)malloc(sizeof(struct node));
  if(aux!=NULL){
    aux->x=x;
    aux->y=y;
    aux->next=*top;
  }
  *top=aux;
}

void stack_pop(struct node **top){
  if(*top!=NULL){
    struct node *aux=*top;
    *top=(*top)->next;
    free(aux);
  }
}

void stack_print(struct node *top){
  if(top!=NULL){
    struct node *aux=top;
    while(aux!=NULL){
      printf("%d,%d ",aux->x,aux->y);
      aux=aux->next;
    }
  }
}

void stack_delete(struct node **top){
  if(*top!=NULL){
    struct node *aux;
    while(*top!=NULL){
      aux=*top;
      *top=(*top)->next;
      free(aux);
    }
  }
}

int stack_is_empty(struct node *top){
  return top==NULL?1:0;
}

int stack_elements_count(struct node *top){
  int i=0;
  struct node *aux=top;
  while(aux!=NULL){
    aux=aux->next;
    i++;
  }
  return i;
}
  

/*
int main(int argc,char *argv[]){
  struct node *pila=NULL;
  int i=0,j=10;
  for(i=0;i<10;i++){
    stack_push(&pila,i,j--);
  }
  stack_print(pila);
  printf("\n");
  printf("%d\n\n",stack_is_empty(pila));
  stack_pop(&pila);
  stack_pop(&pila);
  stack_pop(&pila);
  stack_print(pila);
  printf("\n%d\n\n",stack_is_empty(pila));
  stack_delete(&pila);
  stack_print(pila);
  printf("%d\n",stack_is_empty(pila));
  return 0;
}
*/
