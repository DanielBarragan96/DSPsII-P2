import wave, struct
import socket
import sys

waveFile = wave.open('sine.wav', 'rb')

length = waveFile.getnframes() 
    
#for i in range(0,length):
#    waveData = waveFile.readframes(1)
#    data = struct.unpack("<h", waveData)
#    print(int(data[0]))

UDP_IP = "192.168.0.101"
UDP_PORT = 54320
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

waveData = waveFile.readframes(1)
#https://docs.python.org/2/library/struct.html
data = struct.unpack("<h", waveData) #little endian(<h) y short (h)
print(int(data[0]))

try:
    sock.send(MESSAGE.encode())
    #sock.send(int(data[0]))
    #sock.send(data[0])
    #sock.send(waveFile.readframes(1))
except socket.error:
    print('Error Send')
    sys.exit()
    
print('Message Sent')