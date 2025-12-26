#include "commontiff.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[])
{
    // Vérification du nombre d'arguments
    if (argc != 6)
    {
        fprintf(stderr, "Usage: %s <pathFile> <upperLeftX> <upperLeftY> <bottomRightX> <bottomRightY>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Récupération des arguments
    char *pathFile = argv[1];
    int x1 = atoi(argv[2]);
    int y1 = atoi(argv[3]);
    int x2 = atoi(argv[4]);
    int y2 = atoi(argv[5]);

    

    // Affichage des informations
    printf("Fichier : %s\n", pathFile);    

    // Ici, vous pouvez ajouter la logique pour traiter le fichier et les coordonnées

    TIFF *tif = TIFFOpen(pathFile, "r");
    if (!tif)
    {
        fprintf(stderr, "Impossible d'ouvrir le fichier TIFF.\n");
        return 1;
    }

    // Récupérer les dimensions de l'image
    uint32 width, height;
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);

    printf("Image size: %d, %d\n", width, height);

    // Récupérer la taille des tuiles
    uint32 tile_width, tile_height;
    TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tile_width);
    TIFFGetField(tif, TIFFTAG_TILELENGTH, &tile_height);

    // Calculer la taille du rectangle
    uint32 rect_width = x2 - x1;
    uint32 rect_height = y2 - y1;

    // Allouer un buffer pour une tuile
    uint32 tile_size = TIFFTileSize(tif);
    printf("%d , %d = %d\n", tile_width, tile_height, (tile_width * tile_height) * 3);
    printf("Tile size:%d\n", tile_size);
    uint8 *tile_buf = (uint32 *)_TIFFmalloc(tile_size);
    if (!tile_buf)
    {
        fprintf(stderr, "Erreur d'allocation mémoire pour la tuile.\n");
        TIFFClose(tif);
        return 1;
    }

    // Allouer un buffer pour le rectangle extrait
    uint32 *rect_buf = (uint32 *)_TIFFmalloc(rect_width * rect_height * sizeof(uint32));
    if (!rect_buf)
    {
        fprintf(stderr, "Erreur d'allocation mémoire pour le rectangle.\n");
        _TIFFfree(tile_buf);
        TIFFClose(tif);
        return 1;
    }

    printf("new=%d,%d, source=%d,%d,%d,%d\n", rect_width, rect_height, x1, y1, x2, y2);

    int found = generateImageClip(rect_width, rect_height, rect_buf, y1, y2, tile_height, x1, x2, tile_width, tif, tile_buf, width, height);

    // Sauvegarder le rectangle dans un fichier PNG

    char filename[50];
    snprintf(filename, 50, "work/final-%d,%d-%d,%d.png", x1, y1, x2, y2);
    write_png(filename, rect_buf, rect_width, rect_height);

    // Libérer les ressources
    _TIFFfree(tile_buf);
    _TIFFfree(rect_buf);

    return EXIT_SUCCESS;
}