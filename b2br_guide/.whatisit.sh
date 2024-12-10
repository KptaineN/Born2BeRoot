#!/bin/bash

tput civis  # Cache le curseur

# Fonction pour afficher une frame centrée dans le terminal
display_frame() {
    local text=("$@")
    local rows=${#text[@]}
    local cols=${#text[0]}
    local start_row=$(( ( $(tput lines) - rows ) / 2 ))
    local start_col=$(( ( $(tput cols) - cols ) / 2 ))
    clear

    for (( i=0; i<rows; i++ )); do
        tput cup $(( start_row + i )) $start_col
        echo -e "${text[$i]}"
    done
    sleep 0.2
}

# Frames ASCII pour l'animation
frames=(
    "╚═( ͡° ͜ʖ ͡°)═╝
 ╚═(███)═╝
  ╚═(███)═╝
 ╚═(███)═╝
  ╚═(███)═╝"
         
  	 "╚═(╬▂╬)═╝
 ╚═(███)═╝
   ╚═(███)═╝
 ╚═(███)═╝
  ╚═(███)═╝"
         
 	   "╚═(ಠ益ಠ)═╝
 ╚═(███)═╝
  ╚═(███)═╝
 ╚═(███)═╝
  ╚═(███)═╝"
)

# Fonction pour gérer l'entrée clavier
check_input() {
    read -t 0.01 -n 1 key  # Lit une touche avec un délai court
    if [[ $key == "q" ]]; then
        tput cnorm  # Restaure le curseur
        clear
        exit 0
    fi
}

# Boucle infinie pour afficher les frames jusqu'à ce que 'q' soit pressé
while true; do
    for frame in "${frames[@]}"; do
        IFS=$'\n' read -r -d '' -a lines <<< "$frame"
        display_frame "${lines[@]}"
        check_input  # Vérifie si 'q' a été pressé
    done
done


