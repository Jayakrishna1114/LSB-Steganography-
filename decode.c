#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "common.h"
#include "types.h"

Status read_and_validate_decode_args(int argc, char *argv[],   DecodeInfo *decInfo) // function to read and validate decode arguments
{
    if(argc < 3 || argc > 4) // check for valid number of arguments 
    {
        printf("Decoding: ./a.out -d <bmp_file> [output file]\n");
        return e_failure;
    }
    if (strstr(argv[2], ".bmp") == NULL) // check for .bmp in argv[2]
    {
        printf("ERROR: Stego image must be a .bmp file\n");
        return e_failure;
    }
    else
    {
        decInfo -> stego_image_fname = argv[2]; // store stego image file name
        printf("INFO: Stego image name stored\n");
    }
    if (argv[3] != NULL)
    {
        decInfo -> secret_fname = argv[3]; // store output file name if given
        printf("INFO: Output file name stored\n");
    }
    else
    {
        decInfo -> secret_fname = NULL;
        printf("INFO: Output file name not provided, using default\n");
    }
    return e_success;
}


Status skip_bmp_header(FILE *fptr_stego_image) // function to skip bmp header
{
    //Skip fptr directly to 54 as we dont need header data for secret.txt 
    fseek(fptr_stego_image, 54, SEEK_SET);
    return e_success;
}

Status open_decode_files( DecodeInfo *decInfo)
{
    if (decInfo->secret_fname == NULL)
    {
        char default_name[50] = "decoded";
        strcat(default_name, decInfo -> extn_secret_file);
        decInfo -> fptr_secret = fopen(default_name, "wb");
    }
    else
    {
        decInfo->fptr_secret = fopen(decInfo->secret_fname, "wb");
    }

    if (decInfo->fptr_secret == NULL)
    {
        perror("fopen");
        return e_failure;
    }
    return e_success;
}
//main logic hw 2
Status decode_lsb_to_byte(char *data, char *image_buffer) // function to decode lsb to byte
{
    *data = 0; // initialize data to 0 
    for (int i = 0; i < 8; i++) // loop for 8 bits
    {
        *data = (*data << 1) | (image_buffer[i] & 0x01); // get each LSB from image buffer and store in data
    }
    return e_success;
}
//logic to decode char data from image (.txt or file data)
Status decode_image_to_data(char *data, int size, FILE *fptr_stego_image) // function to decode image to data
{
    char image_buffer[8]; // create 8 bit buffer to get each bit of char
    for (int i = 0; i < size; i++)
    {
        fread(image_buffer, 8, 1, fptr_stego_image); // read 8 bytes from stego image to buffer
        //decode_lsb_to_byte(&data[i], image_buffer);
        char ch = 0; // initialize ch to 0
        for (int j = 0; j < 8; j++)  // loop for 8 bits 
        {
            ch = (ch << 1) | (image_buffer[j] & 1); // get each LSB from image buffer and store in ch
        }

        data[i] = ch; // store ch in data array
    }
    
    return e_success;
}
//logic to decode int data from image ( .txt size and secret file size)
Status decode_size_from_image(int *data,FILE *fptr_stego_image) // function to decode size from image
{
    char image_buffer[32]; // create 32 bit buffer to get each bit of int
    *data = 0; // initialize data to 0
    fread(image_buffer, 32, 1, fptr_stego_image); // read 32 bytes from stego image to buffer
    for (int i = 0; i < 32; i++) // loop for 32 bits
    {
        *data = (*data << 1) | (image_buffer[i] & 1); // get each LSB from image buffer and store in data
    }
    return e_success;
}
//decode & check magic string
Status decode_magic_string(const char *magic_string,  DecodeInfo *decInfo) // function to decode magic string
{
    //find length of our magic string
    int len = strlen(magic_string);
    //create a temporary char array to read magic string from image and checking with ours 
    char decoded[len + 1];
    if(decode_image_to_data(decoded, len, decInfo->fptr_stego_image) != e_success) // function call to decode image to data
    {
       
         printf("ERROR: Unable to decode magic string\n");
        return e_failure;
    }
     decoded[len] = '\0'; // null terminate the string
    
    if (strcmp(decoded, magic_string) != 0) // compare decoded magic string with original
    {
        printf("ERROR: Magic string mismatch\n");
        return e_failure;
    }
    printf("INFO: Magic string matched\n");
    return e_success;
}
//decode secret file extn size (int)
Status decode_secret_file_extn_size( DecodeInfo *decInfo) // function to decode secret file extension size
{
    int size; // temporary storage for size
    if(decode_size_from_image(&size,decInfo->fptr_stego_image) != e_success) // function call to decode size from image
    {
        printf("ERROR: Secret file extn size not decoded correctly\n");
        return e_failure;
    }
    //store this extn size in variable(.txt= 4 bytes)
    decInfo -> extn_size = size;  // store size in decInfo structure
    return e_success;
}
//decode secret file extn (char)
Status decode_secret_file_extn( DecodeInfo *decInfo) // function to decode secret file extension 
{
    memset(decInfo->extn_secret_file, 0, MAX_FILE_SUFFIX); // initialize extn_secret_file to 0

    if(decode_image_to_data(decInfo -> extn_secret_file, 
                                    decInfo -> extn_size, 
                                    decInfo->fptr_stego_image) != e_success) // function call to decode image to data
    {
        printf("ERROR: Secret file extension name not decoded correctly\n");
        return e_failure;
    }
    decInfo -> extn_secret_file[decInfo -> extn_size] = '\0'; // null terminate the string
    return e_success;
}
//decode secret file size(int)
Status decode_secret_file_size( DecodeInfo *decInfo) // function to decode secret file size
{
    int size; // temporary storage for size
    if(decode_size_from_image(&size,decInfo -> fptr_stego_image) != e_success) // function call to decode size from image
    {
        printf("ERROR: Secret file size not decoded correctly\n");
        return e_failure;
    }
    decInfo -> size_secret_file = size; // store size in decInfo structure
    return e_success;
}

Status decode_secret_file_data( DecodeInfo *decInfo) // function to decode secret file data
{
    char ch; // temporary storage for each byte
    for (int i = 0; i < decInfo->size_secret_file; i++) // loop for size of secret file
    {
        if(decode_image_to_data(&ch, 1, decInfo->fptr_stego_image) != e_success) // function call to decode image to data
        {
            printf("ERROR: Secret file data not decoded correctly\n");
            return e_failure;
        }
        fwrite(&ch, 1, 1, decInfo->fptr_secret); // write each byte to secret file
    }
    return e_success;
}

Status do_decoding( DecodeInfo *decInfo)
{
    printf("INFO: ## Decoding Procedure Started ##\n");

    printf("INFO: Opening required files\n");
    printf("INFO: Opened steged_beautiful.bmp\n");
    //open steged image file for decoding data
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");
    if (decInfo->fptr_stego_image == NULL)
    {
        printf("ERROR: Error while opening %s file\n",decInfo->stego_image_fname);
        perror("fopen");
        return e_failure;
    }
    printf("INFO: Files opened successfully\n");
    //Skip the 54 bytes header
    skip_bmp_header(decInfo->fptr_stego_image);
    //1. Decode & validate magic string
    printf("INFO: Decoding Magic String Signature\n");

    if (decode_magic_string(MAGIC_STRING, decInfo) != e_success)
    {
        //If no magic string close image file and exit
        fclose(decInfo->fptr_stego_image);
        return e_failure;
    }
    printf("INFO: Done\n");

    //2. Decode extension size(4 bytes)(int)
    decode_secret_file_extn_size(decInfo);
    //3. Decode extension name(.txt or .pdf,etc)(char)
    decode_secret_file_extn(decInfo);
    //4. Create output file with decoded extension & file name if given, else default name 
    
    if (open_decode_files(decInfo) != e_success)
        return e_failure;
        printf("INFO: Opened secret_msg.txt\n");
        printf("INFO: Done. Opened all required files\n");

    //5. Decode secret file size (int)
    printf("INFO: Decoding secret_msg.txt File Size\n");

    decode_secret_file_size(decInfo);
    printf("INFO: Done\n");

    //6. Decode secret file data (char)
    printf("INFO: Decoding secret_msg.txt File Data\n");

    decode_secret_file_data(decInfo);
    printf("INFO: Done\n");

    //7. Close both files & exit 
    fclose(decInfo->fptr_secret);
    fclose(decInfo->fptr_stego_image);
    printf("INFO: ## Decoding Done Successfully ##\n");

    return e_success;
}