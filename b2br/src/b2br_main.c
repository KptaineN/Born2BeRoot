/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   b2br_main.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkiefer <nkiefer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/03 18:24:37 by nkiefer           #+#    #+#             */
/*   Updated: 2024/12/10 13:24:49 by nkiefer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "get_next_line.h"

// Prototypes
void afficher_menu_principal();
void afficher_etape(const char *description, const char *filename);
void afficher_contenu_simplifie(const char *filename);
void lancer_programme_cache(const char *filename);
void vider_buffer();

#define DIR "texts/"

typedef struct {
    char option; // Change int en char pour permettre des choix comme 'b'
    const char *description;
    const char *filename;
} Step;

Step steps[] = {
    {'1', "Téléchargement de la machine virtuelle", DIR "telechargement_vm.txt"},
    {'2', "Installation de la machine virtuelle", DIR "installation_vm.txt"},
    {'3', "Accéder à la machine virtuelle", DIR "access_vm.txt"},
    {'4', "Configuration de la machine virtuelle", DIR "continue_to_configure.txt"},
    {'5', "Explication Evaluation", DIR "testing.txt"},
    {'6', "Question Theorique", DIR "Qtheorique.txt"},
    {'7', "Question Pratique", DIR "Qpratique.txt"},
    {'b', "Lancer le programme caché", DIR ".programme_bonus"},  // Option cachée
    {'x', "hihi", ".whatisit.sh"},
    {'8', "tester de martin", DIR "tester.txt"},
    {'9', "ReadME", "README.txt"},
    {'0', "Quitter", NULL},
    {'\0', NULL, NULL}
};

int main() {
    char choix;
    do {
        afficher_menu_principal();
        printf("Entrez votre choix : ");
        choix = getchar();
        vider_buffer(); // Vide le buffer après la saisie

        if (choix == '0') {
            printf("Quitter le programme.\n");
            break;
        }

        int found = 0;
        for (int i = 0; steps[i].option != '\0'; i++) {
            if (steps[i].option == choix) {
                found = 1;
                if (choix == 'b' || choix == 'x') { // Les deux options utilisent le programme caché
                    lancer_programme_cache(steps[i].filename);
                } else {
                    afficher_etape(steps[i].description, steps[i].filename);
                }
                break;
            }
        }

        if (!found) {
            printf("Option invalide. Veuillez réessayer.\n");
        }
    } while (choix != '0');

    return 0;
}

void afficher_menu_principal() {
    printf("\n=== Guide Born2BeRoot ===\n");
    for (int i = 0; steps[i].option != '\0'; i++) {
        // N'affiche pas les options 'b' et 'x'
        if (steps[i].option == 'b' || steps[i].option == 'x') {
            continue;
        }
        printf("%c. %s\n", steps[i].option, steps[i].description);
    }
}

void afficher_etape(const char *description, const char *filename) {
    (void)description;
    afficher_contenu_simplifie(filename);
}

void lancer_programme_cache(const char *filename) {
    printf("Lancement du programme caché : %s\n", filename);

    // Utiliser system() pour exécuter le fichier
    char command[256];
    snprintf(command, sizeof(command), "./%s", filename); // Exécute le fichier
    int result = system(command);

    if (result != 0) {
        printf("Erreur lors du lancement de %s.\n", filename);
    } else {
        printf("%s exécuté avec succès.\n", filename);
    }
}

void copier_vers_presse_papier(const char *texte) {
    FILE *fp = popen("xclip -selection clipboard", "w");
    if (fp == NULL) {
        printf("Erreur : impossible d'accéder à xclip.\n");
        return;
    }
    fprintf(fp, "%s", texte);
    pclose(fp);
    printf("Code copié dans le presse-papier !\n");
}

static void strip_newline(char *line) {
    size_t len = strlen(line);
    if (len > 0 && line[len-1] == '\n') {
        line[len-1] = '\0';
    }
}

// Fonction d'extraction du texte entre deux délimiteurs
char *extraire_contenu_delimiteurs(const char *texte, const char *delim_debut, const char *delim_fin) {
    char *start = strstr(texte, delim_debut);
    if (!start) return NULL;
    start += strlen(delim_debut);
    char *end = strstr(start, delim_fin);
    if (!end) return NULL;

    size_t len = (size_t)(end - start);
    char *result = malloc(len + 1);
    if (!result) return NULL;
    strncpy(result, start, len);
    result[len] = '\0';
    return result;
}

// Affichage simplifié - On n'affiche que les parties entre << et >>
void afficher_contenu_simplifie(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        printf("Erreur : impossible d'ouvrir le fichier %s\n", filename);
        return;
    }

    char *line;
    int bloc_mode = 0;
    size_t count = 0;
    size_t capacity = 10;
    char **paragraph = malloc(sizeof(char*) * capacity);

    if (!paragraph) {
        printf("Erreur de mémoire.\n");
        close(fd);
        return;
    }

    while ((line = get_next_line(fd)) != NULL) {
        strip_newline(line);

        if (strncmp(line, "CODE:", 5) == 0) {
            // Début d'un bloc
            bloc_mode = 1;
            count = 0;
            free(line);
            continue;
        } else if (strcmp(line, "end") == 0) {
            // Fin d'un bloc
            bloc_mode = 0;

            // Afficher les lignes collectées
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", paragraph[i]);
                free(paragraph[i]);
            }
            printf("\n");

            // Attendre l'entrée utilisateur
            //printf("Appuyez sur Entrée pour continuer ou 'q' pour quitter...\n");
            int ch = getchar();
            vider_buffer(); // Vide le buffer après saisie

            if (ch == 'q') {
                free(line);
                break;
            }

            count = 0;
            free(line);
            continue;
        }

        if (bloc_mode) {
            // En mode bloc, collecter les lignes
            if (count == capacity) {
                capacity *= 2;
                char **new_paragraph = realloc(paragraph, sizeof(char*) * capacity);
                if (!new_paragraph) {
                    printf("Erreur de mémoire.\n");
                    break;
                }
                paragraph = new_paragraph;
            }
            paragraph[count++] = strdup(line);
        } else {
            // Mode ligne par ligne (hors bloc)
            printf("%s", line);
            //printf("Appuyez sur Entrée pour continuer ou 'q' pour quitter...\n");
            int ch = getchar();
            vider_buffer(); // Vide le buffer après saisie

            if (ch == 'q') {
                free(line);
                break;
            }
        }

        free(line);
    }

    // Libérer la mémoire restante (en cas de sortie anticipée)
    for (size_t i = 0; i < count; i++) {
        free(paragraph[i]);
    }
    free(paragraph);

    close(fd);
}

void vider_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}



