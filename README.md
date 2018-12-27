# Small web daemon

This is my simple implementaion of multithreaded small web daemon. It uses sendfile() syscall, epoll()(but in blocking mode), dynamic queues.
One queue is used for incoming requests, the second one is for sending replies to client in dedicated thread. Serves only static pages.</br>
Features:
* Multithreaded
* My own dynamic queues implementation
* To improve performance it implemented with using sendfile(), epoll syscalls
* My own HTTP protocol parsing implementation
* Supports jpg, png, pdf, mpeg, html MIMEs. Easy to add new types
* Serves only static files

Run:
```bash
./swd -c <path to conf file>
```

conf file:
<pre>
port = 8080;
rootdir =/var/www/;
listen =127.0.0.1;
workers =2;
</pre>
where</br>
```port``` is a listening port</br>
```rootdir``` is a directory to search files</br>
```listen``` - is an address to listen to</br>
```workers``` is a number of worker threads to process requests and number of sender threads</br>

Directory ```error_pages``` must be placed into ```rootdir```.

TODO:
* implement ```HEAD``` method
* implement index page searching(specifying index page in conf file)
* graceful reload daemon by signal (```kill -SIGUSR1```, for example)
* create and dedicate HTTP library
# Working copy 
Running server is availabe at http://denis.ilmen-tau.ru/index.html
