#include <libsystem/Result.h>
#include <libutils/ArgParse.h>
#include <libsystem/io/Stream.h>

struct FileInfo {
  char* filename;
  int bytes;
  int chars;
  int lines;
  int words;
  Result res
};

/*Finding Number of new line characters in a buffer*/
int line_count(const char * str) {
  int liness = 0, i = 0;
  for (i = 0; str[i] != '\0'; i++) {
    if (str[i] == '\n') {
      liness += 1;
    }
  }
  return lines;
}

int word_count(const char * str) {
  int words = 0, i = 0;
  // skip initial white space characters
  while (str[i] != '\0') {
    if (str[i] == ' ' || str[i] == '\n' || str[i] == '\r' || str[i] == '\t') {
      i++;
    } else {
      break;
    }
  }
  for (; str[i] != '\0'; i++) {
    if (str[i] == ' ' || str[i] == '\n' || str[i] == '\r' || str[i] == '\t') {
      words += 1;
      // skip consucative white space
      while (str[i] == ' ' || str[i] == '\n' || str[i] == '\r' || str[i] == '\t') {
        i++;
      }
    }
  }
  return words;
}

void printFileInfo(bool* params, bool all, FileInfo* fl) {
  if(lines || all) {
    stream_format(out_stream, "%d ", fl->lines);
  }
  if(bytes || all) {
    stream_format(out_stream, "%d ", fl->bytes);
  }
  if(words || all) {
    stream_format(out_stream, "%d ", fl->words);
  }
  if(characters || all) {
    stream_format(out_stream, "%d ", fl->chars);
  }

  stream_format(out_stream, "%s\n", fl->filename)
  
}

FileInfo wc(char* path) {
  //Initialization 
  FileInfo to_return;
  to_return.filename = path;

  __cleanup(stream_cleanup) Stream *stream = stream_open(path, OPEN_READ);
  if (handle_has_error(stream)) {
    process_exit(PROCESS_ERROR);
  }

  // Get number of bytes from file

  FileState stat = {};
  stream_stat(stream, &stat);
  to_return.bytes = stat.size;

  char buffer[stat.size];
  size_t read;
  read = stream_read(stream, &buffer, stat.size)
  
  to_return.lines = line_count(buffer);
  to_return.words = word_count(buffer);
  to_return.chars = strlen(buffer);

  return to_return;
}

bool checkArg(char* text, bool* params) {
  if(compare(text, "-c", 2)) {
    params[0] = true;
  } else if (compare(text, "-m", 2)) {
    params[1] = true;
  } else if (compare(text, "-l", 2)) {
    params[2] = true;
  } else if (compare(text, "-w", 2)) {
    params[3] = true;
  } else if (compare(text, "--help", 6)) {
    usage();
    process_exit(PROCESS_SUCCESS);
  } else {
    return false;
  }
  return true;
}

void usage() {
  stream_format("Usage: wc [OPTIONS] files\n");
  stream_format("Options: \n");
  stream_format("\t -m print the character count\n");
  stream_format("\t -c print the bytes count\n");
  stream_format("\t -l print the lines count\n");
  stream_format("\t -w print the words count\n");
}

int main(int argc, char **argv) {
  bool params[4];
  bool all;
  if(argc <= 1) {
    usage();
    process_exit(PROCESS_SUCCESS);
  } else if(argc == 2) {
    all = true;
  }

  for(int c = 0; c < argc; c++) {
    if(!checkArg(argv[c], params)) {
      printFileInfo(params, all, wc(argv[c]));
    }
  }

  return PROCESS_SUCCESS;
}

/*Finding Number of new line characters in a buffer*/
int line_count(const char * str) {
  int liness = 0, i = 0;
  for (i = 0; str[i] != '\0'; i++) {
    if (str[i] == '\n') {
      liness += 1;
    }
  }
  return liness;
}

int word_count(const char * str) {
  int words = 0, i = 0;
  // skip initial white space characters
  while (str[i] != '\0') {
    if (str[i] == ' ' || str[i] == '\n' || str[i] == '\r' || str[i] == '\t') {
      i++;
    } else {
      break;
    }
  }
  for (; str[i] != '\0'; i++) {
    if (str[i] == ' ' || str[i] == '\n' || str[i] == '\r' || str[i] == '\t') {
      words += 1;
      // skip consucative white space
      while (str[i] == ' ' || str[i] == '\n' || str[i] == '\r' || str[i] == '\t') {
        i++;
      }
    }
  }
  return words;
}