
import socket
import sys
import math
import numpy as np
import soundfile as sf

rms = [np.sqrt(np.mean(block**2)) for block in sf.blocks('RussYungGod.wav', blocksize=1024, overlap = 1000)]#blocksize=512, overlap = 511)

UDP_IP = "192.168.0.102"
UDP_PORT = 50007
MESSAGE = " Hello, World! \n"

try:
    sock = socket.socket(socket.AF_INET, # Internet
                         socket.SOCK_DGRAM) # UDP
except:
    print('Error Create')
    sys.exit()

sock.connect((UDP_IP,UDP_PORT))
ax = 0

while(1):
    for i in range(0,len(rms)):
        try:
            val_send = (rms[i])*10000
            
            val = math.floor(val_send)
            ax = ax + 1
            
            #val = math.floor(rms[i]*1000000000)
            print(val)
            sock.sendto((str(val)+"\n").encode(),(UDP_IP,UDP_PORT))
        except socket.error:
            print('Error Send')
            sys.exit()
    
    sock.sendto(("\n"+str(ax)+" END").encode(),(UDP_IP,UDP_PORT))
    print("\n"+str(ax)+" END")
    print('\nMessage Sent\n')
    
    print(sys.getsizeof(ax))
