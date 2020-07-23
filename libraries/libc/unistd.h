#pragma once

#include <__libc__.h>

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

__BEGIN_HEADER

extern char **environ;

pid_t getpid(void);
pid_t getppid(void);

int close(int fd);

pid_t fork(void);

int execl(const char *path, const char *arg, ...);
int execlp(const char *file, const char *arg, ...);
int execle(const char *path, const char *arg, ...);
int execv(const char *path, char *const argv[]);
int execvp(const char *file, char *const argv[]);
int execvpe(const char *file, char *const argv[], char *const envp[]);
int execve(const char *name, char *const argv[], char *const envp[]);
void _exit(int status);

int setuid(uid_t uid);

uid_t getuid(void);
uid_t geteuid(void);
gid_t getgid(void);
gid_t getegid(void);
char *getcwd(char *buf, size_t size);
int pipe(int pipefd[2]);
int dup(int oldfd);
int dup2(int oldfd, int newfd);

pid_t tcgetpgrp(int fd);
int tcsetpgrp(int fd, pid_t pgrp);

ssize_t write(int fd, const void *buf, size_t count);
ssize_t read(int fd, void *buf, size_t count);

int symlink(const char *target, const char *linkpath);
ssize_t readlink(const char *pathname, char *buf, size_t bufsiz);

int chdir(const char *path);
// int fchdir(int fd);
int isatty(int fd);

unsigned int sleep(unsigned int seconds);
int usleep(useconds_t usec);
off_t lseek(int fd, off_t offset, int whence);

int access(const char *pathname, int mode);

int getopt(int argc, char *const argv[], const char *optstring);

extern char *optarg;
extern int optind, opterr, optopt;

int unlink(const char *pathname);

struct utimbuf
{
    time_t actime;
    time_t modtime;
};
char *ttyname(int fd);
int utime(const char *filename, const struct utimbuf *times);
int rmdir(const char *pathname); /* TODO  rm probably just works */
int chown(const char *pathname, uid_t owner, gid_t group);
char *getlogin(void);

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define F_OK 0
#define R_OK 4
#define W_OK 2
#define X_OK 1

int gethostname(char *name, size_t len);
int sethostname(const char *name, size_t len);

pid_t setsid(void);
int setpgid(pid_t, pid_t);
pid_t getpgid(pid_t);

unsigned int alarm(unsigned int seconds);

void *sbrk(intptr_t increment);

__END_HEADER
