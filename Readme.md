# LSB Steganography in C

## NAME
**JAYAKRISHNA**  
**DATE:** 15-01-2026  

## PROGRAM NAME
LSB Steganography

## AIM OF THE PROJECT
To implement LSB Steganography technique in C language to hide and retrieve secret data within BMP image files.

## WHERE IT IS USED
- This technique is used in secure communications, digital watermarking, and data protection to conceal information within images without noticeable changes.
- It is widely applied in fields such as cybersecurity, digital forensics, and intellectual property protection to ensure data confidentiality and integrity.

## DESCRIPTION
- LSB Steganography is used to securely hide a secret text file inside a BMP image without visible changes.
- It uses bitwise operations and file handling in C to embed and extract hidden data.
- The system supports both encoding and decoding processes through command-line arguments.
- It includes error handling for file operations and validates input parameters.
- The program structure is modular, with separate functions for each task, enhancing readability and maintainability.
- The implementation ensures that the BMP image's header remains unaltered during the encoding process.
- The program checks if the image has sufficient capacity to hold the secret data before encoding.
- The decoding process accurately retrieves the hidden data, reconstructing the original secret file.
- The program handles all edge cases and provides informative error messages.
- The system is designed to be user-friendly, with clear instructions for usage.

## FILES USED
- encode.c  
- decode.c  
- encode.h  
- decode.h  
- common.h  
- types.h  
- test_encode.c  

## ENCODE OPERATIONS
1. Open source BMP image and secret text file.  
2. Validate input arguments and check image capacity.  
3. Copy BMP header to stego image.  
4. Encode magic string, secret file extension, size, and data into the image using LSB technique.  
5. Save the modified image as stego image.  

## DECODE OPERATIONS
1. Open stego BMP image.  
2. Validate input arguments.  
3. Skip BMP header.  
4. Decode and validate magic string.  
5. Decode secret file extension, size, and data from the image.  
6. Save the extracted data to a new secret file.  

## ENCODE USAGE

./a.out -e <bmp_file> <.txt file> [output file]


## DECODE USAGE

./a.out -d <.bmp_file> [output file]


## COMPILATION

- gcc -c *.c
- gcc *.o -o stego 
- otherwise use Makefile, make cmd


## RUN COMMAND

- ./stego -e beautiful.bmp secret.txt steged_img.bmp
- ./stego -d steged_img.bmp decoded.txt


## CONCEPTS USED
- File Handling  
- Bitwise Operations  
- Data Encoding/Decoding  
- Error Handling  

## ENVIRONMENT
- Windows OS  
- GCC Compiler  

## EXAMPLES

### Encoding

- ./stego -e beautiful.bmp secret.txt steged_img.bmp
- (or)
- ./a.out -e beautiful.bmp secret.txt output.bmp


### Decoding

-./stego -d steged_img.bmp decoded.txt
-(or)

-./a.out -d steged_img.bmp decode.txt

