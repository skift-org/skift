#pragma once

ssize_t write(int fd, const void * buf, size_t count);
ssize_t read(int fd, void * buf, size_t count);
int chdir(const char *path);