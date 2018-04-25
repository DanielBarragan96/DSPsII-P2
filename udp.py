import wave, struct
import socket
import sys
import math
import numpy as np
import soundfile as sf

rms = [np.sqrt(np.mean(block**2)) for block in sf.blocks('sine.wav', blocksize=1024, overlap = 1000)]

UDP_IP = "192.168.0.100"
UDP_PORT = 50009
MESSAGE = " Hello, World! \n"

try:
    sock = socket.socket(socket.AF_INET, # Internet
                         socket.SOCK_DGRAM) # UDP
except:
    print('Error Create')
    sys.exit()

sock.connect((UDP_IP,UDP_PORT))
ax = 0

for i in range(0,len(rms)):
    try:
        val = math.floor(rms[i]*1000000000)
        print(val)
        sock.sendto((str(val)+"\n").encode(),(UDP_IP,UDP_PORT))
        ax = ax + 1
    except socket.error:
        print('Error Send')
        sys.exit()

sock.sendto(("\n"+str(ax)+"END").encode(),(UDP_IP,UDP_PORT))
print("\n"+str(ax)+"END")
print('Message Sent')
