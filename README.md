# Small web daemon

This is my simple implementaion of multithreaded small web daemon. It uses sendfile() syscall, epoll()(but in blocking mode), dynamic queues.
One queue is used for incoming requests, the second one is for sending replies to client in dedicated thread. Serves only static pages.</br>
Features:
* Multithreaded
* My own dynamic queues implementation
* To improve performance it is implemented with using sendfile(), epoll() syscalls
* My own HTTP protocol parsing implementation
* Supports jpg, png, pdf, mpeg, html MIMEs. Easy to add new types
* Serves only static files
* For storing config and searching virtual hosts RBTree is used(just for fun, it is not optimal in this case)
* Hash tables and associative arrays are used inside virtaul hosts sections (this is also was just for fun and for trying to implement these data structures)

Run:
```bash
./swd -c <path to conf file> -d
```

conf file:
<pre>
&lt;General&gt;
port = 8080;
listen =127.0.0.1;
workers =4;#comment
timeout = 10;
&lt;/General&gt;
&ltHost&gt;
servername=site1.example.com;
rootdir=/var/www/site1;
indexfile=index.html;
&lt/Host&gt;
&ltHost&gt;
servername=site2.example.com;
rootdir=/var/www/site2;
indexfile=index.html;
&lt/Host&gt;
&ltDefault&gt;
servername=site3.example.com;
rootdir=/var/www/site3;
indexfile=index.html;
&lt/Default&gt;
# end of config
</pre>

where</br>
```port``` is a listening port</br>
```listen```  is an address to listen to</br>
```workers``` is a number of worker threads to process requests</br>
Virtual hosts section</br>
```servername``` is a name of a virtaul host</br>
```rootdir``` is a directory where virtual host files located</br>
```indexfile``` is a name of indexfile to be sent</br>
Default section is used in case if no servername matched</br>


TODO:

* graceful reload daemon by signal (```kill -SIGUSR1```, for example)
* create and dedicate HTTP library
* timeout to limit waiting a client
* implement aliases
* duplicated indexfile is also can be considered as additional file for searching, let's say index.html and index.php
* replace explicit defining array of chars by Macro X and put all definitions into h file
* to prevent copy of header name, we can store only pointer to string(first element) in enum and its length
* return a pointer to message_body if request method allows this(i.e.POST)
* implement inorder RBtree traversal
* replace everywhere C-string by string_t to prevent counting length each time(it is too many times!) and spend CPU
* Implement SSL support(https://developer.ibm.com/tutorials/l-openssl/)

For more TODOs see source files</br>

Known problems:</br>

* <em>strncat(char *dest, const char *src, size_t n)</em> - concatenates strings dest and src, the function gets n - length of concatenating string(src), not a buffer size of dest srting. This is not obvious and doesn't protect from buffer overflow. The solution is to use string_t instead of C-string.
