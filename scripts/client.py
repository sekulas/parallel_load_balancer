import socket

request_number = 100
client_sockets = []

for _ in range(request_number):
    try:
        server_address = ('127.0.0.1', 8080)
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_socket.setblocking(False)
        client_sockets.append(client_socket)

        client_socket.connect(server_address)
    except BlockingIOError:
        pass

for client_socket in client_sockets:
    client_socket.setblocking(True)
    response = client_socket.recv(1024)
    client_socket.close()



