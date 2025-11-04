import socket
import time
import argparse
import random

parser = argparse.ArgumentParser(
    prog="Packet sender for packet receive testing")
parser.add_argument('--long', action='store_true')
parser.add_argument('--medium', action='store_true')
parser.add_argument('--short', action='store_true')
parser.add_argument('--fast', action='store_true')
parser.add_argument('--moderate', action='store_true')
parser.add_argument('--slow', action='store_true')
parser.add_argument('--zeros', action='store_true')
parser.add_argument('--random', action='store_true')
parser.add_argument('--reps', type=int)

args = parser.parse_args()

if (args.long and args.medium) or \
        (args.long and args.short) or \
        (args.medium and args.short) or \
        (not args.long and not args.medium and not args.short):
    print("Include exactly one of '--long', '--medium', '--short'")
    exit()

if (args.fast and args.moderate) or \
        (args.fast and args.slow) or \
        (args.moderate and args.slow) or \
        (not args.fast and not args.moderate and not args.slow):
    print("Include exactly one of '--fast', '--moderate', '--slow'")
    exit()

if (args.zeros and args.random) or \
        (not args.zeros and not args.random):
    print("Include exactly one of '--zeros', '--random'")
    exit()

if not args.reps:
    print("You must provide a number of packets to send (e.g. --reps 10)")
    exit()

if args.long:
    length = 200
elif args.medium:
    length = 50
elif args.short:
    length = 1

if args.fast:
    dur = .5
elif args.moderate:
    dur = 1.5
elif args.slow:
    dur = 3


def get_packet():
    global args
    packet = bytearray()
    for _ in range(length):
        if args.zeros:
            packet.append(0)
        elif args.random:
            packet.append(random.randint(0, 255))
    return packet


# remote_addr = ("192.168.1.216", 8002)
remote_addr = ("127.0.0.1", 8002)

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(remote_addr)
for _ in range(args.reps):
    packet = get_packet()
    print(f"sending packet\n{packet}")
    sock.send(packet)
    time.sleep(dur)
sock.close()
