from tkinter import *
from matplotlib.backends.backend_tkagg import (
    FigureCanvasTkAgg, NavigationToolbar2TkAgg)
from matplotlib import figure
from matplotlib.backend_bases import key_press_handler
#from matplotlib.figure import FigureCanvasTkAgg

import numpy as np
import serial
import time
import csv

def Timestamp():
    date_now = time.strftime('%d-%m-%y')
    time_now = time.strftime('_%H-%M-%S')
    return [date_now,time_now]
    
def csvNamer():
    dt = Timestamp()
    fileName = 'Interferogram_' + dt[0] + dt[1] + '.csv'
    return fileName

def Reader():
    global ser
    if (ser.isOpen()):
        try:                    
            x = ser.readline().decode()
            x = (x)
            return x
        except:
            return "Unable to print\n"
    else: 
        return "Cannot open serial port\n"
    



root = Tk()
root.wm_title("Embedding in Tk")
root.title("Welcome to LikeGeeks app")
fig = figure.Figure(figsize=(5, 4), dpi=100)
t = np.arange(0, 3, .01)
fig.add_subplot(111).plot(t, 2 * np.sin(2 * np.pi * t))

canvas = FigureCanvasTkAgg(fig, master=root)  # A tk.DrawingArea.
canvas.draw()
canvas.get_tk_widget().pack(side=TOP, fill=BOTH, expand=1)

toolbar = NavigationToolbar2TkAgg(canvas, root)
toolbar.update()
canvas.get_tk_widget().pack(side=TOP, fill=BOTH, expand=1)


def on_key_press(event):
    print("you pressed {}".format(event.key))
    key_press_handler(event, canvas, toolbar)


canvas.mpl_connect("key_press_event", on_key_press)


def _quit():
    root.quit()     # stops mainloop
    root.destroy()  # this is necessary on Windows to prevent
                    # Fatal Python Error: PyEval_RestoreThread: NULL tstate


button = Button(master=root, text="Quit", command=_quit)
button.pack(side=BOTTOM)
root.geometry('500x500')
 
lbl = Label(root, text="Hello")
 

 
txt = Entry(root,width=10)
 

 
def clicked():
 
    lbl.configure(text="Button was clicked !!")
def water():
    lbl.configure(text="Plants have been watered")
def readInMoistureLevels():
    #this function will use max's serial monitoring to read in moisture values from the sensor
 
    #Open serial port to read from Arduino serial monitor    
    arduino = serial.Serial('COM6', 9600, timeout=.1) #COM9 = left hand side USB
    time.sleep(1) #give the connection a second to settle
    #Open file 
    name = csvNamer()
    count = 0
    with open(name, 'w') as dataFile:
        writer = csv.writer(dataFile) #Open writer to write to csv
        while True:
            data = arduino.readline()[:-2].decode() #[:~2] removes newline chars
            #If current data not zero, print and write it
            if data:
                print(data)    
                count = count + 1
                writer.writerow([count,data])
                dataFile.flush()
                #time.sleep(5)0
                lbl.set(data)

btn = Button(root, text="Click Me", command=clicked)
 


btn2 = Button (root, text="Water plants", command=water)
btn2.pack(side=BOTTOM)

#root.after(10,poll)
root.mainloop()
