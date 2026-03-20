//
// Created by Lorenzo Andreotta on 09/01/26.
//

/**
 * @brief Necessaria all'interazione con i file di salvataggio. Ogni volta che si avvia il gioco, l'array
 * data verrà caricato con delle struct presenti in un file indice, in modo da poter mostrare le
 * statistiche di ciascun salvataggio.
 * I file di salvataggio avranno un nome del tipo "salvataggio0.bin", quindi l'indice di data verrà usato per la stringa da inserire in fopen().
 * Si tratta di una rivisitazione dell'array dinamico (sostitutivo del vector di C++ dato che in C non è presente) visto a lezione nel modulo di teoria di laP.
 */
#include "stdbool.h"
#include "stdlib.h"
#include "stdio.h"

#ifndef PROGETTOINC_VECTOR_H
#define PROGETTOINC_VECTOR_H

#endif //PROGETTOINC_VECTOR_H

/**
 * @brief È presente una struct chiamata fileFormat, il cui scopo è quello memorizzare i dati principali
 * del salvataggio (data, pVita, monete, oggetti...), che verranno mostrati quando dal menù
 * verrà selezionata l'opzione per caricare/eliminare un salvataggio.
 */
typedef struct s_fileFormat{
    char data[50];
    int pVita;
    int monete;
    int oggetti;
    int missioni;
    int deleted;
}fileFormat;

typedef struct s_vector{
    size_t size;
    size_t capacity;
    fileFormat *data;
}vector;

vector* vCreate(){
    vector* newV=(vector*)malloc(sizeof(vector));
    newV->size=0;
    newV->capacity=8;
    newV->data=(fileFormat*)malloc(sizeof(fileFormat)*newV->capacity);
    if(newV->data==NULL) exit(EXIT_FAILURE);

    return newV;
}

void vFree(vector* v){
    if(v->data!=NULL)
        free(v->data);
    free(v);
}

void vAdd(vector* v, fileFormat data){
    if(v->size==v->capacity){
        fileFormat* temp=(fileFormat*) realloc(v->data, v->capacity*2*sizeof(fileFormat));
        if(temp==NULL) exit(EXIT_FAILURE);
        free(v->data); //dato che sto facendo una shallow copy libero data altrimenti ogni volta che size supera capacity lascio allocata la zona di memoria più piccola
        v->data=temp;
        v->capacity*=2;
    }

    v->data[v->size]=data;
    v->size++;
}

void vDelete(vector* v, size_t index){
    if(index>v->size){
        printf("ERROR: index out of bounds");
    }else{
        int i = 0;
        for(i=index; i<v->size-1; i++){
            v->data[i]=v->data[i+1];
        }
        v->size--;

        /*se dopo l'eliminazione la dimensione "logica" (se avevi la mongelli per informatica sai cos'è) è minore di 1/4 di quella "fisica" allora
         * dimezzo la dimensione di data per non sprecare memoria*/
        if(v->capacity>8&&v->size<v->capacity/4){
            fileFormat* temp=(fileFormat*)realloc(v->data, (v->capacity/2)*sizeof(fileFormat));
            if(temp==NULL) exit(EXIT_FAILURE);
            free(v->data); //stesso motivo descritto sopra
            v->data=temp;
            v->capacity=v->capacity/2;
        }
    }
}

fileFormat vGet(vector* v, size_t index) {
    if (index >= v->size) {
        printf("ERROR: index out of bounds\n");
        return (fileFormat) {"", 0, 0, 0, 0,};
    } else
        return v->data[index];
}

void vPrint(vector* v){
    if(v->size>0){
        int i = 0;
        for(i=0; i<v->size; i++){
            printf("\n%d. ", i+1);
            printf("%s\t", v->data[i].data);
            printf("%d P. VITA\t", v->data[i].pVita);
            printf("%d MONETE\t", v->data[i].monete);
            printf("%d OGGETTI \t", v->data[i].oggetti);
            printf("%d MISSIONI COMPLETATE \t", v->data[i].missioni);
        }
        printf("\n");
    }else{
        printf("\nNessun salvataggio presente");
    }
}