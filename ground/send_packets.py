import socket
import time

remote_addr = ("192.168.1.216", 8002)
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

sock.connect(remote_addr)
for _ in range(100):
    sock.send(bytes.fromhex("1802C0000010198D0B01007263000000000000000083B1"))
    time.sleep(1)
sock.close()
