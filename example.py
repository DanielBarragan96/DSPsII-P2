import wave, struct
import socket
import sys
import math
import numpy as np
import soundfile as sf

rms = [np.sqrt(np.mean(block**2)) for block in sf.blocks('sine.wav', blocksize=1024, overlap = 1000)]

UDP_IP = "192.168.0.101"
UDP_PORT = 50009
MESSAGE = " Hello, World! \n"

try:
    sock = socket.socket(socket.AF_INET, # Internet
                         socket.SOCK_DGRAM) # UDP
except:
    print('Error Create')
    sys.exit()

sock.connect((UDP_IP,UDP_PORT))

val = math.floor(rms[1]*1000000000)
valSend = struct.pack("I",val)
print (val)
print (valSend)

try:
    sock.sendto((str(val)+"\n").encode(),(UDP_IP,UDP_PORT))
    sock.sendto(MESSAGE.encode(),(UDP_IP,UDP_PORT))
except socket.error:
    print('Error Send')
    sys.exit()
    
print('Message Sent')
