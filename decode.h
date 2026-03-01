#ifndef DECODE_H
#define DECODE_H
#include<stdio.h>
#include"common.h"
#include "types.h" // Contains user defined types

/* Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1 // 1 byte buffer to store secret data 
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8) // 8 bytes buffer to store image data
#define MAX_FILE_SUFFIX 4 // max file suffix size

typedef struct _DecodeInfo  // structure to store decode information
{

    /* Secret File Info */
    char *secret_fname;  // secret file name
    FILE *fptr_secret;   // secret file pointer
    char extn_secret_file[MAX_FILE_SUFFIX]; // to store secret file extension
    char secret_data[MAX_SECRET_BUF_SIZE]; // to store secret data
    long size_secret_file; // size of secret file
    int extn_size;  // size of extension

    /* Stego Image Info */
    char *stego_image_fname; // stego image file name
    FILE *fptr_stego_image; // stego image file pointer

} DecodeInfo; // 


/* Decoding function prototype */



/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(int argc,char *argv[], DecodeInfo *decInfo);

/* Perform the encoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_decode_files(DecodeInfo *decInfo);

/* Skip bmp image header */
Status skip_bmp_header( FILE *fptr_stego_image);

/* Store Magic String */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/* Encode secret file extenstion */
Status decode_secret_file_extn( DecodeInfo *decInfo);

Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Encode secret file size */
Status decode_secret_file_size( DecodeInfo *decInfo);

/* Encode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Encode function, which does the real encoding */
Status decode_image_to_data(char *data, int size, FILE *fptr_stego_image);

/* Encode a byte into LSB of image data array */
Status decode_lsb_to_byte(char * data, char *image_buffer); // -> Not returning
//Status decode_lsb_to_byte( char *image_buffer); // -> returning

Status decode_size_from_image(int *data,FILE *fptr_stego_image);

Status decode_lsb_to_size(int* data, char *image_buffer); // -> not 

#endif
