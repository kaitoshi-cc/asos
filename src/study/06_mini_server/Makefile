
main_src = main.cpp 
sub_src = Server.cpp Signal.cpp SHA-1.cpp Connection.cpp Processer.cpp ProcesserWebSocket.cpp ProcesserHttp.cpp
header  = Server.hpp Signal.hpp SHA-1.hpp Connection.hpp Processer.hpp ProcesserWebSocket.hpp ProcesserHttp.hpp

sub_src +=  asos/ASOS_Node.cpp
header  +=  asos/ASOS_Node.hpp

all:	$(main_src) $(sub_src) $(header)
	g++ -g -o mini_httpd $(main_src) $(sub_src) -lpthread

clean:
	rm mini_httpd
