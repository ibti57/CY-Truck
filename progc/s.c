#include<stdio.h>
#include <stdlib.h>

//Structure de l'Arbre 
typedef struct ABR{
  int route_ID;
  int etape_ID;
  float distance;
  float min;
  float max;
  int n;
  struct ABR* fG;
  struct ABR* fD;
}ABR;


//Structure AVL
typedef struct AVL{
    int route_ID;
    float min;
    float max;
    float moy;
    float diff;
    int eq;
    struct AVL* fG;
    struct AVL* fD;
}AVL;


//Fonction qui créer les noeuds de l'arbre
ABR* creationNoeudABR(int route_ID, int etape_ID, float distance, float min, float max){
    ABR* pNew = malloc(sizeof(ABR));
    
    if (pNew == NULL) {
        printf("Erreur au niveau du malloc");
        exit(2);
    }

    pNew->route_ID = route_ID;
    pNew->etape_ID = etape_ID;
    pNew->distance = distance;
    pNew->min = min;
    pNew->max = max;
    pNew->n = 1 ;
    pNew->fG = NULL;
    pNew->fD = NULL;
    return pNew;
}


//Fonction insertion dans un ABR (cours)
ABR* insertionABR(ABR* abr, int route_ID, int etape_ID, float distance){
    if(abr == NULL){
        abr = creationNoeudABR(route_ID, etape_ID, distance, distance, distance);
    }
    else if(route_ID < abr->route_ID){
        abr->fG = insertionABR(abr->fG, route_ID, etape_ID, distance);
    } 
    else if(route_ID > abr->route_ID){
        abr->fD = insertionABR(abr->fD, route_ID, etape_ID, distance);
    }
    else if(route_ID == abr->route_ID){
        abr->n++;
        abr->distance += distance;
        if(distance > abr->max){
            abr->max = distance;
        }

        if(distance < abr->min){
            abr->min = distance;
        }
    }

    return abr;
}

// Fonction pour lire le fichier csv 
ABR* lectureCSV(const char* data, ABR* abr) {
    FILE* fichier = fopen(data, "r");
    if (fichier == NULL) {
        perror("Erreur dans l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    int route_ID, etape_ID;
    float distance;
    
    while(fscanf(fichier, "%d;%d;%f", &route_ID, &etape_ID, &distance) == 3) {
        
        abr = insertionABR(abr, route_ID, etape_ID, distance);
    }
    return abr;

    fclose(fichier);
}

void inversion(AVL* avl, FILE* fichier2) {
    if (avl != NULL) {
        inversion(avl->fD, fichier2);
        
        fprintf(fichier2, "%d;%.3f;%.3f;%.3f;%.3f\n", avl->route_ID, avl->min, avl->max, avl->moy, avl->diff);
        inversion(avl->fG, fichier2);
    }
}


//Fonction qui créer les noeuds de l'AVL (cours)
AVL* creationNoeudAVL(ABR* abr){
    
    AVL* new = malloc(sizeof(AVL));
    if (new == NULL) {
        printf("Erreur au niveau du malloc");
        exit(3);
    }

    new->route_ID = abr->route_ID;
    new->min = abr->min;
    new->max = abr->max;
    new->moy = (abr->distance / (double)abr->n);
    new->diff = abr->max - abr->min;
    new->eq = 0;
    new->fG = NULL;
    new->fD = NULL;

    return new;
}


//Fonction qui retourne le minimum entre trois entiers 
int min3(int a, int b, int c){
return (a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c);
}

//Fonction qui retourne le max entre deux entiers 
int max2(int a, int b){
    return (a > b) ? a : b;
}

//Fonction rotation gauche (cours)
AVL* RotationG(AVL* avl){
    if(avl == NULL){
        printf("L'arbre est null\n");
        exit(4);
    }
    AVL* pivot;
    int eq_a, eq_p;

    pivot = avl->fD;
    avl->fD = pivot->fG;
    pivot->fG = avl;
    
    eq_a = avl->eq;
    eq_p = pivot->eq;
    avl->eq = eq_a - max2(eq_p, 0) - 1;
    pivot->eq = min3(eq_a - 2, eq_a + eq_p - 2, eq_p-1);

    avl = pivot;

    return avl;
}

//Fonction qui retourne le minimum entre deux entiers
int min2(int a, int b) {
    return (a < b) ? a : b;
}

//Fonction qui retourne le maximum entre trois entiers
int max3(int a, int b, int c) {
    return (a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c);
}

//Fonction rotation droite (cours)
AVL* RotationD(AVL* avl){
    if(avl == NULL){
        printf("L'arbre est null\n");
        exit(5);
    }
    AVL* pivot;
    int eq_a, eq_p;

    pivot = avl->fG;
    avl->fG = pivot->fD;
    pivot->fD = avl;
    
    eq_a = avl->eq;
    eq_p = pivot->eq;
    avl->eq = eq_a - min2(eq_p, 0) + 1;
    pivot->eq = max3(eq_a + 2, eq_a + eq_p + 2, eq_p + 1);

    avl = pivot;

    return avl;
}

//Focnction double rotation gauche (cours)
AVL* doubleRotationG(AVL* avl){
    if(avl == NULL){
        printf("L'arbre est null\n");
        exit(6);
    }
    avl->fD = RotationD(avl->fD);
    return RotationG(avl);
}

//Fonction double rotation droite (cours)
AVL* doubleRotationD(AVL* avl){
        if(avl == NULL){
        printf("L'arbre est null\n");
        exit(7);
    }
    avl->fG = RotationG(avl->fG);
    return RotationD(avl);
}

//Fonction pour equilibre un AVL (cours)
AVL* equilibrageAVL(AVL* avl){
        if(avl == NULL){
        printf("L'arbre est null\n");
        exit(8);
    }
    if(avl->eq > 1){
        if(avl->fD->eq >= 0){
            return RotationG(avl);
        }
        else{
            return doubleRotationG(avl);
        }
    }
    else if(avl->eq < -1){
        if(avl->fG->eq <= 0){
            return RotationD(avl);
        }
        else{
            return doubleRotationD(avl);
        }
    }
    return avl;
}


//Fonction insertion dans un AVL avec l'equilibrage (cours)
AVL* insertionAVL(AVL* avl, int* h, ABR* abr){
    AVL* new = creationNoeudAVL(abr);

    if(avl ==  NULL){
        *h = 1;
        return new;
    }
    else if(new->diff < avl->diff){
        avl->fG = insertionAVL(avl->fG, h, abr);
        *h = -(*h);
    }
    else if(new->diff > avl->diff){
        avl->fD = insertionAVL(avl->fD, h, abr);
    }
    else{
        *h = 0;
        return avl;
    }

    if(*h != 0){
        avl->eq = avl->eq + *h;
        avl = equilibrageAVL(avl);
        if(avl->eq == 0){
            *h = 0;
        }
        else{
            *h = 1;
        }

    }
    return avl;
}


//Parcours de l'ABR et insertion dans l'AVL
AVL* parcoursAVL(ABR* abr, AVL* avl, int h){
    if(abr != NULL){

        avl = insertionAVL(avl, &h, abr);
        

        avl = parcoursAVL(abr->fG, avl, h);
        avl = parcoursAVL(abr->fD, avl, h);
    } 
    return avl;
}


//Fonction pour libérer ABR 
void freeABR(ABR* abr) {
    if (abr == NULL) {
        return;
    }

    //Libére les sous arbres
    freeABR(abr->fG);
    freeABR(abr->fD);

    
    free(abr);
}


//Fonction pour libérer AVL 
void freeAVL(AVL* avl){
    //si NULL alors return
    if (avl == NULL) {
        return;
    }

    //Libére les sous arbres
    freeAVL(avl->fG);
    freeAVL(avl->fD);

    //Libére le noeud sur lequel on se trouve 
    free(avl);
}


int main(int argc, char *argv[]){
     //Verrification des arguments
    if (argc != 2) {
        printf("Il y a plus d'un argument ");
        exit (1);
    }

    ABR* abr = NULL;
    abr = lectureCSV(argv[1], abr);

    AVL* avl = NULL;
 
    int h = 0;
    avl = parcoursAVL(abr, avl, h);
    

    //Ouvrir un fichier en écriture
    FILE *fichiertemp = fopen("temp/sortie.csv", "w");
    if (fichiertemp == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    inversion(avl, fichiertemp);
    printf("\n");

    fclose(fichiertemp);

    freeABR(abr);
    freeAVL(avl);

    return 0 ;
}
