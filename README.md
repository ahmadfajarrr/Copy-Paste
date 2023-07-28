File Copy Program
Description
This program is a file and directory copying tool that allows you to copy files and directories from one location to another. The program supports the feature to skip copying if there are errors during the process, so the program will continue copying the remaining files and directories even if there are errors for certain files or directories.

How to Use
Compile the program using the following command:
gcc -o cp cp.c
./cp <source_file_or_pattern> <destination_file_or_directory>

<source_file_or_pattern>: The name of the file or file pattern to be copied. If you want to copy the entire contents of a directory, use the wildcard character *, for example, Documents/*.
<destination_file_or_directory>: The name of the destination file or directory where the files will be copied. If the destination directory does not exist, the program will create it automatically.
The program will display the copy progress for each file or directory being copied. If there is an error during the copy process, the program will display an error message, but it will continue the copy process for the next files or directories.

The program will display the message "File '<filename>' copied successfully." for each file that is successfully copied.

Notes:

If you want to copy the entire contents of a directory, make sure to add the wildcard character * at the end of the source directory, for example, Documents/*.
If there is an error while opening, writing, or closing a file, the program will continue with the next file and display an error message for the file that encountered the error.
The program also displays an animated progress bar for each file being copied.
The program optimizes the copy process by using up to 4 threads to increase copy speed.
