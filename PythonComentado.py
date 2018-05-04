import socket
import soundfile as sf
import numpy as np
import time

#Kinetis IP address
UDP_IP = "192.168.0.102"

#Computer defalut port
UDP_PORT = 50007

#Buffer size
SIZE = 255;
LENnewdata = 3218124;

#Range limits for the buffer
Ns = SIZE;
Nsl = 0;

#Read audio file
data, samplerate = sf.read('Crazy.wav')

#Process for calculating the waiting time necessary for a correct sampling
SongDuration = len(data)/(samplerate)
TimeToSleep = float(Ns)/(samplerate/2)


#Converting audio data into mono
data = (data[:,0] + data[:,1])/2
#Sampling every 4 data values
newdata = data[0::2]

NewSongDuration = (len(newdata)/float(Ns))*(TimeToSleep)

# 12bit variable for DAC
newdata =2000*newdata
# all positive values offset
newdata = newdata+2000

# casting to uint16
newdata = np.cast[np.uint16](newdata)

# Creating UDP socket
sock = socket.socket(socket.AF_INET,
                             socket.SOCK_DGRAM)
sock.connect((UDP_IP, UDP_PORT))


while True:
    # Send specific range of values from our buffer
    sock.sendto(newdata[Nsl:Ns], (UDP_IP, UDP_PORT))
    # Changing the range values
    Nsl = Nsl + SIZE
    Ns = Ns + SIZE
    time.sleep((TimeToSleep-.0006413))
    # if we finish going through the buffer, reset the range values
    if(len(newdata) < Ns):
        Nsl = 0
        Ns = SIZE