import matplotlib.pyplot as plt
import numpy as np
import serial
import csv
from sympy import true

sys_init = 'zzzzzzzzzzzzzzzzzzzzzzzzzzzzzz'
freqrange = np.arange(10000, 101800, 1800)



s=serial.Serial('COM6', 9600)


#plt.figure()
plt.ion()
plt.show()

xs=np.array([])

i=0
xs=np.append(xs, freqrange)
while True:
    impedance_str = np.array([])
    phase_str = np.array([])
    userinput = input('start? ')
    while True:
        if userinput=='y':
            s.write(sys_init.encode())
            userinput=''
        data = s.readline()
        data_list = data.split(b',')
        impedance = float(data_list[0])

        data1 = data_list[1]
        data1_list = data1.split(b'\n')
        phase = float(data1_list[0])

        impedance_str = np.append(impedance_str, impedance)
        phase_str = np.append(phase_str, phase)
     
        i+=1
        print(i)
        print(impedance_str)
        print(phase_str)

        if i>=51:
            i=0
            break 
    '''
    with open('rcal_10k_51k_par106.csv', 'w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(["frequency", "impedance", "phase"])
        for val_count in range(51):
            writer.writerow([freqrange[val_count], impedance_str[val_count], phase_str[val_count]])'''

    userinput=input('plot? ')
    while userinput=='y':
        
        
        plt.close('all')
        fig, impedance_plot = plt.subplots()
        phase_plot = impedance_plot.twinx()


        impedance_plot.set_xlim(0, 100000)
        impedance_plot.set_ylim(0, np.max(impedance_str)*2)
        phase_plot.set_ylim(-90, 90)

        impedance_plot.set_title('AD5933')
        impedance_plot.set_xlabel('Frequency')
        impedance_plot.set_ylabel('Impedance')
        phase_plot.set_ylabel('Phase')

        

        line1, = impedance_plot.plot(xs, impedance_str, 'b', label='Impedance')
        line2, = phase_plot.plot(xs, phase_str, 'r', label='Phase')
        '''
        # Create a legend for the first line.
        first_legend = impedance_plot.legend(handles=[line1], loc='best')

        # Add the legend manually to the Axes.
        impedance_plot.add_artist(first_legend)

        # Create another legend for the second line.
        impedance_plot.legend(handles=[line2], loc='best')
        '''
        impedance_plot.legend(handles=[line1, line2])

        userinput=input('plot? ')
        if userinput=='n':
            break