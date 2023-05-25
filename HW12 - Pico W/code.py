import board
import time
import digitalio
import analogio
import io
from ulab import numpy as np

# sample code for initializing I/O
led = digitalio.DigitalInOut(board.LED)
led.direction = digitalio.Direction.OUTPUT
led.value = 0

adcpin = analogio.AnalogIn(board.A0)

button = digitalio.DigitalInOut(board.GP15)
button.direction = digitalio.Direction.INPUT

# populate data array with 3 different sin waves
t = np.arange(0.0, 1024, 1)
data = np.sin(t) + np.sin(5 * t) + np.sin(250 * t)

# take FFT of data
fft = np.fft.fft(data)[0]
fft = fft[0:(int(len(fft)/2)-1)]

# print out data
for pt in fft:
    print(str(pt) + ",")
    time.sleep(0.001)
