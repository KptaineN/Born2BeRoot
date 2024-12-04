/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   b2br_main.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nkiefer <nkiefer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/03 18:24:37 by nkiefer           #+#    #+#             */
/*   Updated: 2024/12/04 15:59:41 by nkiefer          ###   ########.fr       */
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
void copier_vers_presse_papier(const char *texte);

typedef struct {
    int option;
    const char *description;
    const char *filename;
} Step;

Step steps[] = {
    {1, "Téléchargement de la machine virtuelle", "telechargement_vm.txt"},
    {2, "Installation de la machine virtuelle", "installation_vm.txt"},
    {3, "Accéder à la machine virtuelle", "access_vm.txt"},
    {4, "Configuration de la machine virtuelle", "continue_to_configure.txt"},
    {5, "Explication Evaluation", "testing.txt" },
    {0, NULL, NULL}
};

int main() {
    int choix;
    do {
        afficher_menu_principal();
        printf("Entrez votre choix : ");
        if (scanf("%d", &choix) != 1) {
            while (getchar() != '\n');
            continue;
        }
        getchar(); // Consomme le '\n'

        if (choix == 0) {
            printf("Quitter le programme.\n");
            break;
        }

        int found = 0;
        for (int i = 0; steps[i].option != 0; i++) {
            if (steps[i].option == choix) {
                found = 1;
                afficher_etape(steps[i].description, steps[i].filename);
                break;
            }
        }

        if (!found) {
            printf("Option invalide. Veuillez réessayer.\n");
        }
    } while (choix != 0);

    return 0;
}

void afficher_menu_principal() {
    printf("\n=== Guide Born2BeRoot ===\n");
    for (int i = 0; steps[i].option != 0; i++) {
        printf("%d. %s\n", steps[i].option, steps[i].description);
    }
    printf("0. Quitter\n");
}

void afficher_etape(const char *description, const char *filename) {
    (void)description;
    afficher_contenu_simplifie(filename);
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
            // Si un bloc commence
            bloc_mode = 1;
            count = 0;
            free(line);
            continue;
        } else if (strcmp(line, "end") == 0) {
            // Si un bloc se termine
            bloc_mode = 0;

            // Afficher le paragraphe collecté
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", paragraph[i]);
                free(paragraph[i]);
            }
            printf("\n");

            // Attente utilisateur
            printf("\nAppuyez sur Entrée pour continuer ou 'q' pour quitter...\n");
            int ch = getchar();
            if (ch == 'q') {
                free(line);
                break;
            }
            while (ch != '\n' && ch != EOF)
                ch = getchar();

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
            // Mode ligne par ligne
            printf("%s\n", line);
           // printf("Appuyez sur Entrée pour continuer ou 'q' pour quitter...\n");
            int ch = getchar();
            if (ch == 'q') {
                free(line);
                break;
            }
            while (ch != '\n' && ch != EOF)
                ch = getchar();
        }

        free(line);
    }

    // Libérer la mémoire en cas de sortie
    for (size_t i = 0; i < count; i++) {
        free(paragraph[i]);
    }
    free(paragraph);

    close(fd);
}



