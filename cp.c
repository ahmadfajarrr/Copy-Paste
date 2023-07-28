#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <libgen.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <pthread.h>

#define BUF_SIZE 4096

const char* my_basename(const char *path) {
    const char *last_slash = strrchr(path, '/');
    if (last_slash == NULL) {
        return path;
    }
    return last_slash + 1;
}

void print_progress(double percentage) {
    const int bar_width = 50;
    int progress = bar_width * percentage;
    printf("[");
    for (int i = 0; i < bar_width; i++) {
        if (i < progress) {
            printf("=");
        } else {
            printf(" ");
        }
    }
    printf("] %.0f%%\r", percentage * 100);
    fflush(stdout);
}

void copy_file(const char *source_file, const char *destination) {
    int source_fd, destination_fd;
    ssize_t bytesRead, bytesWritten;
    char buffer[BUF_SIZE];
    struct stat destination_stat;

    source_fd = open(source_file, O_RDONLY);
    if (source_fd == -1) {
        perror("Error opening source file");
        return;
    }

    // Get information about the destination file or directory
    if (stat(destination, &destination_stat) == -1) {
        perror("Error getting destination file/directory information");
        return;
    }

    if (S_ISDIR(destination_stat.st_mode)) {
        // If destination is a directory, construct the target file path
        char destination_file_path[BUF_SIZE];
        snprintf(destination_file_path, BUF_SIZE, "%s/%s", destination, my_basename(source_file));

        destination_fd = open(destination_file_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (destination_fd == -1) {
            perror("Error opening destination file");
            return;
        }
    } else {
        // If destination is not a directory, open it as a file
        destination_fd = open(destination, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (destination_fd == -1) {
            perror("Error opening destination file");
            return;
        }
    }

    off_t file_size = lseek(source_fd, 0, SEEK_END);
    lseek(source_fd, 0, SEEK_SET);

    off_t bytesCopied = 0;

    while ((bytesRead = read(source_fd, buffer, BUF_SIZE)) > 0) {
        bytesWritten = write(destination_fd, buffer, bytesRead);
        if (bytesWritten != bytesRead) {
            perror("Error writing to destination file");
            return;
        }

        bytesCopied += bytesRead;
        double percentage = (double)bytesCopied / file_size;
        print_progress(percentage);
    }

    if (bytesRead == -1) {
        perror("Error reading from source file");
        return;
    }

    if (close(source_fd) == -1) {
        perror("Error closing source file");
        return;
    }

    if (close(destination_fd) == -1) {
        perror("Error closing destination file");
        return;
    }

    printf("\nFile '%s' copied successfully.\n", source_file);
}

void copy_directory(const char *source_dir, const char *destination) {
    DIR *dir;
    struct dirent *entry;
    char source_path[PATH_MAX];
    char destination_path[PATH_MAX];

    dir = opendir(source_dir);
    if (dir == NULL) {
        perror("Error opening source directory");
        return;
    }

    // Create destination directory if it doesn't exist
    if (mkdir(destination, 0777) == -1) {
        perror("Error creating destination directory");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            snprintf(source_path, PATH_MAX, "%s/%s", source_dir, entry->d_name);
            snprintf(destination_path, PATH_MAX, "%s/%s", destination, entry->d_name);

            struct stat entry_stat;
            if (stat(source_path, &entry_stat) == -1) {
                perror("Error getting source file/directory information");
                continue; // Skip this entry and continue with the next one
            }

            if (S_ISDIR(entry_stat.st_mode)) {
                copy_directory(source_path, destination_path);
            } else {
                copy_file(source_path, destination);
            }
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source_file_or_pattern> <destination_file_or_directory>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *source = argv[1];
    const char *destination = argv[2];

    struct stat source_stat;
    if (stat(source, &source_stat) == -1) {
        perror("Error getting source file/directory information");
        exit(EXIT_FAILURE);
    }

    if (S_ISDIR(source_stat.st_mode)) {
        copy_directory(source, destination);
    } else {
        copy_file(source, destination);
    }

    exit(EXIT_SUCCESS);
}
