{
 "cells": [
  {
   "cell_type": "code",
   "execution_count": 62,
   "metadata": {},
   "outputs": [
    {
     "name": "stdout",
     "output_type": "stream",
     "text": [
      "UDP target IP: 192.168.0.101\n",
      "UDP target port: 50009\n",
      "message:  Hello, World! \n",
      "\n",
      "492699649\n",
      "b'\\x01\\x00^\\x1d'\n",
      "Message Sent\n"
     ]
    }
   ],
   "source": [
    "import wave, struct\n",
    "import socket\n",
    "import sys\n",
    "import math\n",
    "import numpy as np\n",
    "import soundfile as sf\n",
    "\n",
    "rms = [np.sqrt(np.mean(block**2)) for block in sf.blocks('sine.wav', blocksize=1024, overlap = 160)]\n",
    "\n",
    "#waveFile = wave.open('sine.wav', 'rb')\n",
    "\n",
    "#length = waveFile.getnframes() \n",
    "    \n",
    "#for i in range(0,length):\n",
    "#    waveData = waveFile.readframes(1)\n",
    "#    data = struct.unpack(\"<h\", waveData)\n",
    "#    print(int(data[0]))\n",
    "\n",
    "UDP_IP = \"192.168.0.101\"\n",
    "UDP_PORT = 50009\n",
    "MESSAGE = \" Hello, World! \\n\"\n",
    " \n",
    "print (\"UDP target IP:\", UDP_IP)\n",
    "print (\"UDP target port:\", UDP_PORT)\n",
    "print (\"message:\", MESSAGE)\n",
    " \n",
    "try:\n",
    "    sock = socket.socket(socket.AF_INET, # Internet\n",
    "                         socket.SOCK_STREAM) # UDP\n",
    "except:\n",
    "    print('Error Create')\n",
    "    sys.exit()\n",
    "\n",
    "sock.connect((UDP_IP,UDP_PORT))\n",
    "\n",
    "#waveData = waveFile.readframes(1)\n",
    "#https://docs.python.org/2/library/struct.html\n",
    "#data = struct.unpack(\">H\", waveData) #little endian(<h) y short (h)\n",
    "#waveData = waveFile.readframes(1)\n",
    "#data = struct.unpack(\">H\", waveData)\n",
    "#waveData = waveFile.readframes(1)\n",
    "#print(int(data[0]))\n",
    "#print(bytes(data[0]))\n",
    "val = math.floor(rms[1]*1000000000)\n",
    "valSend = struct.pack(\"I\",val)\n",
    "print (val)\n",
    "print (valSend)\n",
    "\n",
    "try:\n",
    "    sock.send((str(val)+\"\\n\").encode())\n",
    "    sock.send((MESSAGE+\"\\n\").encode())\n",
    "    #sock.send(bytes(data[0]))\n",
    "    #sock.send(val)\n",
    "    #sock.send(int(data[0]))\n",
    "    #sock.send(waveFile.readframes(1))\n",
    "except socket.error:\n",
    "    print('Error Send')\n",
    "    sys.exit()\n",
    "    \n",
    "print('Message Sent')"
   ]
  },
  {
   "cell_type": "code",
   "execution_count": null,
   "metadata": {},
   "outputs": [],
   "source": []
  },
  {
   "cell_type": "code",
   "execution_count": null,
   "metadata": {},
   "outputs": [],
   "source": []
  }
 ],
 "metadata": {
  "kernelspec": {
   "display_name": "Python 3",
   "language": "python",
   "name": "python3"
  },
  "language_info": {
   "codemirror_mode": {
    "name": "ipython",
    "version": 3
   },
   "file_extension": ".py",
   "mimetype": "text/x-python",
   "name": "python",
   "nbconvert_exporter": "python",
   "pygments_lexer": "ipython3",
   "version": "3.6.4"
  }
 },
 "nbformat": 4,
 "nbformat_minor": 2
}
