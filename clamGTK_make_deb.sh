#!/bin/bash

FILE_BIN="ClamGTK"

DOSSIER_BUILD="DEBUILD"
DOSSIER_DATA="$DOSSIER_BUILD/$FILE_BIN/usr/share/ClamGTK/DATA"
DOSSIER_BIN="$DOSSIER_BUILD/$FILE_BIN/usr/bin"
DOSSIER_DEBIAN="$DOSSIER_BUILD/$FILE_BIN/DEBIAN"
DOSSIER_APPLICATION="$DOSSIER_BUILD/$FILE_BIN/usr/share/applications/"

IMAGE_CLAMAV="DATA/clamav.png"
IMAGE_BT_SEARCH="DATA/search.png"
IMAGE_BT_UPDATE="DATA/update.png"
IMAGE_BT_FILE="DATA/file.png"
IMAGE_BT_FOLDER="DATA/folder.png"
CLAMGTK_SCRIPT="DATA/clam_gtk_update_script.sh"
DESKTOP_SCRIPT="DATA/clamgtk.desktop"

POSTINST_SCRIPT="#!/bin/bash"
POSTRM_SCRIPT="#!/bin/bash"
PREINST_SCRIPT="#!/bin/bash"
PRERM_SCRIPT="#!/bin/bash"

CONTROL_SCRIPT="Package: $FILE_BIN\nVersion: 1.0\nSection: Utility\nPriority: optional\nArchitecture: all\nDepends: libgtk-3-0\nMaintainer: Bubi Gaudrioles <bubi.gaudrioles@hotmail.com>\nDescription: Interface Graphique pour ClamAV GTK3\nHomepage: https://github.com/Gaudrioles"

case "$1" in
    "make")
    if [ -d "$DOSSIER_BUILD" ]; then
        printf "run clam_GTK_make_deb.sh clean\n"
        exit
    else
        mkdir -p $DOSSIER_DATA
        mkdir -p $DOSSIER_BIN
        mkdir -p $DOSSIER_DEBIAN
        mkdir -p $DOSSIER_APPLICATION
        printf "Creation des repertoires\n"

        echo -e $POSTINST_SCRIPT > $DOSSIER_DEBIAN/postinst
        chmod 755 $DOSSIER_DEBIAN/postinst
        echo -e $POSTRM_SCRIPT > $DOSSIER_DEBIAN/postrm
        chmod 755 $DOSSIER_DEBIAN/postrm
        echo -e $PREINST_SCRIPT > $DOSSIER_DEBIAN/preinst
        chmod 755 $DOSSIER_DEBIAN/preinst
        echo -e $PRERM_SCRIPT > $DOSSIER_DEBIAN/prerm
        chmod 755 $DOSSIER_DEBIAN/prerm
        echo -e $CONTROL_SCRIPT > $DOSSIER_DEBIAN/control
        printf "Creation des scripts\n"
    fi
    
    if [ -f "$FILE_BIN" ]; then
        printf "$FILE_BIN\t\t\t\t[OK]\n"
    else 
        printf "run make clean && make\n"
        exit
    fi

    cp $FILE_BIN $DOSSIER_BIN
    printf "$FILE_BIN\t\t\t\t[OK]\n"
    cp $IMAGE_CLAMAV $DOSSIER_DATA
    printf "$IMAGE_CLAMAV\t\t\t[OK]\n"
    cp $IMAGE_BT_SEARCH $DOSSIER_DATA
    printf "$IMAGE_BT_SEARCH\t\t\t[OK]\n"
    cp $IMAGE_BT_UPDATE $DOSSIER_DATA
    printf "$IMAGE_BT_UPDATE\t\t\t[OK]\n"
    cp $IMAGE_BT_FILE $DOSSIER_DATA
    printf "$IMAGE_BT_FILE\t\t\t[OK]\n"
    cp $IMAGE_BT_FOLDER $DOSSIER_DATA
    printf "$IMAGE_BT_FOLDER\t\t\t[OK]\n"
    cp $CLAMGTK_SCRIPT $DOSSIER_DATA
    printf "$CLAMGTK_SCRIPT\t[OK]\n"
    cp $DESKTOP_SCRIPT $DOSSIER_APPLICATION
    printf "$DESKTOP_SCRIPT\t\t[OK]\n"
    printf "\nsudo dpkg-deb --build $DOSSIER_BUILD/$FILE_BIN\n"
    ;;
    
    "clean")
    if [ -d "$DOSSIER_BUILD" ]; then
        rm -rf $DOSSIER_BUILD
    else 
        printf "$DOSSIER_BUILD existe pas\n"
        exit
    fi
    ;;

    *)
    printf "Bad flag\n"
    printf " make || clean\n"
    ;;
esac