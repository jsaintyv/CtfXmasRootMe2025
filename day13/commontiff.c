#include "commontiff.h"

void write_png(const char *filename, uint32 *buffer, uint32 width, uint32 height)
{
    FILE *fp = fopen(filename, "wb");
    if (!fp)
    {
        fprintf(stderr, "Impossible de créer le fichier PNG.\n");
        return;
    }

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        fclose(fp);
        fprintf(stderr, "Erreur lors de la création de la structure PNG.\n");
        return;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_write_struct(&png_ptr, NULL);
        fclose(fp);
        fprintf(stderr, "Erreur lors de la création de l'information PNG.\n");
        return;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        fprintf(stderr, "Erreur lors de l'écriture du fichier PNG.\n");
        return;
    }

    png_init_io(png_ptr, fp);

    // Configurer les informations de l'image
    png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    // Allouer une ligne pour l'écriture
    png_bytep row_pointers[height];
    for (uint32 y = 0; y < height; y++)
    {
        row_pointers[y] = (png_bytep)malloc(3 * width * sizeof(png_byte));
        for (uint32 x = 0; x < width; x++)
        {
            uint32 pixel = buffer[y * width + x];
            row_pointers[y][3 * x] = (pixel >> 16) & 0xFF;    // Rouge
            row_pointers[y][3 * x + 1] = (pixel >> 8) & 0xFF; // Vert
            row_pointers[y][3 * x + 2] = pixel & 0xFF;        // Bleu
        }
    }

    png_set_rows(png_ptr, info_ptr, row_pointers);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    // Libérer la mémoire
    for (uint32 y = 0; y < height; y++)
    {
        free(row_pointers[y]);
    }

    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
}


int generateImageClip(uint32 rect_width, uint32 rect_height, uint32 *rect_buf, uint32 y1, uint32 y2, uint32 tile_height, uint32 x1, uint32 x2, uint32 tile_width, TIFF *tif, uint8 *tile_buf, uint32 width, uint32 height)
{
    int found = 0;
    // Lire chaque tuile et extraire la région d'intérêt
    for (int x = 0; x < rect_width * rect_height; x++)
    {
        rect_buf[x] = 0xff;
    }

    for (uint32 y = y1; y < y2; y += tile_height)
    {
        for (uint32 x = x1; x < x2; x += tile_width)
        {
            // Calculer les coordonnées de la tuile
            uint32 tile_x = x / tile_width;
            uint32 tile_y = y / tile_height;

            // printf("Tile %d, %d\n", tile_x, tile_y);

            // Lire la tuile
            if (TIFFReadTile(tif, tile_buf, x, y, 0, 0) < 0)
            {
                fprintf(stderr, "Erreur de lecture de la tuile (%u, %u).\n", tile_x, tile_y);
                continue;
            }

            // Copier les pixels de la tuile vers le rectangle
            for (uint32 ty = 0; ty < tile_height && (y + ty) < y2; ty++)
            {
                for (uint32 tx = 0; tx < tile_width && (x + tx) < x2; tx++)
                {
                    int trueCoordX = tx + x;
                    int trueCoordY = ty + y;

                    if (trueCoordX >= width || trueCoordY >= height)
                    {
                        continue;
                    }

                    uint32 src_idx = (ty * tile_width + tx) * 3;
                    uint32 r = tile_buf[src_idx];
                    uint32 g = tile_buf[src_idx + 1];
                    uint32 b = tile_buf[src_idx + 2];

                    uint32 color = (r << 16) + (g << 8) + b;

                    uint32 dy = (ty + y - y1);
                    uint32 dx = (tx + x - x1);

                    uint32 dst_idx = dy * rect_width + dx;

                    rect_buf[dst_idx] = color;

                    if (color != 0xffffff)
                    {
                        printf("%d, %d %08x\n", trueCoordX, trueCoordY, color);

                        printf("Coord in 100kx100k resolution : (%d, %d)\n\n", 4 * trueCoordX, 4 * trueCoordY);
                        found++;
                    }
                }
            }
        }
    }
    return found;
}
