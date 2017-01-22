import socket


def tcp_init():
    wait_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0, None)
    wait_sock.bind(('' ,234))
    wait_sock.listen(5)
    
    return wait_sock
    
def client_interact(Socket):
    while(True):
        try:
            buf = Socket.recv(100)
        except OSError as error_msg:
            return;
        str = buf.decode('utf-8')
        print(str)
    
def tcp_server_run(Socket):
    while(True):
        sock = Socket.accpet()
        client_interact(sock)
        sock.close()
    
def __main__():
    listen_sock = tcp_init()
    tcp_server_run(listen_sock);