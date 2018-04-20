import wave, struct
import socket
import sys
import math
import numpy as np
import soundfile as sf

rms = [np.sqrt(np.mean(block**2)) for block in sf.blocks('sine.wav', blocksize=1024, overlap = 160)]

#waveFile = wave.open('sine.wav', 'rb')

#length = waveFile.getnframes() 
    
#for i in range(0,length):
#    waveData = waveFile.readframes(1)
#    data = struct.unpack("<h", waveData)
#    print(int(data[0]))

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

#waveData = waveFile.readframes(1)
#https://docs.python.org/2/library/struct.html
#data = struct.unpack(">H", waveData) #little endian(<h) y short (h)
#waveData = waveFile.readframes(1)
#data = struct.unpack(">H", waveData)
#waveData = waveFile.readframes(1)
#print(int(data[0]))
#print(bytes(data[0]))
val = math.floor(rms[1]*1000000000)
valSend = struct.pack("I",val)
print (val)
print (valSend)

try:
    sock.send((str(val)+"\n").encode())
    sock.send((MESSAGE+"\n").encode())
    #sock.send(bytes(data[0]))
    #sock.send(val)
    #sock.send(int(data[0]))
    #sock.send(waveFile.readframes(1))
except socket.error:
    print('Error Send')
    sys.exit()
    
print('Message Sent')
