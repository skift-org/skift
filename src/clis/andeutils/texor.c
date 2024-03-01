/*
*	Copyright (C) 2017 Kyle Tolle
*	Copyright (C) 2023, 2024 AnatoliyL
* This program is free software: you can redistribute it 
* and/or modify it under the terms of the BSD 2-Clause License.
* See COPYING.bsd2 for more details.
*/
/*** includes ***/

#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

/*** defines ***/

#define TEXOR_VERSION "0.0.2"
#define TEXOR_TAB_STOP 2
#define TEXOR_QUIT_TIMES 3

#define CTRL_KEY(k) ((k) & 0x1f)

enum editorKey {
  BACKSPACE = 127,
  ARROW_LEFT = 1000,
  ARROW_RIGHT,
  ARROW_UP,
  ARROW_DOWN,
  DEL_KEY,
  HOME_KEY,
  END_KEY,
  PAGE_UP,
  PAGE_DOWN
};

enum editorHighlight {
  HL_NORMAL = 0,
  HL_COMMENT,
  HL_MLCOMMENT,
  HL_KEYWORD1,
  HL_KEYWORD2,
  HL_STRING,
  HL_NUMBER,
  HL_MATCH
};

#define HL_HIGHLIGHT_NUMBERS (1<<0)
#define HL_HIGHLIGHT_STRINGS (1<<1)

/*** data ***/

struct editorSyntax {
  char *file_type;
  char **file_match;
  char **keywords;
  char *singleline_comment_start;
  char *multiline_comment_start;
  char *multiline_comment_end;
  int flags;
};

typedef struct erow {
  int index;
  int size;
  int rendered_size;
  char *characters;
  char *rendered_characters;
  unsigned char *highlight;
  int highlight_open_comment;
} erow;

struct editorConfig {
  int file_position_x, file_position_y;
  int screen_position_x;
  int row_offset;
  int column_offset;
  int screen_rows;
  int screen_columns;
  int number_of_rows;
  erow *row;
  int dirty;
  char *filename;
  char status_message[80];
  time_t status_message_time;
  struct editorSyntax *syntax;
  struct termios orig_termios;
};

struct editorConfig E;

/*** filetypes ***/

char *C_HL_extensions[] = { ".c", ".h", ".cpp", ".hpp", NULL };
char *C_HL_keywords[] = {
  "switch", "if", "while", "for", "break", "continue", "return", "else",
  "struct", "union", "typedef", "static", "enum", "class", "case",
  "int|", "long|", "double|", "float|", "char|", "unsigned|", "signed|",
  "void|", "FILE|", "__asm__", "asm", "volatile", "_Bool", "bool", NULL
};

char *RUBY_HL_extensions[] = { ".rb", NULL };
char *RUBY_HL_keywords[] = {
  "__ENCODING__", "__LINE__", "__FILE__", "BEGIN", "END", "alias", "and",
  "begin", "break", "case", "class", "def", "defined?", "do", "else", "elsif",
  "end", "ensure", "false", "for", "if", "in", "module", "next", "nil", "not",
  "or", "redo", "rescue", "retry", "return", "self", "super", "then", "true",
  "undef", "unless", "until", "when", "while", "yield", NULL
};

struct editorSyntax HLDB[] = {
  {
    "c",
    C_HL_extensions,
    C_HL_keywords,
    "//", "/*", "*/",
    HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
  },
  {
    "ruby",
    RUBY_HL_extensions,
    RUBY_HL_keywords,
    "#", "=begin", "=end",
    HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
  },
};

# define HLDB_ENTRIES (sizeof(HLDB) / sizeof(HLDB[0]))

/*** prototypes ***/

void editorSetStatusMessage(const char *fmt, ...);
void editorRefreshScreen();
char *editorPrompt(char *prompt, void (*callback)(char *, int));

/*** terminal ***/

void die(const char *s) {
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);
  perror(s);
  exit(1);
}

void disableRawMode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
    die("tcsetattr");
}

void enableRawMode() {
  if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) die("tcgetattr");
  atexit(disableRawMode);

  struct termios raw = E.orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

int editorReadKey() {
  int nread;
  char c;
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN) die("read");
  }

  if (c == '\x1b') {
    char seq[3];

    if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
    if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';

    if (seq[0] == '[') {
      if (seq[1] >= '0' && seq[1] <= '9') {
        if (read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
        if (seq[2] == '~') {
          switch (seq[1]) {
            case '1': return HOME_KEY;
            case '3': return DEL_KEY;
            case '4': return END_KEY;
            case '5': return PAGE_UP;
            case '6': return PAGE_DOWN;
            case '7': return HOME_KEY;
            case '8': return END_KEY;
          }
        }
      } else {
        switch (seq[1]) {
          case 'A': return ARROW_UP;
          case 'B': return ARROW_DOWN;
          case 'C': return ARROW_RIGHT;
          case 'D': return ARROW_LEFT;
          case 'H': return HOME_KEY;
          case 'F': return END_KEY;
        }
      }
    } else if (seq[0] == '0') {
      switch (seq[1]) {
        case 'H': return HOME_KEY;
        case 'F': return END_KEY;
      }
    }

    return '\x1b';
  } else {
    return c;
  }
}

int getCursorPosition(int *rows, int *cols) {
  char buf[32];
  unsigned int i = 0;

  if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;

  while (i < sizeof(buf) - 1) {
    if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
    if (buf[i] == 'R') break;
    i++;
  }
  buf[i] = '\0';

  if (buf[0] != '\x1b' || buf[1] != '[') return -1;
  if (sscanf(&buf[2], "%d;%d", rows, cols) != 2) return -1;

  return 0;
}

int getWindowSize(int *rows, int *cols) {
  struct winsize ws;

  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1;
    return getCursorPosition(rows, cols);
  } else {
    *cols = ws.ws_col;
    *rows = ws.ws_row;
    return 0;
  }
}

/*** syntax highlighting ***/

int is_separator(int c) {
  return isspace(c) || c == '\0' || strchr(",.()+-/*=~%<>[];", c) != NULL;
}

void editorUpdateSyntax(erow *row) {
  row->highlight = realloc(row->highlight, row->rendered_size);
  memset(row->highlight, HL_NORMAL, row->rendered_size);

  if (E.syntax == NULL) return;

  char **keywords = E.syntax->keywords;

  char *singleline_comment_start = E.syntax->singleline_comment_start;
  char *multiline_comment_start = E.syntax->multiline_comment_start;
  char *multiline_comment_end = E.syntax->multiline_comment_end;

  int singleline_comment_start_length = singleline_comment_start ? strlen(singleline_comment_start) : 0;
  int multiline_comment_start_length = multiline_comment_start ? strlen(multiline_comment_start) : 0;
  int multiline_comment_end_length = multiline_comment_end ? strlen(multiline_comment_end) : 0;

  int previous_separator = 1;
  int in_string = 0;
  int in_comment = (row->index > 0 && E.row[row->index - 1].highlight_open_comment);

  int i = 0;
  while (i < row->rendered_size) {
    char c = row->rendered_characters[i];
    unsigned char prev_highlight = (i > 0) ? row->highlight[i - 1] : HL_NORMAL;

    if (singleline_comment_start_length && !in_string && !in_comment) {
      if (!strncmp(&row->rendered_characters[i], singleline_comment_start, singleline_comment_start_length)) {
        memset(&row->highlight[i], HL_COMMENT, row->rendered_size - i);
        break;
      }
    }

    if (multiline_comment_start_length && multiline_comment_end_length && !in_string) {
      if (in_comment) {
        row->highlight[i] = HL_MLCOMMENT;
        if (!strncmp(&row->rendered_characters[i], multiline_comment_end, multiline_comment_end_length)) {
          memset(&row->highlight[i], HL_MLCOMMENT, multiline_comment_end_length);
          i += multiline_comment_end_length;
          in_comment = 0;
          previous_separator = 1;
          continue;
        } else {
          i++;
          continue;
        }
      } else if (!strncmp(&row->rendered_characters[i], multiline_comment_start, multiline_comment_start_length)) {
        memset(&row->highlight[i], HL_MLCOMMENT, multiline_comment_start_length);
        i += multiline_comment_start_length;
        in_comment = 1;
        continue;
      }
    }

    if (E.syntax->flags & HL_HIGHLIGHT_STRINGS) {
      if (in_string) {
        row->highlight[i] = HL_STRING;
        if (c == '\\' && i + 1 < row->rendered_size) {
          row->highlight[i + 1] = HL_STRING;
          i += 2;
          continue;
        }
        if (c == in_string) in_string = 0;
        i++;
        previous_separator = 1;
        continue;
      } else {
        if (c == '"' || c == '\'') {
          in_string = c;
          row->highlight[i] = HL_STRING;
          i++;
          continue;
        }
      }
    }

    if (E.syntax->flags & HL_HIGHLIGHT_NUMBERS) {
      if ((isdigit(c) && (previous_separator || prev_highlight == HL_NUMBER)) || (c == '.' && prev_highlight == HL_NUMBER)) {
        row->highlight[i] = HL_NUMBER;
        i++;
        previous_separator = 0;
        continue;
      }
    }

    if (previous_separator) {
      int j;
      for (j = 0; keywords[j]; j++) {
        int klen = strlen(keywords[j]);
        int kw2 = keywords[j][klen - 1] == '|';
        if (kw2) klen--;

        if (!strncmp(&row->rendered_characters[i], keywords[j], klen) &&
            is_separator(row->rendered_characters[i + klen])) {
          memset(&row->highlight[i], kw2 ? HL_KEYWORD2 : HL_KEYWORD1, klen);
          i += klen;
          break;
        }
      }
      if (keywords[j] != NULL) {
        previous_separator = 0;
        continue;
      }
    }

    previous_separator = is_separator(c);
    i++;
  }

  int changed = (row->highlight_open_comment != in_comment);
  row->highlight_open_comment = in_comment;
  if (changed && row->index + 1 < E.number_of_rows)
    editorUpdateSyntax(&E.row[row->index + 1]);
}

int editorSyntaxToColor(int hl) {
  switch (hl) {
    case HL_COMMENT:
    case HL_MLCOMMENT: return 36;
    case HL_KEYWORD1: return 33;
    case HL_KEYWORD2: return 32;
    case HL_STRING: return 35;
    case HL_NUMBER: return 31;
    case HL_MATCH: return 34;
    default: return 37;
  }
}

void editorSelectSyntaxHighlight() {
  E.syntax = NULL;
  if (E.filename == NULL) return;

  char *ext = strrchr(E.filename, '.');

  for (unsigned int j = 0; j < HLDB_ENTRIES; j++) {
    struct editorSyntax *s = &HLDB[j];
    unsigned int i = 0;
    while (s->file_match[i]) {
      int is_ext = (s->file_match[i][0] == '.');
      if ((is_ext && ext && !strcmp(ext, s->file_match[i])) ||
          (!is_ext && strstr(E.filename, s->file_match[i]))) {
        E.syntax = s;

        int filerow;
        for (filerow = 0; filerow < E.number_of_rows; filerow++) {
          editorUpdateSyntax(&E.row[filerow]);
        }

        return;
      }
      i++;
    }
  }
}

/*** row operations ***/

int editorRowFilePositionXToScreenPositionX(erow *row, int file_position_x) {
  int screen_position_x = 0;
  int j;
  for (j = 0; j < file_position_x; j++) {
    if (row->characters[j] == '\t')
      screen_position_x += (TEXOR_TAB_STOP - 1) - (screen_position_x % TEXOR_TAB_STOP);
    screen_position_x++;
  }
  return screen_position_x;
}

int editorRowScreenPositionXToFilePositionX(erow *row, int screen_position_x) {
  int cur_screen_position_x = 0;
  int file_position_x;
  for (file_position_x = 0; file_position_x < row->size; file_position_x++) {
    if (row->characters[file_position_x] == '\t')
      cur_screen_position_x += (TEXOR_TAB_STOP - 1) - (cur_screen_position_x % TEXOR_TAB_STOP);
    cur_screen_position_x++;

    if (cur_screen_position_x > screen_position_x) return file_position_x;
  }
  return file_position_x;
}

void editorUpdateRow(erow *row) {
  int tabs = 0;
  int j;
  for (j = 0; j < row->size; j++)
    if (row->characters[j] == '\t') tabs++;

  free(row->rendered_characters);
  row->rendered_characters = malloc(row->size + tabs*(TEXOR_TAB_STOP - 1) + 1);

  int index = 0;
  for (j = 0; j < row->size; j++) {
    if (row->characters[j] == '\t') {
      row->rendered_characters[index++] = ' ';
      while (index % TEXOR_TAB_STOP != 0) row->rendered_characters[index++] = ' ';
    } else {
      row->rendered_characters[index++] = row->characters[j];
    }
  }
  row->rendered_characters[index] = '\0';
  row->rendered_size = index;

  editorUpdateSyntax(row);
}

void editorInsertRow(int at, char *s, size_t len) {
  if (at < 0 || at > E.number_of_rows) return;

  E.row = realloc(E.row, sizeof(erow) * (E.number_of_rows + 1));
  memmove(&E.row[at + 1], &E.row[at], sizeof(erow) * (E.number_of_rows - at));
  for (int j = at + 1; j <= E.number_of_rows; j++) E.row[j].index++;

  E.row[at].index = at;

  E.row[at].size = len;
  E.row[at].characters = malloc(len + 1);
  memcpy(E.row[at].characters, s, len);
  E.row[at].characters[len] = '\0';

  E.row[at].rendered_size = 0;
  E.row[at].rendered_characters = NULL;
  E.row[at].highlight = NULL;
  E.row[at].highlight_open_comment = 0;
  editorUpdateRow(&E.row[at]);

  E.number_of_rows++;
  E.dirty++;
}

void editorFreeRow(erow *row) {
  free(row->rendered_characters);
  free(row->characters);
  free(row->highlight);
}

void editorDelRow(int at) {
  if (at < 0 || at >= E.number_of_rows) return;
  editorFreeRow(&E.row[at]);
  memmove(&E.row[at], &E.row[at + 1], sizeof(erow) * (E.number_of_rows - at - 1));
  for (int j = at; j < E.number_of_rows - 1; j++) E.row[j].index--;
  E.number_of_rows--;
  E.dirty++;
}

void editorRowInsertChar(erow *row, int at, int c) {
  if (at < 0 || at > row->size) at = row->size;
  row->characters = realloc(row->characters, row->size + 2);
  memmove(&row->characters[at + 1], &row->characters[at], row->size - at + 1);
  row->size++;
  row->characters[at] = c;
  editorUpdateRow(row);
  E.dirty++;
}

void editorRowDelChar(erow *row, int at) {
  if (at < 0 || at >= row->size) return;
  memmove(&row->characters[at], &row->characters[at + 1], row->size - at);
  row->size--;
  editorUpdateRow(row);
  E.dirty++;
}

/*** editor operations ***/

void editorInsertChar(int c) {
  if (E.file_position_y == E.number_of_rows) {
    editorInsertRow(E.number_of_rows, "", 0);
  }
  editorRowInsertChar(&E.row[E.file_position_y], E.file_position_x, c);
  E.file_position_x++;
}

void editorRowAppendString(erow *row, char *s, size_t len) {
  row->characters = realloc(row->characters, row->size + len + 1);
  memcpy(&row->characters[row->size], s, len);
  row->size += len;
  row->characters[row->size] = '\0';
  editorUpdateRow(row);
  E.dirty++;
}

void editorInsertNewline() {
  if (E.file_position_x == 0) {
    editorInsertRow(E.file_position_y, "", 0);
  } else {
    erow *row = &E.row[E.file_position_y];
    editorInsertRow(E.file_position_y + 1, &row->characters[E.file_position_x], row->size - E.file_position_x);
    row = &E.row[E.file_position_y];
    row->size = E.file_position_x;
    row->characters[row->size] = '\0';
    editorUpdateRow(row);
  }
  E.file_position_y++;
  E.file_position_x = 0;
}

void editorDelChar() {
  if (E.file_position_y == E.number_of_rows) return;
  if (E.file_position_x == 0 && E.file_position_y == 0) return;

  erow *row = &E.row[E.file_position_y];
  if (E.file_position_x > 0) {
    editorRowDelChar(row, E.file_position_x - 1);
    E.file_position_x--;
  } else {
    E.file_position_x = E.row[E.file_position_y - 1].size;
    editorRowAppendString(&E.row[E.file_position_y - 1], row->characters, row->size);
    editorDelRow(E.file_position_y);
    E.file_position_y--;
  }
}

/*** file i/o ***/

char *editorRowsToString(int *buflen) {
  int totlen = 0;
  int j;
  for (j = 0; j < E.number_of_rows; j++)
    totlen += E.row[j].size + 1;
  *buflen = totlen;

  char *buf = malloc(totlen);
  char *p = buf;
  for (j = 0; j < E.number_of_rows; j++) {
    memcpy(p, E.row[j].characters, E.row[j].size);
    p += E.row[j].size;
    *p = '\n';
    p++;
  }

  return buf;
}

void editorOpen(char *filename) {
  free(E.filename);
  E.filename = strdup(filename);

  editorSelectSyntaxHighlight();

  FILE *fp = fopen(filename, "r");
  if (!fp) die("fopen");

  char *line = NULL;
  size_t linecap = 0;
  ssize_t linelen;
  while((linelen = getline(&line, &linecap, fp)) != -1) {
    while(linelen > 0 && (line[linelen - 1] == '\n' ||
                          line[linelen - 1] == '\r'))
      linelen--;
    editorInsertRow(E.number_of_rows, line, linelen);
  }
  free(line);
  fclose(fp);
  E.dirty = 0;
}

void editorSave() {
  if (E.filename == NULL) {
    E.filename = editorPrompt("Save as: %s (ESC to cancel)", NULL);
    if (E.filename == NULL) {
      editorSetStatusMessage("Save aborted");
      return;
    }
    editorSelectSyntaxHighlight();
  }

  int len;
  char *buf = editorRowsToString(&len);

  int fd = open(E.filename, O_RDWR | O_CREAT, 0644);
  if (fd != -1) {
    if (ftruncate(fd, len) != -1) {
      if (write(fd, buf, len) == len) {
        close(fd);
        free(buf);
        E.dirty = 0;
        editorSetStatusMessage("%d bytes written to disk", len);
        return;
      }
    }
    close (fd);
  }

  free(buf);
  editorSetStatusMessage("Can't save! I/O error: %s", strerror(errno));
}

/*** find **/

void editorFindCallback(char *query, int key) {
  static int last_match = -1;
  static int direction = 1;

  static int saved_highlight_line;
  static char *saved_highlight = NULL;

  if (saved_highlight) {
    memcpy(E.row[saved_highlight_line].highlight, saved_highlight, E.row[saved_highlight_line].rendered_size);
    saved_highlight = NULL;
  }

  if (key == '\r' || key == '\x1b') {
    last_match = -1;
    direction = 1;
    return;
  } else if (key == ARROW_RIGHT || key == ARROW_DOWN) {
    direction = 1;
  } else if (key == ARROW_LEFT || key == ARROW_UP) {
    direction = -1;
  } else {
    last_match = -1;
    direction = 1;
  }

  if (last_match == -1) direction = 1;
  int current = last_match;
  int i;
  for (i = 0; i < E.number_of_rows; i++) {
    current += direction;
    if (current == -1) current = E.number_of_rows - 1;
    else if (current == E.number_of_rows) current = 0;

    erow *row = &E.row[current];
    char *match = strstr(row->rendered_characters, query);
    if (match) {
      last_match = current;
      E.file_position_y = current;
      E.file_position_x = editorRowScreenPositionXToFilePositionX(row, match - row->rendered_characters);
      E.row_offset = E.number_of_rows;

      saved_highlight_line = current;
      saved_highlight = malloc(row->rendered_size);
      memcpy(saved_highlight, row->highlight, row->rendered_size);
      memset(&row->highlight[match - row->rendered_characters], HL_MATCH, strlen(query));
      break;
    }
  }
}

void editorFind() {
  int saved_file_position_x = E.file_position_x;
  int saved_file_position_y = E.file_position_y;
  int saved_coloff = E.column_offset;
  int saved_rowoff = E.row_offset;

  char *query = editorPrompt("Search, %s (ESC/Arrows/Enter)",
                             editorFindCallback);

  if (query) {
    free(query);
  } else {
    E.file_position_x = saved_file_position_x;
    E.file_position_y = saved_file_position_y;
    E.column_offset = saved_coloff;
    E.row_offset = saved_rowoff;
  }
}

/*** append buffer ***/

struct abuf {
  char *b;
  int len;
};

#define ABUF_INIT {NULL, 0}

void abAppend(struct abuf *ab, const char *s, int len) {
  char *new = realloc(ab->b, ab->len + len);

  if (new == NULL) return;

  memcpy(&new[ab->len], s, len);
  ab->b = new;
  ab->len +=len;
}

void abFree(struct abuf *ab) {
  free(ab->b);
}

/*** output ***/

void editorScroll() {
  E.screen_position_x = 0;
  if (E.file_position_y < E.number_of_rows) {
    E.screen_position_x = editorRowFilePositionXToScreenPositionX(&E.row[E.file_position_y], E.file_position_x);
  }

  if (E.file_position_y < E.row_offset) {
    E.row_offset = E.file_position_y;
  }
  if (E.file_position_y >= E.row_offset + E.screen_rows) {
    E.row_offset = E.file_position_y - E.screen_rows + 1;
  }
  if (E.screen_position_x < E.column_offset) {
    E.column_offset = E.screen_position_x;
  }
  if (E.screen_position_x >= E.column_offset + E.screen_columns) {
    E.column_offset = E.screen_position_x - E.screen_columns + 1;
  }
}

void editorDrawRows(struct abuf *ab) {
  int y;
  for (y = 0; y < E.screen_rows; y++) {
    int filerow = y + E.row_offset;
    if (filerow >= E.number_of_rows ) {
      if (E.number_of_rows == 0 && y == E.screen_rows / 3) {
        char welcome[80];

        int welcomelen = snprintf(welcome, sizeof(welcome),
            "Texor editor -- version %s", TEXOR_VERSION);
        if (welcomelen > E.screen_columns) welcomelen = E.screen_columns;
        int padding = (E.screen_columns - welcomelen) / 2;
        if (padding) {
          abAppend(ab, "~", 2);
          padding--;
        }
        while(padding--) abAppend(ab, " ", 1);
        abAppend(ab, welcome, welcomelen);
      } else {
        abAppend(ab, "~", 1);
      }
    } else {
      int len = E.row[filerow].rendered_size - E.column_offset;
      if (len < 0) len = 0;
      if (len > E.screen_columns) len = E.screen_columns;
      char *c = &E.row[filerow].rendered_characters[E.column_offset];
      unsigned char *highlight = &E.row[filerow].highlight[E.column_offset];
      int current_color = -1;
      int j;
      for (j = 0; j < len; j++) {
        if (iscntrl(c[j])) {
          char sym = (c[j] <= 26) ? '@' + c[j] : '?';
          abAppend(ab, "\x1b[7m", 4);
          abAppend(ab, &sym, 1);
          abAppend(ab, "\x1b[m", 3);
          if (current_color != -1) {
            char buf[16];
            int clen = snprintf(buf, sizeof(buf), "\x1b[%dm", current_color);
            abAppend(ab, buf, clen);
          }
        } else if (highlight[j] == HL_NORMAL) {
          if (current_color != -1) {
            abAppend(ab, "\x1b[39m", 5);
            current_color = -1;
          }
          abAppend(ab, &c[j], 1);
        } else {
          int color = editorSyntaxToColor(highlight[j]);
          if (color != current_color) {
            current_color = color;
            char buf[16];
            int clen = snprintf(buf, sizeof(buf), "\x1b[%dm", color);
            abAppend(ab, buf, clen);
          }
          abAppend(ab, &c[j], 1);
        }
      }
      abAppend(ab, "\x1b[39m", 5);
    }

    abAppend(ab, "\x1b[K", 3);
    abAppend(ab, "\r\n", 2);
  }
}

void editorDrawStatusBar(struct abuf *ab) {
  abAppend(ab, "\x1b[7m", 4);
  char status[80], rstatus[80];
  int len = snprintf(status, sizeof(status), "%.20s - %d lines %s",
      E.filename ? E.filename : "[No Name]", E.number_of_rows,
      E.dirty ? "(modified)" : "");
  int rlen = snprintf(rstatus, sizeof(rstatus), "%s | %d/%d",
      E.syntax ? E.syntax->file_type : "no ft",
      E.file_position_y + 1, E.number_of_rows);
  if (len > E.screen_columns) len = E.screen_columns;
  abAppend(ab, status, len);
  while (len < E.screen_columns) {
    if (E.screen_columns - len == rlen) {
      abAppend(ab, rstatus, rlen);
      break;
    } else {
      abAppend(ab, " ", 1);
      len++;
    }
  }
  abAppend(ab, "\x1b[m", 3);
  abAppend(ab, "\r\n", 2);
}

void editorDrawMessageBar(struct abuf *ab) {
  abAppend(ab, "\x1b[K", 3);
  int msglen = strlen(E.status_message);
  if (msglen > E.screen_columns) msglen = E.screen_columns;
  if (msglen && time(NULL) - E.status_message_time < 5)
    abAppend(ab, E.status_message, msglen);
}

void editorRefreshScreen() {
  editorScroll();

  struct abuf ab = ABUF_INIT;

  abAppend(&ab, "\x1b[?25l", 6);
  abAppend(&ab, "\x1b[H", 3);

  editorDrawRows(&ab);
  editorDrawStatusBar(&ab);
  editorDrawMessageBar(&ab);

  char buf[32];
  snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (E.file_position_y - E.row_offset) + 1,
                                            (E.screen_position_x - E.column_offset) + 1);
  abAppend(&ab, buf, strlen(buf));

  abAppend(&ab, "\x1b[?25h", 6);

  write(STDOUT_FILENO, ab.b, ab.len);
  abFree(&ab);
}

void editorSetStatusMessage(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(E.status_message, sizeof(E.status_message), fmt, ap);
  va_end(ap);
  E.status_message_time = time(NULL);
}

/*** input ***/

char *editorPrompt(char *prompt, void (*callback)(char *, int)) {
  size_t bufsize = 128;
  char *buf = malloc(bufsize);

  size_t buflen = 0;
  buf[0] = '\0';

  while(1) {
    editorSetStatusMessage(prompt, buf);
    editorRefreshScreen();

    int c = editorReadKey();
    if (c == DEL_KEY || c == CTRL_KEY('h') || c == BACKSPACE) {
      if (buflen != 0) buf[--buflen] = '\0';
    } else if (c == '\x1b') {
      editorSetStatusMessage("");
      if (callback) callback(buf, c);
      free(buf);
      return NULL;
    } else if (c == '\r') {
      if (buflen != 0) {
        editorSetStatusMessage("");
        if (callback) callback(buf, c);
        return buf;
      }
    } else if (!iscntrl(c) && c < 128) {
      if (buflen == bufsize - 1) {
        bufsize *= 2;
        buf = realloc(buf, bufsize);
      }
      buf[buflen++] = c;
      buf[buflen] = '\0';
    }

    if (callback) callback(buf, c);
  }
}

void editorMoveCursor(int key) {
  erow *row = (E.file_position_y >= E.number_of_rows) ? NULL : &E.row[E.file_position_y];

  switch (key) {
    case ARROW_LEFT:
      if (E.file_position_x != 0) {
        E.file_position_x--;
      } else if (E.file_position_y > 0) {
        E.file_position_y--;
        E.file_position_x = E.row[E.file_position_y].size;
      }
      break;
    case ARROW_RIGHT:
      if (row && E.file_position_x < row->size) {
        E.file_position_x++;
      } else if (row && E.file_position_x == row->size) {
        E.file_position_y++;
        E.file_position_x = 0;
      }
      break;
    case ARROW_UP:
      if (E.file_position_y != 0) {
        E.file_position_y--;
      }
      break;
    case ARROW_DOWN:
      if (E.file_position_y < E.number_of_rows) {
        E.file_position_y++;
      }
      break;
  }

  row = (E.file_position_y >= E.number_of_rows) ? NULL : &E.row[E.file_position_y];
  int rowlen = row ? row->size : 0;
  if (E.file_position_x > rowlen) {
    E.file_position_x = rowlen;
  }
}

void editorProcessKeypress() {
  static int quit_times = TEXOR_QUIT_TIMES;

  int c = editorReadKey();

  switch(c) {
    case '\r':
      editorInsertNewline();
      break;

    case CTRL_KEY('q'):
      if (E.dirty && quit_times > 0) {
        editorSetStatusMessage("WARNING!!! File has unsaved changes. "
          "Press Ctrl-Q %d more times to quit.", quit_times);
        quit_times--;
        return;
      }
      write(STDOUT_FILENO, "\x1b[2J", 4);
      write(STDOUT_FILENO, "\x1b[H", 3);
      exit(0);
      break;

    case CTRL_KEY('s'):
      editorSave();
      break;

    case HOME_KEY:
      E.file_position_x = 0;
      break;

    case END_KEY:
      if (E.file_position_y < E.number_of_rows)
        E.file_position_x = E.row[E.file_position_y].size;
      break;

    case CTRL_KEY('f'):
      editorFind();
      break;

    case BACKSPACE:
    case CTRL_KEY('h'):
    case DEL_KEY:
      if (c == DEL_KEY) editorMoveCursor(ARROW_RIGHT);
      editorDelChar();
      break;

    case PAGE_UP:
    case PAGE_DOWN:
      {
        if (c == PAGE_UP) {
          E.file_position_y = E.row_offset;
        } else if (c == PAGE_DOWN) {
          E.file_position_y = E.row_offset + E.screen_rows - 1;
          if (E.file_position_y > E.number_of_rows) E.file_position_y = E.number_of_rows;
        }

        int times = E.screen_rows;
        while(times--)
          editorMoveCursor(c == PAGE_UP? ARROW_UP : ARROW_DOWN);
      }
      break;

    case ARROW_UP:
    case ARROW_DOWN:
    case ARROW_LEFT:
    case ARROW_RIGHT:
      editorMoveCursor(c);
      break;

    case CTRL_KEY('l'):
    case '\x1b':
      break;

    default:
      editorInsertChar(c);
      break;
  }

  quit_times = TEXOR_QUIT_TIMES;
}
/*** init ***/
void initEditor() {
  E.file_position_x = 0;
  E.file_position_y = 0;
  E.screen_position_x = 0;
  E.row_offset = 0;
  E.column_offset = 0;
  E.number_of_rows = 0;
  E.row = NULL;
  E.dirty = 0;
  E.filename = NULL;
  E.status_message[0] = '\0';
  E.status_message_time = 0;
  E.syntax = NULL;

  if (getWindowSize(&E.screen_rows, &E.screen_columns) == -1) die("getWindowSize");
  E.screen_rows -= 2;
}
int main(int argc, char **argv) {
  enableRawMode();
  initEditor();
  if (argc < 2)
  {
    // DO NOTHING \_(ツ)_/
  }
  if (argc >= 2) 
  {
    editorOpen(argv[1]);
  }
  if (fopen(argv[1], "r+") == NULL && argc >= 2)
  {
     FILE *f = fopen(argv[1], "w");
     editorOpen(argv[1]);
  }  
  editorSetStatusMessage(
      "HELP: Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find");
  while (1) {
    editorRefreshScreen();
    editorProcessKeypress();
  }
  return 0;
}
