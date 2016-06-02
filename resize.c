/**
 * resize.c - branch of copy.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Resize a BMP using a factor of 1 - 100.
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./resize factor infile outfile\n");
        return 1;
    }

    // remember factor and filenames
    int* factor = argv[1];
    char* infile = argv[2];
    char* outfile = argv[3];

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // copy bf and bi into bf_mod & bi_mod to preserve original header values
    BITMAPFILEHEADER bf_mod = bf;
    BITMAPINFOHEADER bi_mod = bi;

    // change width & heght by factor
    bi_mod.biWidth *= factor;
    bi_mod.biHeight *= factor;

    // determine padding for scanlines
    int padding =  (4 - (bi_mod.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // set new biSizeImage
    bi_mod.biSizeImage = ((bi_mod.biWidth * sizeof(RGBTRIPLE)) + padding) * bi_mod.biHeight;

    // set new bfSize
    bf_mod.bfSize = bi_mod.biSizeImage + 54;

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf_mod, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi_mod, sizeof(BITMAPINFOHEADER), 1, outptr);

/* CODE SWEEP BELOW HERE */

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // iterate over pixels in scanline
        for (int j = 0; j < bi.biWidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            // write RGB triple to outfile
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
        }

        // skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);

        // then add it back (to demonstrate how)
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, outptr);
        }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
