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
    {4, "Configuration de la machine virtuelle", "configuration_vm.txt"},
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
    int in_code_block = 0;
    size_t capacity = 0;
    size_t count = 0;
    char **code_lines = NULL;

    while ((line = get_next_line(fd)) != NULL) {
        strip_newline(line);

        if (!in_code_block && strncmp(line, "CODE:", 5) == 0) {
            // Début du bloc de code
            in_code_block = 1;
            capacity = 10;
            count = 0;
            code_lines = malloc(sizeof(char*) * capacity);
            if (!code_lines) {
                free(line);
                break;
            }
            free(line);
            continue;
        }

        if (in_code_block) {
            // On est dans le bloc de code, on attend 'end'
            if (strcmp(line, "end") == 0) {
                // Fin du bloc
                in_code_block = 0;

                // Afficher uniquement les parties extraites
                printf("\n-Vous pouvez copier avec 'c'");
                for (size_t i = 0; i < count; i++) {
                    printf("%s\n", code_lines[i]);
                }

                printf("\n 'q' pour quitter, ou Entrée pour continuer...\n");

                int ch = getchar();
                if (ch == 'q') {
                    for (size_t i = 0; i < count; i++)
                        free(code_lines[i]);
                    free(code_lines);
                    free(line);
                    close(fd);
                    return;
                } else if (ch == 'c') {
                    // Concaténer le bloc pour extraire (si nécessaire)
                    size_t total_len = 0;
                    for (size_t i = 0; i < count; i++)
                        total_len += strlen(code_lines[i]) + 1;

                    char *full_block = malloc(total_len + 1);
                    if (full_block) {
                        full_block[0] = '\0';
                        for (size_t i = 0; i < count; i++) {
                            strcat(full_block, code_lines[i]);
                            strcat(full_block, "\n");
                        }

                        // On pourrait ré-extraire mais c'est déjà fait.  
                        // Ici, supposons qu'on copie tout le bloc déjà extrait.
                        copier_vers_presse_papier(full_block);
                        free(full_block);
                    }
                    while (getchar() != '\n'); // Vider le buffer
                } else {
                    while (ch != '\n' && ch != EOF) {
                        ch = getchar();
                    }
                }

                for (size_t i = 0; i < count; i++)
                    free(code_lines[i]);
                free(code_lines);
                code_lines = NULL;
                free(line);
                continue;
            } else {
                // Dans le bloc, on extrait immédiatement la partie entre << et >>
                char *extrait = extraire_contenu_delimiteurs(line, "<<", ">>");
                if (!extrait) {
                    // Si aucune délimitation trouvée, on peut choisir de ne rien afficher
                    // ou de tout simplement ignorer cette ligne
                    extrait = strdup("");
                }

                if (count == capacity) {
                    capacity *= 2;
                    char **new_arr = realloc(code_lines, sizeof(char*) * capacity);
                    if (!new_arr) {
                        for (size_t i = 0; i < count; i++)
                            free(code_lines[i]);
                        free(code_lines);
                        free(line);
                        free(extrait);
                        break;
                    }
                    code_lines = new_arr;
                }
                code_lines[count++] = extrait;
                free(line);
                continue;
            }
        }

        // Ligne hors bloc CODE:...end
        if (strlen(line) > 0) {
            printf("%s\n", line);
            //printf("Appuyez sur Entrée pour continuer, 'q' pour quitter...\n");
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

    close(fd);
}

