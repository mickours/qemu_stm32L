import Tkinter
import socket
import threading
from Tkinter import PhotoImage

class UI(Tkinter.Tk):
    
    def __init__(self,parent):
        Tkinter.Tk.__init__(self,parent)
        self.parent = parent
        self.initialize()

    def initialize(self):
        
        ################ Menu ################
        self.menu1 = Tkinter.Menu(tearoff=0)
        
        self.fichier = Tkinter.Menu(tearoff=0)
        self.menu1.add_cascade(label="Fichier",menu=self.fichier)
        #self.fichier.add_command(label="  Reinitialiser", command=app.reinit)
        self.fichier.add_command(label="  Quitter", command=self.quitter)
        self.fichier = Tkinter.Menu(tearoff=0)
        self.menu1.add_cascade(label="Aide",menu=self.fichier)
        self.fichier.add_command(label="  Utilisation", command=self.use)
        self.fichier.add_command(label="  A propos", command=self.apropos)
        
        self.config(menu=self.menu1)
        ################ /Menu ###############
        
        self.grid()
        
        self.labelIP = Tkinter.Label(self,anchor='w',text="IP :")
        self.labelIP.grid(column=0,row=0,sticky='EW')
        
        self.entryVariableIP = Tkinter.StringVar()
        self.entryVariableIP.set("Not supported")
        self.entryIP = Tkinter.Entry(self,textvariable=self.entryVariableIP)
        self.entryIP.grid(column=0,row=1,sticky='EW')
        
        self.labelPort = Tkinter.Label(self,anchor='w',text="Port :")
        self.labelPort.grid(column=0,row=2,sticky='EW') 
        
        self.entryVariablePort = Tkinter.StringVar()
        self.entryVariablePort.set(u"4242")
        self.entryPort = Tkinter.Entry(self,textvariable=self.entryVariablePort)
        self.entryPort.grid(column=0,row=3,sticky='EW')
        
        button = Tkinter.Button(self,text="Lancer",command=self.OnButtonConnexionClick)
        
        button.grid(column=0,row=4)
        
        self.labelVariableCo = Tkinter.StringVar()
        self.labelVariableCo.set(u"Non connecte")
        self.labelConnexion = Tkinter.Label(self,textvariable=self.labelVariableCo)
        self.labelConnexion.grid(column=0,row=5,sticky='EW')
        
        self.labelVariableLog = Tkinter.StringVar()
        self.labelVariableLog.set("Log : ")
        self.labelLog = Tkinter.Label(self,textvariable=self.labelVariableLog)
        self.labelLog.grid(column=0,row=6,sticky='EW')
        
        self.carte=PhotoImage(file="carte.gif")
        self.ledBlanche=PhotoImage(file="LEDblanche.gif")
        self.ledBleue=PhotoImage(file="LEDbleue.gif")
        self.ledVerte=PhotoImage(file="LEDverte.gif")
        
        self.canvas = Tkinter.Canvas(self,width=self.carte.width(),height=self.carte.height())
        self.canvas.grid(column=1,row=1,rowspan=70)
        self.cartei = self.canvas.create_image(0,0, anchor = "nw", image=self.carte)
        self.ledGauche = self.canvas.create_image(35,400, anchor = "nw", image=self.ledBlanche)
        self.ledDroite = self.canvas.create_image(190,400, anchor = "nw", image=self.ledBlanche)
        
        #self.grid_columnconfigure(1,weight=1)
        #self.grid_columnconfigure(0,weight=1) INUTILE
        self.grid_rowconfigure(0,weight=1) #Gestion verticale de l'agrandissement
        self.resizable(False,False) #Pas redimentionable !
        
        self.update()
        #self.geometry(self.geometry())
        
    def OnButtonConnexionClick(self):
        app.event.set()
    
    def quitter(self):
        if hasattr(app,'thread') :
            app.close_conn()
        self.quit()
            
    def printlog(self, m):
        self.labelVariableLog.set(m)
        self.update()
        
    def apropos(self):
        pass
    
    def use(self):
        pass
        
#***************************************#
#Controlleur							#
#***************************************#
class Ctrl:
    def __init__(self,parent):
        self.ui = UI(parent)
        self.on = False
        self.event = threading.Event()
        self.thread=threading.Thread(target=self.fct_thread)
        self.thread.setDaemon(True)
        self.thread.start()
        
    def close_conn(self):
        if hasattr(self,'conn') :
            self.conn.close()
            self.ui.labelVariableCo.set("Non connecte")
        
        
    def traiterrecv(self):
        while 1:
            data = self.conn.recv(1)
            if not data: break
            if ord(data) == 0:
                self.ui.canvas.itemconfigure(self.ui.ledGauche, image=self.ui.ledBlanche)
                self.ui.canvas.itemconfigure(self.ui.ledDroite, image=self.ui.ledBlanche)
                self.ui.printlog("Log : Led off") 
            elif ord(data) == 192:
                self.ui.canvas.itemconfigure(self.ui.ledGauche, image=self.ui.ledBleue)
                self.ui.canvas.itemconfigure(self.ui.ledDroite, image=self.ui.ledVerte)
                self.ui.printlog("Log : Led on")
            else:
                self.ui.printlog("Log : Data non reconnue")
            self.ui.update()
    
    def clicked(self):
    	if not hasattr(self,'serversocket') :
		    self.serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		    self.serversocket.bind((socket.gethostname(), int(self.ui.entryVariablePort.get())))
		    self.serversocket.listen(1)
		    self.ui.labelVariableCo.set(u"En attente de connexion \nsur le port " + self.ui.entryVariablePort.get())
		    self.ui.update()
		    self.conn, self.addr = self.serversocket.accept()
		    self.ui.labelVariableCo.set("Connecte")
		    self.ui.update()
		    self.traiterrecv()
		    self.close_conn()
                
    def tester(self):
        self.ui.canvas.itemconfigure(self.ui.ledGauche, image=self.ui.ledBleue)
        self.ui.canvas.itemconfigure(self.ui.ledDroite, image=self.ui.ledVerte)
        
    def fct_thread(self):
        while 1:
            while not self.event.isSet():
                self.event.wait()
            self.clicked()
            self.event.clear()
           
    
if __name__ == "__main__":
    app = Ctrl(None)
    app.ui.title('STM32LDiscovery UI')
    app.ui.mainloop()
        
