import socket
import soundfile as sf
import numpy as np
import time

#Kinetis IP address
UDP_IP = "192.168.0.102"

#Computer defalut port
UDP_PORT = 50011

#Buffer size
SIZE = 225;

#Range limits for the buffer
Ns = SIZE;
Nsl = 0;

#Read audio file
data, samplerate = sf.read('eagles.flac')

#Process for calculating the waiting time necessary for a correct sampling
SongDuration = len(data)/(samplerate)
TimeToSleep = float(Ns)/(samplerate/4)

#Converting audio data into mono
data = (data[:,0] + data[:,1])/2
#Sampling every 4 data values
newdata = data[0::4]

NewSongDuration = (len(newdata)/float(Ns))*(TimeToSleep)

# 12bit variable for DAC
newdata =2048*newdata
# all positive values offset
newdata = newdata+2048

# casting to uint16
newdata = np.cast[np.uint16](newdata)

# Creating UDP socket
sock = socket.socket(socket.AF_INET,
                             socket.SOCK_DGRAM)
sock.connect((UDP_IP, UDP_PORT))


while True:
    for i in range(len(newdata)):
        # Send specific range of values from our buffer
        sock.sendto(newdata[Nsl:Ns], (UDP_IP, UDP_PORT))
        # Changing the range values
        Nsl = Nsl + SIZE
        Ns = Ns + SIZE
        time.sleep((TimeToSleep-.00541))
        # if we finish going through the buffer, reset the range values
        if(len(newdata) < Ns):
            Nsl = 0
            Ns = SIZE