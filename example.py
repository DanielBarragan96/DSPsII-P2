import wave, struct
import socket
import sys
import math
import numpy as np
import soundfile as sf

rms = [np.sqrt(np.mean(block**2)) for block in sf.blocks('sine.wav', blocksize=1024, overlap = 160)]

UDP_IP = "192.168.0.101"
UDP_PORT = 50009
MESSAGE = " Hello, World! \n"
 
print ("UDP target IP:", UDP_IP)
print ("UDP target port:", UDP_PORT)
print ("message:", MESSAGE)
 
try:
    sock = socket.socket(socket.AF_INET, # Internet
                         socket.SOCK_STREAM) # UDP
except:
    print('Error Create')
    sys.exit()

sock.connect((UDP_IP,UDP_PORT))

val = math.floor(rms[1]*1000000000)
print (val)

try:
    sock.send((str(val)+"\n").encode())
    sock.send((MESSAGE+"\n").encode())
except socket.error:
    print('Error Send')
    sys.exit()
    
print('Message Sent')
