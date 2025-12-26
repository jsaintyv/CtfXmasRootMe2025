#include "commontiff.h"

int main()
{
    TIFF *tif = TIFFOpen("white_as_snow/xaac.tif", "r");
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

    uint32 basew = width/10;
    uint32 baseh = height/10;

    for (int zy = 0; zy < 10; zy++)
    
    {
        for (int zx = 1; zx < 10; zx++)
        {
            

            // Coordonnées du rectangle à extraire
            uint32 x1 = zx * basew;
            uint32 y1 = zy * baseh;
            uint32 x2 = (zx + 1) * basew;
            if(x2 >= width) {
                x2 = width-1;                
            }

            
            uint32 y2 = (zy + 1) * baseh;
            if(y2 >= height) {
                y2 = height -1;                
            }
            //  uint32 x1 = 1000, y1 = 1000;
            // uint32 x2 = 3000, y2 = 3000;

            printf("source=%d,%d,%d,%d\n", x1, y1, x2, y2);

            if (x1 % tile_width != 0)
            {
                x1 = x1 - (x1 % tile_width);
            }

            if (x2 % tile_width != 0)
            {
                x2 = x2 + tile_width - (x2 % tile_width) - 1;
            }

            if (y1 % tile_height != 0)
            {
                y1 = y1 - (y1 % tile_height);
            }

            if (y2 % tile_height != 0)
            {
                y2 = y2 + tile_height - (y2 % tile_height) - 1;
            }

            // Vérifier que les coordonnées sont valides
            if (x1 >= width || y1 >= height )
            {
                continue;                                
            }

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
            snprintf(filename, 50, "work/rect%d_%d.png", zx, zy);
            write_png(filename, rect_buf, rect_width, rect_height);

            // Libérer les ressources
            _TIFFfree(tile_buf);
            _TIFFfree(rect_buf);
        

            printf("Rectangle extrait et enregistré en PNG avec succès %s !\n", filename);
            if(found > 0) {
                exit(0);
            }
        }
    }
        TIFFClose(tif);
    return 0;
}
