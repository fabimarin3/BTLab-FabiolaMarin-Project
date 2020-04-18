# -*- coding: utf-8 -*-
"""
@author: Fabi
"""
import time
import csv
import serial



ser = serial.Serial('/dev/ttyACM0', 9600)
count = 0
elapsed_time = 0

while True:
    line = ser.readline()
    print(line)
    with open(r'Baseline_Data0.csv', 'a') as b:
       Datos = csv.writer(b,delimiter=' ')
       Datos.writerow([line])        
        

    time.sleep(1)

