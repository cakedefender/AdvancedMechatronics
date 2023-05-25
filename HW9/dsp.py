import csv
import sys
import numpy as np
import matplotlib.pyplot as plt # for plotting


# Parameters for filters:
numSamplesToAverage = 300  # number of samples to average over in the Moving Average Filter

A = .8                   # Weight of previous average vs newest data point in IIR filter
B = 1.0-A                 # Weight of newest data point vs previous average in IIR filter

# coefficients for Low-pass windowed-sinc FIR filter (hamming window)
# coefficients were sourced from https://fiiir.com/
h = [
    -0.000050679794726066,
    -0.000041099278318167,
    -0.000031279920668665,
    -0.000021183486597150,
    -0.000010770285292045,
    0.000000000000000000,
    0.000011167446754809,
    0.000022770999889941,
    0.000034847558259864,
    0.000047431049079466,
    0.000060551498686721,
    0.000074234108254511,
    0.000088498343199344,
    0.000103357045109305,
    0.000118815575023601,
    0.000134870996840645,
    0.000151511309510219,
    0.000168714736477861,
    0.000186449080596567,
    0.000204671152403140,
    0.000223326279275218,
    0.000242347902542027,
    0.000261657269119383,
    0.000281163223679941,
    0.000300762106756334,
    0.000320337763510928,
    0.000339761667195315,
    0.000358893160569588,
    0.000377579817760222,
    0.000395657928211038,
    0.000412953103529159,
    0.000429281007152519,
    0.000444448205872873,
    0.000458253141344113,
    0.000470487218795955,
    0.000480936009263626,
    0.000489380560741255,
    0.000495598812776237,
    0.000499367108150093,
    0.000500461794444300,
    0.000498660907473236,
    0.000493745927786584,
    0.000485503600706003,
    0.000473727809671115,
    0.000458221492033063,
    0.000438798585855176,
    0.000415285995764155,
    0.000387525565446236,
    0.000355376044004699,
    0.000318715033091691,
    0.000277440901501588,
    0.000231474653767861,
    0.000180761739242710,
    0.000125273788160487,
    0.000065010261293197,
    0.000000000000000000,
    -0.000069697336247377,
    -0.000143989957415198,
    -0.000222752767634882,
    -0.000305826338672358,
    -0.000393016043088374,
    -0.000484091357342654,
    -0.000578785344322494,
    -0.000676794323931742,
    -0.000777777739462615,
    -0.000881358226495441,
    -0.000987121890034750,
    -0.001094618794499868,
    -0.001203363670049808,
    -0.001312836837542114,
    -0.001422485353209744,
    -0.001531724372895900,
    -0.001639938734420840,
    -0.001746484755374530,
    -0.001850692242341569,
    -0.001951866706278179,
    -0.002049291777482158,
    -0.002142231812333790,
    -0.002229934682745978,
    -0.002311634738053158,
    -0.002386555927898205,
    -0.002453915073551964,
    -0.002512925274028313,
    -0.002562799432345805,
    -0.002602753886341418,
    -0.002632012127569287,
    -0.002649808591023194,
    -0.002655392497711921,
    -0.002648031731496151,
    -0.002627016731069257,
    -0.002591664377536210,
    -0.002541321857718479,
    -0.002475370483091317,
    -0.002393229444145817,
    -0.002294359479963247,
    -0.002178266442894981,
    -0.002044504738458277,
    -0.001892680620886388,
    -0.001722455325210333,
    -0.001533548017297868,
    -0.001325738543930948,
    -0.001098869965763655,
    -0.000852850856865069,
    -0.000587657355512251,
    -0.000303334951952833,
    0.000000000000000001,
    0.000322159059450752,
    0.000662880773589522,
    0.001021830060775982,
    0.001398597909331569,
    0.001792701398335994,
    0.002203584045179127,
    0.002630616483032971,
    0.003073097469789485,
    0.003530255228366684,
    0.004001249116626688,
    0.004485171623483913,
    0.004981050686118591,
    0.005487852321559077,
    0.006004483564265146,
    0.006529795699742466,
    0.007062587782654921,
    0.007601610426384373,
    0.008145569849526276,
    0.008693132163411565,
    0.009242927883419039,
    0.009793556645595150,
    0.010343592108937170,
    0.010891587022627668,
    0.011436078436539264,
    0.011975593032464911,
    0.012508652552774892,
    0.013033779302562583,
    0.013549501700820601,
    0.014054359855790191,
    0.014546911139352909,
    0.015025735735186426,
    0.015489442135386880,
    0.015936672560369614,
    0.016366108277098043,
    0.016776474791055797,
    0.017166546887869318,
    0.017535153501103896,
    0.017881182383493146,
    0.018203584559716979,
    0.018501378539810983,
    0.018773654273367416,
    0.019019576825867947,
    0.019238389759765797,
    0.019429418204303113,
    0.019592071599501125,
    0.019725846101288819,
    0.019830326636332028,
    0.019905188596781104,
    0.019950199166862841,
    0.019965218274992248,
    0.019950199166862841,
    0.019905188596781104,
    0.019830326636332028,
    0.019725846101288819,
    0.019592071599501125,
    0.019429418204303113,
    0.019238389759765800,
    0.019019576825867947,
    0.018773654273367420,
    0.018501378539810983,
    0.018203584559716979,
    0.017881182383493149,
    0.017535153501103892,
    0.017166546887869318,
    0.016776474791055797,
    0.016366108277098043,
    0.015936672560369614,
    0.015489442135386881,
    0.015025735735186426,
    0.014546911139352912,
    0.014054359855790193,
    0.013549501700820601,
    0.013033779302562583,
    0.012508652552774890,
    0.011975593032464912,
    0.011436078436539264,
    0.010891587022627668,
    0.010343592108937174,
    0.009793556645595150,
    0.009242927883419041,
    0.008693132163411567,
    0.008145569849526276,
    0.007601610426384373,
    0.007062587782654920,
    0.006529795699742466,
    0.006004483564265146,
    0.005487852321559078,
    0.004981050686118592,
    0.004485171623483914,
    0.004001249116626688,
    0.003530255228366684,
    0.003073097469789486,
    0.002630616483032971,
    0.002203584045179127,
    0.001792701398335993,
    0.001398597909331569,
    0.001021830060775982,
    0.000662880773589522,
    0.000322159059450752,
    0.000000000000000001,
    -0.000303334951952833,
    -0.000587657355512251,
    -0.000852850856865070,
    -0.001098869965763655,
    -0.001325738543930948,
    -0.001533548017297868,
    -0.001722455325210333,
    -0.001892680620886389,
    -0.002044504738458278,
    -0.002178266442894981,
    -0.002294359479963247,
    -0.002393229444145818,
    -0.002475370483091317,
    -0.002541321857718479,
    -0.002591664377536210,
    -0.002627016731069256,
    -0.002648031731496151,
    -0.002655392497711923,
    -0.002649808591023195,
    -0.002632012127569288,
    -0.002602753886341418,
    -0.002562799432345805,
    -0.002512925274028314,
    -0.002453915073551965,
    -0.002386555927898205,
    -0.002311634738053157,
    -0.002229934682745978,
    -0.002142231812333790,
    -0.002049291777482159,
    -0.001951866706278179,
    -0.001850692242341569,
    -0.001746484755374531,
    -0.001639938734420841,
    -0.001531724372895900,
    -0.001422485353209745,
    -0.001312836837542114,
    -0.001203363670049807,
    -0.001094618794499867,
    -0.000987121890034750,
    -0.000881358226495441,
    -0.000777777739462615,
    -0.000676794323931742,
    -0.000578785344322494,
    -0.000484091357342654,
    -0.000393016043088375,
    -0.000305826338672358,
    -0.000222752767634882,
    -0.000143989957415198,
    -0.000069697336247377,
    0.000000000000000000,
    0.000065010261293198,
    0.000125273788160487,
    0.000180761739242710,
    0.000231474653767861,
    0.000277440901501588,
    0.000318715033091691,
    0.000355376044004699,
    0.000387525565446236,
    0.000415285995764155,
    0.000438798585855176,
    0.000458221492033064,
    0.000473727809671115,
    0.000485503600706004,
    0.000493745927786584,
    0.000498660907473236,
    0.000500461794444300,
    0.000499367108150093,
    0.000495598812776237,
    0.000489380560741255,
    0.000480936009263626,
    0.000470487218795956,
    0.000458253141344114,
    0.000444448205872873,
    0.000429281007152519,
    0.000412953103529159,
    0.000395657928211038,
    0.000377579817760223,
    0.000358893160569588,
    0.000339761667195315,
    0.000320337763510927,
    0.000300762106756335,
    0.000281163223679941,
    0.000261657269119383,
    0.000242347902542027,
    0.000223326279275218,
    0.000204671152403140,
    0.000186449080596567,
    0.000168714736477861,
    0.000151511309510219,
    0.000134870996840645,
    0.000118815575023601,
    0.000103357045109305,
    0.000088498343199344,
    0.000074234108254511,
    0.000060551498686721,
    0.000047431049079466,
    0.000034847558259864,
    0.000022770999889941,
    0.000011167446754809,
    0.000000000000000000,
    -0.000010770285292045,
    -0.000021183486597150,
    -0.000031279920668665,
    -0.000041099278318167,
    -0.000050679794726066,
]

def avg(nums):
    return sum(nums)/len(nums)

# get data file name
try:
    fileName = sys.argv[1]
except IndexError:
    print("Error: no filename given.")
    quit()

# open and read data file
t = [] # column 0
data = [] # column 1

with open(fileName) as f:
    # open the csv file
    reader = csv.reader(f)
    for row in reader:
        # read the rows 1 one by one
        t.append(float(row[0])) # leftmost column
        data.append(float(row[1])) # second column

# Moving Average Filter 
def MAF(data, X):
    maf = []
    for x in range(X-1): # prefill zeros to make maf the same length as data
        maf.append(0) 
    for i in range(len(data)):
        if i >= (X-1):
            temp = []
            for x in range(X):
                temp.append(data[i-x])
            maf.append(avg(temp))
    return maf

# Infinite Impulse Response filter (IIR)
def IIR(data, A, B):
    iir = []
    prevAvg = 0
    for x in range(len(data)):
        iir.append((A * prevAvg) + (B * data[x]))
        prevAvg = avg(iir)
    return iir


# Low Pass Windowed-sinc FIR (w/ Hamming window)
def FIR(data, h):
    fir = []
    for x in range(len(h)-1): # prefill zeros to make fir the same length as data
        fir.append(0) 

    for i in range(len(data)):
        if i >= (len(h)-1):
            temp = []
            for x in range(len(h)):
                temp.append(data[i-x] * h[x]) # apply coefficient
            fir.append(sum(temp))
    return fir


# calculate sample rate
sampleRate = len(data) / (t[-1] - t[0])
print(f"Data Sample Rate: {sampleRate:.2f}")

# Fast Fourier Transform of raw data
n = len(data) # length of the signal
k = np.arange(n)
T = n/sampleRate
frq = k/T # two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range
Y = np.fft.fft(data)/n # fft computing and normalization
Y = Y[range(int(n/2))]

# Fast Fourier Transform of filtered data
# filtered = MAF(data, numSamplesToAverage) # Moving Average Filter
# filtered = IIR(data, A, B) # Infinite Impulse Response filter
filtered = FIR(data, h)
filtered = FIR(filtered, h)

n2 = len(filtered)            # length of the signal
k2 = np.arange(n2)
T2 = n2/sampleRate
frq2 = k2/T2                  # two sides frequency range
frq2 = frq2[range(int(n2/2))] # one side frequency range
Y2 = np.fft.fft(filtered)/n2  # fft computing and normalization
Y2 = Y2[range(int(n2/2))]

# plot results
fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(t,data,'black')
ax1.plot(t,filtered,'r')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax2.loglog(frq,abs(Y),'black') # plotting the fft
ax2.loglog(frq,abs(Y2), 'r')
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
fig.suptitle(f'*Double* Windowed-sinc FIR filter, Hamming window with {len(h)} coeffs, at 14 Hz cutoff freq and 14 Hz transition bandwidth')
plt.show()
