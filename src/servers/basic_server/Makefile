
exe_name = asos_basic_server

main_src = main.cpp 

sub_src  = ../server_common/Server.cpp 
sub_src += ../server_common/Signal.cpp 
sub_src += ../server_common/websock_util.cpp 
sub_src += ../server_common/sha1.cpp 
sub_src += ../server_common/Connection.cpp 
sub_src += ../server_common/Processer.cpp 
sub_src += ../server_common/ProcesserWebSocket.cpp 
sub_src += ../server_common/ProcesserHttp.cpp
sub_src += ../asos_common/ASOS_Node.cpp
sub_src += ../asos_common/ASOS_message.cpp
sub_src += ../asos_common/ASOS_Core.cpp
sub_src += ../asos_common/ASOS_ObjectField.cpp
sub_src += ../asos_common/ASOS_Object.cpp
sub_src += ../asos_common/ASOS_Protocolv1.cpp

header   = ../server_common/Server.hpp
header  += ../server_common/Signal.hpp 
header  += ../server_common/websock_util.hpp 
header  += ../server_common/sha1.hpp 
header  += ../server_common/Connection.hpp 
header  += ../server_common/Processer.hpp 
header  += ../server_common/ProcesserWebSocket.hpp 
header  += ../server_common/ProcesserHttp.hpp
header  += ../asos_common/ASOS_Node.hpp
header  += ../asos_common/ASOS_message.hpp
header  += ../asos_common/ASOS_Core.hpp
header  += ../asos_common/ASOS_ObjectField.hpp
header  += ../asos_common/ASOS_Object.hpp
header  += ../asos_common/ASOS_Protocolv1.hpp

include_dir = -I../server_common/ -I../asos_common/

all:	$(exe_name)

$(exe_name):	$(main_src) $(sub_src) $(header)
	g++ -g -o $(exe_name) $(include_dir) $(main_src) $(sub_src) -lpthread

clean:
	rm $(exe_name)

