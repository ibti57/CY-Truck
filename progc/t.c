#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>


//Structure d'une ville
typedef struct Ville{
    char * nom;
    int total;
    int depart;
}Ville;


// tructure d'un AVL
typedef struct AVL{
    Ville* ville;
    struct AVL* fG;
    struct AVL* fD;
    int eq;
}AVL;


//Fonction qui créer un Noeud
AVL* creationNoeud(Ville* v){
    AVL* noeud = malloc(sizeof(AVL));

    if (noeud == NULL){
        exit(1);
    }

    noeud->fG = NULL;
    noeud->ville = v;
    noeud->fD = NULL;
    noeud->eq = 0;

    return noeud;    
}


//Fonction qui retourne le minimum entre deux valeurs
int min(int a, int b){
    if (a < b){
        return a;
    }
    else {
        return b;
    }
}


//Fonction qui retourne le maximum entre deux valeurs 
int max(int a, int b){
    if (a > b){
        return a;
    }
    else {
        return b;
    }
}


//Fonction rotation simple gauche avce les équilibres à chaque noeud (cours)
AVL* rotationG(AVL* a){
  if (a == NULL) {
    return a;
  }

  int eqa, eqp; 
  AVL * p = a->fD;

  eqa = a->eq;
  eqp = p->eq;

  a->fD = p->fG;
  p->fG = a;

  a->eq = eqa - max(eqp, 0) - 1;
  p->eq = min(eqa - 2, min(eqa + eqp - 2, eqp - 1));

  return p;
}


//Fonction rotation simple droite avec les équilibres à chaque noeud (cours)
AVL* rotationD(AVL* a){
  if (a == NULL) {
    return a;
  }

  int eqa, eqp; 
  AVL * p = a->fG;

  a->fG = p->fD;
  p->fD = a;

  eqa = a->eq;
  eqp = p->eq;

  a->eq = eqa - min(eqp, 0) + 1;
  p->eq = max(eqa + 2, max(eqa + eqp + 2, eqp + 1));

  return p;
}


//Fonction rotation double gauche (cours)
AVL * rotationDoubleG(AVL * a){
  if (a == NULL) {
    return a;
  }

  a->fD = rotationD(a->fD);
  return rotationG(a);
}


//Fonction rotation double droite (cours)
AVL* rotationDoubleD(AVL * a){
  if (a == NULL) {
    return a;
  }

  a->fG = rotationG(a->fG);
  return rotationD(a);
}


//fonction équilibre (cours)
AVL* equilibre(AVL* a){
    if(a == NULL){
        return a;
    }

    if(a->eq >= 2){
        if(a->fD->eq >= 0 ){
            return rotationG(a);
        }
        else{
            return rotationDoubleG(a);
        }
    }

    else if(a->eq <= -2){
        if(a->fG->eq <= 0 ){
            return rotationD(a);
        }
        else{
            return rotationDoubleD(a);
        }
    }
    return a;
}


//Fonction insertion d'une ville dans AVL
AVL* insertion(AVL * a, Ville * v, int * h){
    if(a == NULL){
        *h = 1;
        return creationNoeud(v);
    }
    else if(a->ville->total > v->total){
        a->fG = insertion(a->fG,v,h);
        *h = -*h;
    }
    else if(a->ville->total < v->total){
        a->fD = insertion(a->fD,v,h);
    }
    else if(v->total == a->ville->total){
        int cmp = strcmp(v->nom, a->ville->nom);
        if(cmp > 0){
            a->fD = insertion(a->fD,v,h);
        }
        else{
            a->fG = insertion(a->fG,v,h);
            *h = -*h;
        }
    }
    else{ 
        *h = 0;
        return a;
    }

    if(*h != 0){
        a->eq = a->eq + *h;
        a = equilibre(a);
        if(a->eq == 0){
            *h=0;
        }
        else{
            *h=1;
        }
    }

    return a;       
}


//Fonction qui récupère les données 
AVL * donnee(char *nom_f) {
    FILE *fichier = NULL;
    fichier = fopen(nom_f, "r");

    if (fichier == NULL) {
        fprintf(stderr, "Le fichier %s est inconnu \n", nom_f); // Ecrire l'erreur dans la sortie stderr
        exit(2);
    }
    AVL * a = NULL;
    char ligne[500];

    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        char *token = strtok(ligne, ",");

        if (token != NULL) {
            Ville * v = malloc(sizeof(Ville));
            v->nom = strdup(token);

            token = strtok(NULL, ",");
            v->total= atoi(token);

            token = strtok(NULL, ",");
            v->depart = atoi(token);

            int h = 0;
            a = insertion(a, v, &h);
        }
    }

    fclose(fichier);
    return a;
}


//Parcours suffixe dans AVL
void affichage(AVL* a, int * cpt, FILE *fp){
    if (a != NULL && *cpt > 0) {
        affichage(a->fD,cpt, fp);

        if (*cpt > 0) {
            fprintf(fp, "%s,%d,%d\n", a->ville->nom, a->ville->total, a->ville->depart);
            (*cpt)--;
        }
           affichage(a->fG,cpt,fp);
    }
}


//Libération de l'espace en mémoire de la structure Ville
void libererV(Ville* v) {
    if (v != NULL) {
        free(v->nom);
        free(v);
    }
}


//Libération de l'espace en mémoire d'un AVL
void libererAVL(AVL* a){
    if(a != NULL){
        libererAVL(a->fG);
        libererAVL(a->fD);
        libererV(a->ville);
        free(a);
    }
}


int main(int n, char *parametre[]){
    
    //Test du nombre d'arguments
    if ( n != 2){
        fprintf(stderr, "%s : Nombre de paramètres incorrect, un fichier est attendu \n", parametre[0]);
        return 1;
    }

    
    AVL* avl = donnee(parametre[1]); 
    int compte  = 10;
    
    
    //Création du fichier pour avoir les 10 villes 
    char fichiertemp[] = "temp/fin.csv";
    FILE *fp=fopen(fichiertemp, "w");
    if(!fp){
    	fprintf(stderr, "Erreur lors de l'ouverture du fichier.\n");
    	libererAVL(avl);
    	return 1;
    	}
    
    
    affichage(avl, &compte,fp);
    
    fclose(fp);

    libererAVL(avl);

    return 0;
}
