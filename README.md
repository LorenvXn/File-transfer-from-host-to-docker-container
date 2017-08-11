# File-transfer-from-host-to-docker-container


<i> Good ol' C programming. </i>

usage: ``gcc  <program>.c -o <program>``

From host, compile & run host.c ; from container, compile & run container.c (obviously...)

Example:

From Host Terminal -  sending the containing of file located under /home/tron/chichi/send.txt to container

```
root@tron:~/netty# more /home/tron/chichi/send.txt
ooo
rrrr
aaaaaaa
root@tron:~/netty#
root@tron:~/netty# ./host
host: waiting for connections...
host: got connection from 172.17.0.2
[host] Sending /home/tron/chichi/send.txt to the Client...File sent!................
[host] Connection with Container closed. Host  will wait now...
^C
```

From Container Terminal - provide the IP of the host 

```
root@e61e42e5b2b8:# ./container 10.0.2.15
container: connecting to 10.0.2.15
File received !
root@e61e42e5b2b8:#
root@e61e42e5b2b8:# 
root@e61e42e5b2b8:/home# more /home/test/receive.txt 
ooo
rrrr
aaaaaaa
root@e61e42e5b2b8:# 

```
