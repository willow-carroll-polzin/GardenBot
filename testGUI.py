from tkinter import *
from matplotlib.backends.backend_tkagg import (
    FigureCanvasTkAgg, NavigationToolbar2TkAgg)
from matplotlib import figure
from matplotlib.backend_bases import key_press_handler

from random import (seed, random)

import numpy as np
import serial
import time
import csv
import datetime
import queue
import logging
import signal
import threading
from tkinter.scrolledtext import ScrolledText
from tkinter import ttk, VERTICAL, HORIZONTAL, N, S, E, W

logger = logging.getLogger(__name__)

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
#root.wm_title("Embedding in Tk")
#root.title("GardenBot Monitoring System")



def on_key_press(event):
    print("you pressed {}".format(event.key))
    key_press_handler(event, canvas, toolbar)

def _quit():
    root.quit()     # stops mainloop
    root.destroy()  # this is necessary on Windows to prevent
                    # Fatal Python Error: PyEval_RestoreThread: NULL tstate


button = Button(master=root, text="Quit", command=_quit)
button.grid(row=11,column=6)
root.geometry('800x500')
 
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
def randomValueGenerator():
    return random()
 

lightLabel = Label(root, text="Amount of light per day (hrs)")
lightLabel.grid(row=0, columnspan = 2, column = 1)
lightScale = Scale (root, from_=0, to_=24, orient=HORIZONTAL)
lightScale.grid(row=1, columnspan = 2, column = 1)

moistLabel = Label(root, text="Minimum Moisture Level Threshold (%)")
moistLabel.grid(row=2, columnspan = 2, column = 1)
moistScale = Scale (root, from_=0, to_=100, orient=HORIZONTAL, resolution=5)
moistScale.grid(row=3, columnspan = 2, column = 1)

lightLabel = Label(root, text="Amount of water per watering (mL)?")
lightLabel.grid(row=4, columnspan=2,column = 1)
lightScale = Scale (root, from_=0, to_=250, orient=HORIZONTAL, resolution =25)
lightScale.grid(row=5, columnspan=2,column = 1)



class Clock(threading.Thread):
    """Class to display the time every seconds
    Every 5 seconds, the time is displayed using the logging.ERROR level
    to show that different colors are associated to the log levels
    """

    def __init__(self):
        super().__init__()
        self._stop_event = threading.Event()

    def run(self):
        logger.debug('Clock started')
        previous = -1
        while not self._stop_event.is_set():
            now = datetime.datetime.now()
            if previous != now.second:
                previous = now.second
                if now.second % 5 == 0:
                    level = logging.ERROR
                else:
                    level = logging.INFO
                logger.log(level, now)
            time.sleep(0.2)

    def stop(self):
        self._stop_event.set()


class QueueHandler(logging.Handler):
    """Class to send logging records to a queue
    It can be used from different threads
    The ConsoleUi class polls this queue to display records in a ScrolledText widget
    """
    # Example from Moshe Kaplan: https://gist.github.com/moshekaplan/c425f861de7bbf28ef06
    # (https://stackoverflow.com/questions/13318742/python-logging-to-tkinter-text-widget) is not thread safe!
    # See https://stackoverflow.com/questions/43909849/tkinter-python-crashes-on-new-thread-trying-to-log-on-main-thread

    def __init__(self, log_queue):
        super().__init__()
        self.log_queue = log_queue

    def emit(self, record):
        self.log_queue.put(record)

class ConsoleUi:
    def __init__(self, frame):
        self.frame = frame
        # Create a ScrolledText wdiget
        self.scrolled_text = ScrolledText(frame, state='disabled', height=12)
        self.scrolled_text.grid(row=0, column=0, sticky=(N, S, W, E))
        self.scrolled_text.configure(font='TkFixedFont')
        self.scrolled_text.tag_config('INFO', foreground='black')
        self.scrolled_text.tag_config('DEBUG', foreground='gray')
        self.scrolled_text.tag_config('WARNING', foreground='orange')
        self.scrolled_text.tag_config('ERROR', foreground='red')
        self.scrolled_text.tag_config('CRITICAL', foreground='red', underline=1)
        # Create a logging handler using a queue
        self.log_queue = queue.Queue()
        self.queue_handler = QueueHandler(self.log_queue)
        formatter = logging.Formatter('%(asctime)s: %(message)s')
        self.queue_handler.setFormatter(formatter)
        logger.addHandler(self.queue_handler)
        # Start polling messages from the queue
        self.frame.after(100, self.poll_log_queue)

    def display(self, record):
        msg = self.queue_handler.format(record)
        self.scrolled_text.configure(state='normal')
        self.scrolled_text.insert(END, msg + '\n', record.levelname)
        self.scrolled_text.configure(state='disabled')
        # Autoscroll to the bottom
        self.scrolled_text.yview(END)

    def poll_log_queue(self):
        # Check every 100ms if there is a new message in the queue to display
        while True:
            try:
                record = self.log_queue.get(block=False)
            except queue.Empty:
                break
            else:
                self.display(record)
        self.frame.after(100, self.poll_log_queue)

class LoggingUi:

    def __init__(self, frame):
        self.frame = frame
        # Create a combobbox to select the logging level
        values = ['DEBUG', 'INFO', 'WARNING', 'ERROR', 'CRITICAL']
        self.level = StringVar()
        ttk.Label(self.frame, text='Level:').grid(column=0, row=0, sticky=W)
        self.combobox = ttk.Combobox(
            self.frame,
            textvariable=self.level,
            width=25,
            state='readonly',
            values=values
        )
        self.combobox.current(0)
        self.combobox.grid(column=1, row=0, sticky=(W, E))
        # Create a text field to enter a message
        self.message =StringVar()
        ttk.Label(self.frame, text='Message:').grid(column=0, row=1, sticky=W)
        ttk.Entry(self.frame, textvariable=self.message, width=25).grid(column=1, row=1, sticky=(W, E))
        # Add a button to log the message
        self.button = ttk.Button(self.frame, text='Submit', command=self.submit_message)
        self.button.grid(column=1, row=2, sticky=W)

    def submit_message(self):
        # Get the logging level numeric value
        lvl = getattr(logging, self.level.get())
        logger.log(lvl, self.message.get())


class ThirdUi:

    def __init__(self, frame):
        self.frame = frame
        ttk.Label(self.frame, text='This is just an example of a third frame').grid(column=0, row=1, sticky=W)
        ttk.Label(self.frame, text='With another line here!').grid(column=0, row=4, sticky=W)
        fig = figure.Figure(figsize=(5, 4), dpi=100)
        t = np.arange(0, 3, .01)
        fig.add_subplot(111).plot(t, 2 * np.sin(2 * np.pi * t))

        canvas = FigureCanvasTkAgg(fig, master=self.frame)  # A tk.DrawingArea.
        canvas.draw()
        canvas.get_tk_widget().grid(row=0)
        
        toolbar_frame = Frame(self.frame)
        toolbar_frame.grid(row=11, column = 0, sticky = W)
        toolbar = NavigationToolbar2TkAgg(canvas, toolbar_frame)
        toolbar.update()
        canvas.get_tk_widget().grid(row=0, rowspan = 10)
    
        
class App:

    def __init__(self, root):
        self.root = root
        root.title("GardenBot Monitoring System")
        root.columnconfigure(0, weight=1)
        root.rowconfigure(0, weight=1)

        # Create the FRAMES (no PANES PLS)
        graph_frame = Frame(width=550, height=470)
        graph_frame.grid(row=0,column=0,columnspan=2,rowspan=10)
        logging_frame = Frame(width=200, height=100)
        logging_frame.grid(row=10,column=2)
        control_frame = Frame(width=200,height=470)
        control_frame.grid(row=0,column=2, rowspan=10)
        
        #Design and Boxes and Sections
        lightLabel = Label(control_frame, text="Amount of light per day (hrs)")
        lightLabel.grid(row=0, columnspan = 2, column = 1)
        lightScale = Scale (control_frame, from_=0, to_=24, orient=HORIZONTAL)
        lightScale.grid(row=1, columnspan = 2, column = 1)

        moistLabel = Label(control_frame, text="Minimum Moisture Level Threshold (%)")
        moistLabel.grid(row=2, columnspan = 2, column = 1)
        moistScale = Scale (control_frame, from_=0, to_=100, orient=HORIZONTAL, resolution=5)
        moistScale.grid(row=3, columnspan = 2, column = 1)

        lightLabel = Label(control_frame, text="Amount of water per watering (mL)?")
        lightLabel.grid(row=4, columnspan=2,column = 1)
        lightScale = Scale (control_frame, from_=0, to_=250, orient=HORIZONTAL, resolution =25)
        lightScale.grid(row=5, columnspan=2,column = 1)
        

        
        
        
        vertical_pane = ttk.PanedWindow(self.root, orient=VERTICAL)
        vertical_pane.grid(row=0, column=0, sticky="nsew")
        vertical_pane.configure(height=470,width=550)
        horizontal_pane = ttk.PanedWindow(vertical_pane, orient=HORIZONTAL)
        vertical_pane.add(horizontal_pane)
        form_frame = ttk.Labelframe(horizontal_pane, text="MyForm")
        form_frame.columnconfigure(1, weight=1)
        horizontal_pane.add(form_frame, weight=1)
        console_frame = ttk.Labelframe(horizontal_pane, text="Console")
        console_frame.columnconfigure(0, weight=1)
        console_frame.rowconfigure(0, weight=1)
        horizontal_pane.add(console_frame, weight=1)
        third_frame = ttk.Labelframe(vertical_pane, text="Data from Past 24h")
        
        vertical_pane.add(third_frame, weight=1)
        # Initialize all frames
        self.form = ThirdUi(form_frame)
        self.console = ConsoleUi(console_frame)
        self.third = LoggingUi(third_frame)
        self.clock = Clock()
        self.clock.start()
        self.root.protocol('WM_DELETE_WINDOW', self.quit)
        self.root.bind('<Control-q>', self.quit)
        signal.signal(signal.SIGINT, self.quit)
                
                



    def quit(self, *args):
        self.clock.stop()
        self.root.destroy()
        
def main():
    logging.basicConfig(level=logging.DEBUG)
    root = Tk()
    app = App(root)
    app.root.mainloop()
if __name__ == '__main__':
    main()
