FileServer
==========

Build a fileserver and a client program that can upload/download/show the dir on server. Using the aid of md5 to save time when file is already on server.

compile server:
gcc -o server server.c check_md5.c show_dir.c ../methods.c ../Transfer.c ../File_recv.c md5.c -lcrypto

compile client:

gcc -o file_client_get file_client_get.c ../methods.c ../File_recv.c

gcc -o file_client_show file_client_show.c ../methods.c

gcc -o file_client_push file_client_push.c ../methods.c ../Transfer.c ../md5.c -lcrypto

