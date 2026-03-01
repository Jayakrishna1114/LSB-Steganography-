/*
NAME                : JAYAKRISHNA 
DATE                : 15-01-2026
PROGRAM NAME        : LSB Steganography 
AIM OF THE PROJECT  : To implement LSB Steganography technique in C language to hide and retrieve secret data within BMP image files.

WHERE IT IS USED    :  * This technique is used in secure communications, digital watermarking, and data protection to conceal information within images without noticeable changes.
                       * It is widely applied in fields such as cybersecurity, digital forensics, and intellectual property protection to ensure data confidentiality and integrity.

DESCRIPTION         : -> LSB Steganography to securely hide a secret text file inside a BMP image without visible changes.
 
                      -> It uses bitwise operations and file handling in C to embed and extract hidden data.

                      ->  The system supports both encoding and decoding processes through command-line arguments.

                      ->  It includes error handling for file operations and validates input parameters.

                      -> The program structure is modular, with separate functions for each task, enhancing readability and maintainability.

                      -> The implementation ensures that the BMP image's header remains unaltered during the encoding process.
                    
                      -> The program checks if the image has sufficient capacity to hold the secret data before encoding.
                    
                      -> The decoding process accurately retrieves the hidden data, reconstructing the original secret file.
                    
                      -> The program handles all edge cases and provides informative error messages.
                    
                      -> The system is designed to be user-friendly, with clear instructions for usage.
                    
FILES USED         : encode.c,
                     decode.c, 
                     encode.h, 
                     decode.h, 
                     common.h, 
                     types.h, 
                     test_encode.c.

ENOCODE OPERATIONS : 1. Open source BMP image and secret text file.
                     2. Validate input arguments and check image capacity.
                     3. Copy BMP header to stego image.
                     4. Encode magic string, secret file extension, size, and data into the image using LSB technique.
                     5. Save the modified image as stego image.

DECODE OPERATIONS  : 1. Open stego BMP image.
                     2. Validate input arguments.
                     3. Skip BMP header.
                     4. Decode and validate magic string.
                     5. Decode secret file extension, size, and data from the image.
                     6. Save the extracted data to a new secret file.    

ENCODE USAGE       : ./a.out -e <bmp_file> <.txt file> [output file]
DECODE USAGE       : ./a.out -d <.bmp_file> [output file]

COMPILATION        : gcc -c *.c
                     gcc *.o -o stego
RUN COMMAND        : ./stego -e beautiful.bmp secret.txt steged_img.bmp
                     ./stego -d steged_img.bmp decoded.txt                 
                     
CONCEPTS USED      : File handling, Bitwise operations, Data encoding/decoding, Error handling.

Environment        : Windows OS, GCC compiler.

Examples          : To encode a secret text file into a BMP image:
                     ./stego -e beautiful.bmp secret.txt steged_img.bmp  (or) ./a.out -e beautiful.bmp secret.txt output.bmp

                     To decode the secret text file from the stego BMP image:
                     ./stego -d steged_img.bmp decoded.txt   (or) ./a.out -d steged_img.bmp decode.txt

END OF PROJECT DESCRIPTION  
***************************


*/
#include <stdio.h>
#include<string.h>
#include"common.h"
#include "encode.h"
#include"decode.h"
#include "types.h"

OperationType check_operation_type(char *argv[]) // function to check operation type
{
    /*
    Step -1: check if the argv[1] is "-e" or not
             if yes -> return e_encode
    Step -2: check if the argv[1] is "-d" or not
             if yes -> return e_decode
             if NO -> e_unsupported
            
    */
   if (strcmp(argv[1], "-e") == 0) // To check for encoding operation
    {
        return e_encode; // if it is encode operation then return_encode
    }
    if (strcmp(argv[1], "-d") == 0) // To check for decoding operation
    {
        return e_decode;  // if it is decode operation then return_decode
    }
    else
    return e_unsupported;  //if it is neither encode nor decode then return unsupported 
}


int main(int argc, char* argv[])  // main function
{
    EncodeInfo encInfo; //structure variable creation for encode info
    DecodeInfo decInfo;  //structure variable creation for decode info

     if (argc < 2) // check for atleast 2 arguments
    {
        printf("Encoding: ./a.out -e <bmp_file> <.txt file> [output file]\n"); //sample message format  for encoding
        printf("Decoding: ./a.out -d <.bmp_file> [output file]\n"); // sample message format for decoding
        return e_failure; 
    }
    if (argc == 3 && (strcmp(argv[1], "-e") == 0)) //check for encoding arguments
    {
        printf("Encoding: ./a.out -e <bmp_file> <.txt file> [output file]\n");
        return e_failure;
    }

   int res = check_operation_type(argv); // function call to check operation type

    if(res == e_encode) //encode
    {
        printf("INFO: Encoding selected\n");
        if(read_and_validate_encode_args(argc,argv,&encInfo) == e_success)
        {
            //Print the success mes
            printf("INFO: Argument validation successful\n");
            do_encoding(&encInfo); // function call for encoding
            //return e_success;
        }
        else
        {
            //print the failure mes
             printf("ERROR: Argument validation failed\n");
            return e_failure;
            
        }
    }
    else if(res == e_decode) //It Handles Decode Operation
        {
            printf("INFO: Decoding Selected\n");
                // do_decoding(&encInfo);
            if(read_and_validate_decode_args(argc,argv,&decInfo) == e_success)
            {
                //Print the success mes
                printf("INFO: Argument validation successful\n");
                do_decoding(&decInfo); //function call for decoding
                //return e_success;
            }
            else
            {
                //print the failure mes
                printf("ERROR: Argument validation failed\n");
                return e_failure;
                
            }
        }
    else  // Handles Unsupported operation
        {
             printf("ERROR: Unsupported operation\n");
                return e_failure;
         }
    return e_success; 
    return 0;
}
