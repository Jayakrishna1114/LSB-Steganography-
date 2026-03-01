#include <stdio.h>
#include<string.h>
#include"common.h"
#include "encode.h"
#include "types.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)// func to get image size of bmp
{
    uint width, height;
    // Seek and set fptr to  18th byte to store bmp image
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);
    // 1 pixel = 3 bytes (R+G+B)

    // Return image capacity
    return width * height * 3; //this is bytes of  data
}

/* Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_encode_files(EncodeInfo *encInfo)// function to open encode files
{   
    
    // Src Image file
    encInfo -> fptr_src_image = fopen(encInfo->src_image_fname, "rb"); // reading SRC image file in binary mode
    //Do Error handling
    if (encInfo->fptr_src_image == NULL) //if file not opened
    {
    	perror("fopen");  
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname); // print the error message 
    	return e_failure;
    }
    else
    {
       printf("INFO: Opened beautiful.bmp\n");
    }

    // Secret file (Open secret.txt and store its fptr into fptr secret)
    encInfo -> fptr_secret = fopen(encInfo->secret_fname, "r"); //reading secret file in read mode
    //Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname); // print the error message  
    	return e_failure;
    }
    else
    {
        printf("INFO: Opened secret.txt\n");
    }

    // Stego Image file
    encInfo -> fptr_stego_image = fopen(encInfo->stego_image_fname, "wb"); //writing stego image file in binary mode
    //Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
    	return e_failure;
    }
    else
    {
        printf("INFO: Opened steged_img.bmp\n");
    }

    // No failure return e_success
    return e_success;
}


Status read_and_validate_encode_args(int argc,char *argv[], EncodeInfo *encInfo) // function to read and validate encode arguments
{
    /*
    STEP-1 : Check the argc is less than 4 and greater than 5
        yes->
            return e_failure;

    STEP-2 : check the argv[2] is having ".bmp" or not
        yes-> 
            Store the argv[2] to the encInfo ->src_image_fname
        No-> 
            return e_failure;
    
    STEP-3 : Check the argv[3] is having '.' or not 
        yes->
            Store the argv[3] to the encInfo ->secret_fname
        No->
            return e_failure;

    STEP-4 : Check the argv[4] is NULL or not
        yes-> NULL
            Store the "output.bmp" to the encInfo ->stego_image_fname
        No -> 
            check the new created default name conatins argv[4] is having ".bmp" or not
            yes-> 
                Store the argv[4] to the encInfo ->stego_image_fname
            No-> 
                return e_failure;

    return e_success;
    */
    
    if(argc < 4 || argc > 5)
    {
        return e_failure; // returning failure if the condition is  false
    }
    if(!(strstr(argv[2], ".bmp"))) // check for .bmp in argv[2]
    {
        return e_failure;
    }
    else
    {
        encInfo -> src_image_fname = argv[2]; //Store the "output.bmp" to the encInfo ->stego_image_fname
          printf("src_img_fname stored\n");
    }
    char *extn = strchr(argv[3],'.'); //check for . in argv[3]
    if(extn == NULL)
    {
        return e_failure;
    }
    else
    {
        encInfo -> secret_fname = argv[3]; //Store the argv[3] to the encInfo ->secret_fname
        printf("secret file name stored\n");
         //store .txt in extn_secret_file
        strncpy(encInfo->extn_secret_file, extn, MAX_FILE_SUFFIX - 1); // copying extension name from argv[2] to extn_secret_file
        encInfo->extn_secret_file[MAX_FILE_SUFFIX - 1] = '\0'; // terminating with null character
        printf("secret file extension name stored\n");
        // encInfo -> extn_secret_file = strchr(argv[3],'.');
    }
    if (argc == 4)
    {
        printf("INFO: Output file not mentioed. Creating steged_img.bmp as default");
        encInfo->stego_image_fname = "steged_img.bmp";  //store output file name in stego_imag_fname if not given create default file and store
        printf("Default Output file name created\n");
    }
    else
    {
        /* argv[4] is given */
        if (strstr(argv[4], ".bmp") == NULL) // check for .bmp in argv[4]
        {
            return e_failure;
        }
        else
        {
            //store output file name in stego_imag_fname if given
            encInfo->stego_image_fname = argv[4]; // to store argv[4] to stego_image_fname file
             printf("Image file name stored\n");
            return e_success;
        }
    }
    return e_success;
}

uint get_file_size(FILE *fptr) // function to get file size
{
    /*
        -> Use fseek & ftell 
        -> Use fseek to move the file pointer to end 
        -> Use ftell to find the pos of the file pointer 
        -> finally return the pos
     */
    fseek(fptr, 0, SEEK_END);
    //uint size = ftell(fptr);
    return ftell(fptr);;

}

Status check_capacity(EncodeInfo *encInfo) // function to check capacity
{
    //store image size in image_capacity variable (1024 * 768 - sizeof(beautiful.bmp))
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);

    //store secret file size in size_secret_file variable (24 bytes - sizeof(secret.txt))
    encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);

    
   // (secret file size(24) + secret file extension size[.txt](4) + Magic string size[#*](2) + image header(54))
   // 54 + 2 + 4 + 24 = 84 , image size should be greater than 84*8 = 672 bytes 
   if(encInfo -> size_secret_file * 8 > encInfo->image_capacity)
   {
        printf("ERROR: Secret file is too large to encode in this image\n");
        return e_failure;
   }
   return e_success;

}

Status copy_bmp_header(FILE * fptr_src_image, FILE *fptr_dest_image) // function to copy bmp header
{  
    //copy 54 bytes of bmp without touching it
    //create buffer(temporary storage) to use as a medium

    char buffer[54]; //to store header
    printf("INFO: Copying image header\n");
    rewind(fptr_src_image);//set bmp image fptr to 0 to start copying data. it is at file ending. you put it there to find its size
    //read 54 bytes and store in buffer
    if(fread(buffer, sizeof(buffer), 1, fptr_src_image) == 0)
    {
        printf("INFO: Error Copying image header\n");
        return e_failure;
    }
    else
    {
        //set again to 0 , just for fun, set stego bmp to 0 also
        rewind(fptr_src_image);
        //store this 54 bytes into output stego bmp file without touching it
        if(fwrite(buffer, sizeof(buffer), 1, fptr_dest_image) == 0)
        {
            printf("INFO: Error Copying image header\n");
            return e_failure;
        }
        else
        {
            printf("INFO: Done\n");  
            return e_success;
        }
    }  
    
    /*
    Step - 1 : Read a 54 bytes from fptr_src_image & store the data into buffer
    Step - 2 : Write a 54 bytes of buffer into the fptr_dest_image

    return e_success;
    */

}

Status encode_byte_to_lsb(char data, char *image_buffer) // function to encode byte to lsb
{
   /*
   for(8 times)
   {
     Step 1: Get the bit from data (MSB to LSB)
     Step 2: Clear the LSB bit of arr[i]
     Step 3: Set the getted bit into arr[i] at LSB
   }
     return e_success;
   */
   for(int i = 0; i < 8; i++) // loop for 8 bits
    {
        int x = (data >> (7 - i)) & 1; // get each bit from data
        image_buffer[i] = x | (image_buffer[i] & ~1); // clear LSB of image buffer and set the getted bit into LSB
    }
    return e_success;
}

Status encode_data_to_image( const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image) // function to encode data to image
{
    char image_buffer[8];  //create 8 bit buffer to get each bit of char 

    //fread(image_buffer, sizeof(image_buffer), 1, fptr_src_image);
    for(int i = 0; i < size; i++)
    {
     /*
         Step 1 : Read 8 bytes from the source file (fptr_src_image) & store the data into buffer
         Step 2 : encode_byte_to_lsb(data[i], buffer);
         Step 3 : Write 8 bytes into the fptr_stego_image
     */
        //read each bit of data from source
        if (fread(image_buffer, 8, 1, fptr_src_image) != 1) // read 8 bytes from source image to buffer
            return e_failure;

        //store into buffer
        if (encode_byte_to_lsb(data[i], image_buffer) == e_failure) // encode each byte of data into buffer
            return e_failure;

        // fptr_stego_image = encode_byte_to_lsb(data[i], image_buffer);
        //again write it into stego image from buffer 
        if (fwrite(image_buffer, 8, 1, fptr_stego_image) != 1) // write 8 bytes from buffer to stego image
           return e_failure;
    
    }
    return e_success;
    
}



Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo) // function to encode magic string
{
    printf("INFO : Encoding Magic String Signature\n");
    //store #* (2 bytes *8 = 16 bytes ) into stego bmp bit by bit

  if(encode_data_to_image(magic_string, strlen(magic_string), 
                                    encInfo -> fptr_src_image, 
                                    encInfo -> fptr_stego_image) == e_success) //function call to encode data to image
        return e_success;
  else
  {
       printf("INFO: Error while Encoding magic string ");
        return e_failure;
  }

}

//Status encode_size_to_lsb(int data, char *image_buffer,EncodeInfo  *encInfo)
Status encode_size_to_lsb(int data, EncodeInfo  *encInfo) // function to encode size to lsb


{
    /*
    for(32 times)
   {
     Step 1: Get the bit from data (MSB to LSB)
     Step 2: Clear the LSB bit of arr[i]
     Step 3: Set the getted bit into arr[i] at LSB
   }
     return e_success;
   */
   char image_buffer[32];
    // Read 32 bytes from source image to buffer
    if (fread(image_buffer, 32, 1, encInfo -> fptr_src_image) != 1)  
    {
        return e_failure;
    }
    // Encode 32 bits of integer into 32 image bytes
    for (int i = 0; i < 32; i++)
    {
        int bit = (data >> (31 - i)) & 1; //get each bit from data 
        image_buffer[i] = (image_buffer[i] & ~1) | bit; // clear LSB of image buffer and set the getted bit into LSB
    }
    // Write modified bytes to stego image
    if (fwrite(image_buffer, 32, 1, encInfo-> fptr_stego_image) != 1) // write 32 bytes from buffer to stego image
    {
        return e_failure;
    }
    return e_success;

}

Status encode_secret_file_extn_size(long extn_file_size, EncodeInfo *encInfo) // function to encode extension size

{  
    //  char buffer[32];
    // /*
    //      Step 1 : Read 32 bytes from the source file (fptr_src_image) & store the data into buffer
    //      Step 2 : encode_size_to_lsb(extn_file_size, buffer);
    //      Step 3 : Write 32 bytes into the fptr_stego_image
    //              return e_success;
    //  */

    printf("INFO : Encoding secret file extension size\n");
    if(encode_size_to_lsb(extn_file_size, encInfo) == e_failure) // function call to encode size to lsb
    {
        return e_failure;
    }
    return e_success;

}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo) // function to encode extension name
{
    // Logic to store extension name second and then data.(first is magic string)
    printf("INFO: Encoding secret file extension\n");

    // int size = strlen(file_extn); 
    // // fread(buffer,sizeof(buffer),1,file_extn);
    // for(int i = 0;i < size;i++)
    // {
    //     if (encode_byte_to_lsb(file_extn[i], encInfo->image_data) == e_failure)
    //     {
    //         return e_failure;
    //     }
    //     // fptr_stego_image = encode_byte_to_lsb(file_extn[i], buffer);
    // }
    // return e_success;
    /*
    encode_data_to_image(file_extn,strlen(file_extn), encInfo -> fptr_src_image, encInfo -> fptr_stego_image);
        return e_success;
    */

   return encode_data_to_image(file_extn,strlen(file_extn), 
                                    encInfo -> fptr_src_image, 
                                    encInfo -> fptr_stego_image);  // function call to encode data to image
        return e_success;
}


Status encode_secret_file_size(long file_size, EncodeInfo *encInfo) // function to encode secret file size
{
   //char buffer[32];
   printf("INFO : Encoding secret file extension file size\n");

   if(encode_size_to_lsb(file_size, encInfo) == e_success) // function call to encode size to LSB
        return e_success;
    else    
        return e_failure;
    
}

Status encode_secret_file_data(EncodeInfo *encInfo) // function to encode secret file data
{
    //char buffer[encInfo -> size_secret_file];
    /*
    Step - 1: Read a eccInfo -> size_secret_file bytes from encInfo -> fptr_secret

    enocode_data_to_image(buffer,encInfo -> size_secret_file, encInfo -> fptr_src_image, encInfo -> fptr_stego_image);
    */

    printf("INFO : Encoding secret file data\n");

    rewind(encInfo -> fptr_secret);
    char ch;

   while (fread(&ch, 1, 1, encInfo->fptr_secret) == 1)
    {
        if (encode_data_to_image(&ch, 1,
            encInfo->fptr_src_image,
            encInfo->fptr_stego_image) == e_failure)
        {
            return e_failure;
        }
    }
     return e_success; 
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_stego) // function to copy remaining image data 
{
     /*
    loop(EOF)
    {
      1. Read 1 byte from fptr_src & store to buffer
      2. Write 1 byte of bufert into fptr_dest
    }
    */
   //
    printf("INFO : Copying remaining image data\n");
    char ch; // temporary storage 
    while(fread(&ch, 1, 1, fptr_src) == 1) // read each byte from source image to end of file
    {
        fwrite(&ch, 1, 1, fptr_stego); // write each byte to stego image
    }
    return e_success;
    
   
}

Status do_encoding(EncodeInfo *encInfo) //only function call and print the message
{
    printf("INFO: ## Encoding Procedure Started ##\n");

    if(open_encode_files(encInfo) == e_success) // encInfo is structure so need not to pass address
    {
        //print the success mes
        //printf("Open files  successful\n"); 
        printf("INFO: Done. Not Empty\n");

    }
    else
    {
        //print the failure mes
        return e_failure;
    }
    //check the image capacity whether it is greater than 672 or not
    check_capacity(encInfo);

    printf("INFO: Checking for beautiful.bmp capacity to handle secret.txt\n");
    printf("INFO: Done. Found OK\n");
 

    //print the sucess and failure messages
     
      //copy header data safely without modifying it
      //printf("INFO: Copying Image Header\n");

     copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image);

     //printf("INFO: Done\n");


     //encode(store each bit of magic string into 1 byte of stego image after 54 bytes)[#* = 2 bytes * 8 = 16 bytes of magic string into stego image from 55th byte]

     //printf("INFO: Encoding Magic String Signature\n");

      encode_magic_string(MAGIC_STRING,encInfo);

      printf("INFO: Done\n");


     encode_secret_file_extn_size(strlen(encInfo -> extn_secret_file),encInfo);
         printf("INFO: Done\n");
     //encode([encode]store extension name into stego image .txt(4 bytes * 8 = 32 bytes into stego image))
    printf("INFO: Encoding secret.txt File Extension\n");
       

     encode_secret_file_extn(encInfo -> extn_secret_file, encInfo);
    
     printf("INFO: Done\n");

     printf("INFO: Encoding secret.txt File Size\n");

     encode_secret_file_size(encInfo -> size_secret_file, encInfo);
     printf("INFO: Done\n");
     
     //printf("INFO: Encoding secret.txt File Data\n");
     encode_secret_file_data(encInfo);
     printf("INFO: Done\n");

   // printf("INFO: Copying Left Over Data\n");

     copy_remaining_img_data(encInfo -> fptr_src_image,encInfo -> fptr_stego_image);
    printf("INFO: Done\n");

    printf("INFO: ## Encoding Done Successfully ##\n");
    
    //close all files
    fclose(encInfo -> fptr_src_image);
    fclose(encInfo -> fptr_secret);
    fclose(encInfo -> fptr_stego_image);

    return e_success; 


}