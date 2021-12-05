#Read data from serial port while running multiconnect_raw_data.ino and print to csv
#extrapolate breath rate from RR-Interval data from polar h10


from __future__ import division
import serial
import time
import csv


#define serial port
ser = serial.Serial(port='COM3', baudrate=115200)
ser.flushInput()

steps=0
i=0
RR=[]
BP=[]
HR=0
EMAlow=0
EMAhigh=0

B=0
BR=0
t=0
BRA=[]
avg=0
L=0
Q=0
while True:
    try:
        #ser_bytes = ser.readline()
        try:
            if L>2:
                L=0

            #read raw data from serail port
            ser_bytes = ser.readline()
            #print(ser_bytes)
            decoded_bytes = float(ser_bytes[0:len(ser_bytes) - 2].decode("utf-8"))
            #print(decoded_bytes)

            #assign data to correct array/varible
            if L==0:
                HR = decoded_bytes

            if L==1:
                RR.append(decoded_bytes)

            if L==2:
                steps=decoded_bytes

            #print("HR: ",HR)
            #print("RR: ",RR)
            L+=1




            #Take dervitave of RR-interval array
            
            # print(RR)
            RRD = (RR[i - 1] - RR[i]) / 2
            # print(RRD*2)
            
            #Run RR-interval dervitave through bandpass filter
            EMAlow = (.2 * RRD) + (.8 * EMAlow)
            EMAhigh = (1.2 * RRD) + (-.2 * EMAhigh)

            bandpass = EMAhigh - EMAlow
            BP.append(bandpass)
            #print(bandpass)
               
            #Use peak/vally detection to detect breath
            if BP[i-1]<BP[i-2] and BP[i-1]<BP[i] and BP[i-1]<0:
                B+=1
                
                #calculate breaths per min
                BR=60/(i-t-1)
                BR=round(BR,2)
                #BRA.append(BR)
                
                #add breaths per min to array
                while Q<(i-t-1):

                    #BRA.pop(0)
                    BRA.append(BR)
                    Q+=1

                Q=0
                
                #calculate average of the array
                avg = sum(BRA) / len(BRA)
                avg = round(avg, 2)
                t=i
            
            #every 15 seconds clear the array and add the current average as the first element in the array
            if i & 15 == 0:
                BRA.clear()
                BRA.append(avg)






            #display data
            
            #print(B)
            print("HR:",HR," BR:",BR," BRA:",avg," Breaths:",B," Steps:",steps)
            i += 1




        except:
            #print("connecting")
            continue
            
        #Write data to csv file
        
        with open("test_data.csv", "a", newline="") as f:

            writer = csv.writer(f, delimiter=",")

            writer.writerow([HR,BR,avg,steps])
    except:

        print("Keyboard Interrupt")

        break
