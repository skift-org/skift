#include <libsystem/Logger.h>
#include <libsystem/Result.h>
#include <libsystem/cmdline/CMDLine.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/Directory.h>
#include <libsystem/io/Stream.h>

int total_line=0;
int total_words=0;
int total_character=0;
int flag=0;

Result _wc(const char *path,const char *n) //this function is implemented for switches
{
																						
int line=0;
int words=0;
int character=0;

   // Initialize result
  
    __cleanup(stream_cleanup) Stream *stream = stream_open(path, OPEN_READ);

    if (handle_has_error(stream))
    {
        return handle_get_error(stream);
    }

    FileState stat = {};
    stream_stat(stream, &stat);

    size_t read;
    char buffer[1];
    char buffer1[1]={'a'};  //Initializing 

    while ((read = stream_read(stream, &buffer, 1)) != 0)
    {
        if (handle_has_error(stream))
        {
            return handle_get_error(stream);
        }
        
        if(buffer[0]==buffer1[0]){
        	if(buffer[0]=='\n'){
               	words--;
                }
        }
        
        buffer1[0]=buffer[0];
        
        if(buffer[0]=='\n'){
        	line++;
        }
        if(buffer[0]==' ' || buffer[0]=='\n'){
        	words++;
        }
        character++;
                    
        if (handle_has_error(out_stream))
        {
            return ERR_WRITE_STDOUT;
        }
          
    }
	total_line+=line;
	total_words+=words;
	total_character+=character;

	if(strcmp(n,"-l")==0){
	printf("Lines = %d",line);
	}
	
	if(strcmp(n,"-w")==0){	
	printf("Words = %d",words);
	}
	
	if(strcmp(n,"-m")==0){
	printf("Characters = %d",character);
	}	

    stream_flush(out_stream);
    return SUCCESS;
}

Result wc(const char *path)
{
											
int line=0;
int words=0;
int character=0;

   // Initialize result
  
    __cleanup(stream_cleanup) Stream *stream = stream_open(path, OPEN_READ);

    if (handle_has_error(stream))
    {
        return handle_get_error(stream);
    }

    FileState stat = {};
    stream_stat(stream, &stat);

    size_t read;
    char buffer[1];
    char buffer1[1]={'a'};

    while ((read = stream_read(stream, &buffer, 1)) != 0)
    {
        if (handle_has_error(stream))
        {
            return handle_get_error(stream);
        }
        
        if(buffer[0]==buffer1[0]){
        	if(buffer[0]=='\n'){
               	words--;
                }
        }
        
        buffer1[0]=buffer[0];
        
        
        if(buffer[0]=='\n'){
        	line++;
        }
        if(buffer[0]==' ' || buffer[0]=='\n'){
        	words++;
        }
        character++;
                  
          
        if (handle_has_error(out_stream))
        {
            return ERR_WRITE_STDOUT;
        }
        
        
    }
total_line+=line;
total_words+=words;
total_character+=character;
printf("Lines = %d",line);
printf("\nWords = %d",words);
printf("\nCharacters = %d",character);
    
    stream_flush(out_stream);

    return SUCCESS;
}


int main(int argc, char **argv)
{
    if (argc == 1)
    {
        /* TODO: stdin option */
        return PROCESS_SUCCESS;
    }

    Result result;
    int exit_code = PROCESS_SUCCESS;
    
    int sw = 0;
    if(strcmp(argv[1],"-l")==0 || strcmp(argv[1],"-w")==0 || strcmp(argv[1],"-m")==0){
    	sw=1;
    }                                  //wc -l file1 file2
    
   if(sw==0){ 
     for (int i = 1; i < argc; i++)
    {
    	result = wc(argv[i]);	
	
	
        if (result != SUCCESS)
        {
            stream_format(err_stream, "%s: %s: %s", argv[0], argv[i], get_result_description(result));
            exit_code = PROCESS_FAILURE;
        }
        printf("\n");
         flag++;         
    }
    if(flag>1){    
    	printf("Total Line=%d",total_line);
    	printf("\tTotal Words=%d",total_words);
    	printf("\tTotal Character=%d",total_character);
    	
    
    }
   }
   
    
 if(sw==1){   
   for (int i = 2; i < argc; i++)
    {
    	result = _wc(argv[i],argv[1]);	
	
	
        if (result != SUCCESS)
        {
            stream_format(err_stream, "%s: %s: %s", argv[0], argv[i], get_result_description(result));
            exit_code = PROCESS_FAILURE;
        }
                printf("\n");
                flag++;
    }
    if(flag>1){
    	if(strcmp(argv[1],"-l")==0){
		printf("Total Lines = %d",total_line);
	}
	if(strcmp(argv[1],"-w")==0){
		printf("Total Words = %d",total_words);
	}
	if(strcmp(argv[1],"-m")==0){
		printf("Total Characters = %d",total_character);
    	}
    }
    
 
}

  
    return exit_code;
}








