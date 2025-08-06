#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

#define WINDOW_WIDTH    800
#define WINDOW_HEIGHT   400
#define BUFFER_SIZE     256

#define MSG_COMPILE     "\n----------- Compilation des Signatures en cours -----------\n"
#define MSG_SCAN        "\n----------- Analyse en cours -----------\n"
#define MSG_SCAN_F      "\n----------- Analyse Terminée -----------\n"
#define MSG_EMPTY       "\n----------- Aucun élément à analyser -----------\n"
#define MSG_UPDATE      "\n\n----------- Mise à jours -----------\n\n"
#define MSG_UPDATE_F    "\n\n----------- Mise à jours Terminée -----------\n\n"
#define MSG_VIRUS       "Fichier dangereux identifié\nsupprimer ou conserver ?\n"
#define MSG_RUN         "zenity --warning --title \"ClamGTK\" --width 200 --height 100 --text \"ClamGTK already running\""

#define UPDATE_OK       "up-to-date"
#define UPDATE_KO       "updated"

#define CLAMGTK_APP     "ClamGTK.app"

#define CMD_FRESHCLAN   "pkexec bash \"/usr/share/ClamGTK/update.sh\""
#define CMD_DEL         "pkexec rm"

#define UI_ANALYSE      "Analyse"
#define UI_BT_QUIT      "Quitter"
#define UI_BT_RETOUR    "Retour"
#define UI_ANALYSER     "Analyser"
#define UI_UPDATE       "Mise à jour"
#define UI_SLT_FILE     "Sélectionner un fichier"
#define UI_SLT_FOLDER   "Sélectionner un dossier"
#define UI_SCAN_TXT     "Désignez des cibles pour commencer l'analyse."
#define UI_DB           "Base de donnée"
#define UI_DATE         "Mise à jour"

#endif /* !DEFS_H_INCLUDED */