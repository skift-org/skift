#pragma once

int filesystem_link(const char* oldpath, const char* newpath);
int filesystem_unlink(const char* path);
int filesystem_mkdir(const char* path);
int filesystem_rename(const char* old_path, const char* new_path);