import wave, struct
import socket
import soundfile as sf
import numpy as np
import time

UDP_IP = "192.168.0.102"
#UDP_IP2 = "192.168.0.106"
UDP_PORT = 50007
#UDP_PORT2 = 54307
MESSAGE = "Hello World!"

#print "UDP target IP:", UDP_IP
#print "UDP target port:", UDP_PORT
#print "message:", MESSAGE
SIZE = 225;
Ns = SIZE;
Ms = 0;
Nsl = 0;


data, samplerate = sf.read('Crazy.wav')

data = (data[:,0] + data[:,1])/2
#print data[10000:10010]
newdata = data[0::4]
#print data[4000:4010]
print(len(newdata))
print(len(newdata))
print(len(newdata))
print(len(newdata))
print(len(newdata))
newdata =2048*newdata

newdata = newdata+2048

newdata = np.cast[np.uint16](newdata)
#print newdata[14000:14005]
sock = socket.socket(socket.AF_INET, # Internet
                             socket.SOCK_DGRAM) # UDP
sock.connect((UDP_IP, UDP_PORT))
sock2 = socket.socket(socket.AF_INET, # Internet
                             socket.SOCK_DGRAM) # UDP
sock2.connect((UDP_IP, UDP_PORT))
sock3 = socket.socket(socket.AF_INET, # Internet
                             socket.SOCK_DGRAM) # UDP
sock3.connect((UDP_IP, UDP_PORT))
sock4 = socket.socket(socket.AF_INET, # Internet
                             socket.SOCK_DGRAM) # UDP
sock4.connect((UDP_IP, UDP_PORT))

while True:
    for i in range(len(newdata)):

        sock.sendto(newdata[Nsl:Ns], (UDP_IP, UDP_PORT))
        sock2.sendto(newdata[Nsl:Ns], (UDP_IP, UDP_PORT))
        sock3.sendto(newdata[Nsl:Ns], (UDP_IP, UDP_PORT))
        sock4.sendto(newdata[Nsl:Ns], (UDP_IP, UDP_PORT))
        #print (newdata[Nsl:Ns])
        Nsl = Nsl + SIZE
        Ns = Ns + SIZE
        Ms = Ms + 1
        time.sleep(0.019)
        if(len(newdata) < Ns):
            Nsl = 0
            Ns = SIZE
#socks = socket.socket(socket.AF_INET, # Internet
#                    socket.SOCK_STREAM) # UDP
#socks.connect((UDP_IP2, UDP_PORT2))
#socks.sendto(MESSAGE, (UDP_IP2, UDP_PORT2))