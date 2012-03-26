import Tkinter
import socket
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
        self.fichier.add_command(label="  Reinitialiser", command=self.initialize)
        self.fichier.add_command(label="  Quitter", command=self.quit)
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
        self.labelVariableCo.set("Initialisation...")
        app.clicked()
        
    def apropos(self):
        pass
    
    def use(self):
        pass

class Ctrl:
    def __init__(self,parent):
        self.ui = UI(parent)
        
        
    def traiterrecv(self):
        self.ui.canvas.itemconfigure(self.ledGauche, image=self.ledBleue)
        self.ui.canvas.itemconfigure(self.ledDroite, image=self.ledVerte)
        
        while 1:
            data = self.conn.recv(4)
            if not data: break
    
            index = 0
            while index < len(data):
                print ord(data[index])
                index = index +1
        
        
    def clicked(self):
        self.serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.serversocket.bind((socket.gethostname(), int(self.ui.entryVariablePort.get())))
        self.serversocket.listen(1)
        self.ui.labelVariableCo.set(u"En attente de connexion \nsur le port " + self.ui.entryVariablePort.get())
        self.ui.update()
        self.conn, self.addr = self.serversocket.accept()
        self.ui.labelVariableCo.set("Connecte a : " +self.addr)
        self.ui.update()
        self.traiterrecv()
        self.conn.close()
        self.ui.labelVariableCo.set("Non connecte")
        
           
    
if __name__ == "__main__":
    app = Ctrl(None)
    app.ui.title('STM32LDiscovery UI')
    app.ui.mainloop(0)
        