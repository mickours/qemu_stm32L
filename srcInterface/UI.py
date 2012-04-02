
##################################################################
#     Interface pour l'emulation de la carte STM32LDiscovery     #  
#                            avec Qemu                           #
#        EID Timothee - MERCIER Michael - CLAVELIN Aurelien      #
##################################################################

import Tkinter
import socket
import threading
import struct

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
        self.fichier.add_command(label="  Quitter", command=self.quitter)
        self.fichier = Tkinter.Menu(tearoff=0)
        self.menu1.add_cascade(label="Aide",menu=self.fichier)
        self.fichier.add_command(label="  A propos", command=self.apropos)
        
        self.config(menu=self.menu1)
        ################ /Menu ###############
        
        self.grid()
        
        '''
        self.labelIP = Tkinter.Label(self,anchor='w',text="IP :")
        self.labelIP.grid(column=0,row=0,sticky='EW')
        
        self.entryVariableIP = Tkinter.StringVar()
        self.entryVariableIP.set("Not supported")
        self.entryIP = Tkinter.Entry(self,textvariable=self.entryVariableIP)
        self.entryIP.grid(column=0,row=1,sticky='EW')
        '''
        
        self.labelPort = Tkinter.Label(self,anchor='w',text="Port :")
        self.labelPort.grid(column=0,row=2,sticky='EW') 
        
        self.entryVariablePort = Tkinter.StringVar()
        self.entryVariablePort.set(u"4242")
        self.entryPort = Tkinter.Entry(self,textvariable=self.entryVariablePort)
        self.entryPort.grid(column=0,row=3,sticky='EW')
        
        button = Tkinter.Button(self,text="Lancer",command=self.OnButtonConnexionClick)
        button.grid(column=0,row=4,sticky="EW")
        
        self.labelLog = Tkinter.Label(self,anchor='w',text="Log : ")
        self.labelLog.grid(column=0,row=7,sticky='EW')
        
        self.log = Tkinter.Text(width=20,height=20)
        self.log.grid(column=0,row=8,sticky='EWNS',rowspan=50)
        self.log.insert(Tkinter.END,"")
        
        button2 = Tkinter.Button(self,text="Vider",command=self.Clear)
        button2.grid(column=0,row=59,sticky="EW")
        
        scrollbar = Tkinter.Scrollbar(self.log)
        scrollbar.pack(side='right', fill='y')
        
        self.log.config(yscrollcommand=scrollbar.set)
        scrollbar.config(command=self.log.yview)

        self.carte=PhotoImage(file="carte.gif")
        self.ledBlanche=PhotoImage(file="LEDblanche.gif")
        self.ledBleue=PhotoImage(file="LEDbleue.gif")
        self.ledVerte=PhotoImage(file="LEDverte.gif")
        self.user=PhotoImage(file="user.gif")
        
        self.canvas = Tkinter.Canvas(self,width=self.carte.width(),height=self.carte.height())
        self.canvas.grid(column=2,row=1,rowspan=70)
        self.canvas.create_image(0,0, anchor = "nw", image=self.carte)
        
        self.canvas.create_image(70,390,image=self.user, tag="user", anchor = "nw")
        self.canvas.create_image(150,390,image=self.user, tag="reset", anchor = "nw")
        self.canvas.tag_bind("reset", "<Button-1>",self.Reset)
        self.canvas.tag_bind("user", "<Button-1>",self.User)
        
        self.ledGauche = self.canvas.create_image(35,400, anchor = "nw", image=self.ledBlanche)
        self.ledDroite = self.canvas.create_image(190,400, anchor = "nw", image=self.ledBlanche)
        
        #self.grid_columnconfigure(1,weight=1)
        #self.grid_columnconfigure(0,weight=1) INUTILE
        self.grid_rowconfigure(0,weight=1) #Gestion verticale de l'agrandissement
        self.resizable(False,False) #Pas redimentionable !
        
        
        #self.geometry(self.geometry())
        self.update()
        
    def OnButtonConnexionClick(self):
        app.event.set()
        
    def User(self,none):
        self.log.insert(Tkinter.END,"User button clicked\n")
        app.user_clicked()
        self.log.see(Tkinter.END)
        
    def Reset(self,none):
        self.log.insert(Tkinter.END,"Reset button clicked\n")
        self.log.see(Tkinter.END)
        
    def Clear (self):
        self.log.delete(1.0, Tkinter.END)
    
    def quitter(self):
        if hasattr(app,'thread') :
            app.close_conn()
        self.quit()
            
    def printlog(self, m):
        self.labelVariableLog.set(m)
        self.update()
        
    def apropos(self):
        self.apropos = Fenetre()
 
        
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
            self.ui.log.insert(Tkinter.END,"Non connecte\n")
            
    def user_clicked(self):
        self.conn.send(struct.pack('B',1))
        self.conn.send(struct.pack('B',0))
        
    def traiterrecv(self):
        while 1:
            data = self.conn.recv(1)
            if not data: break
            if ord(data) == 0:
                self.ui.canvas.itemconfigure(self.ui.ledGauche, image=self.ui.ledBlanche)
                self.ui.canvas.itemconfigure(self.ui.ledDroite, image=self.ui.ledBlanche)
                self.ui.log.insert(Tkinter.END,"Donnee recue : " + str(ord(data)) + "\n" + "=>LEDs off\n")
            elif ord(data) == 192:
                self.ui.canvas.itemconfigure(self.ui.ledGauche, image=self.ui.ledBleue)
                self.ui.canvas.itemconfigure(self.ui.ledDroite, image=self.ui.ledVerte)
                self.ui.log.insert(Tkinter.END,"Donnee recue : " + str(ord(data)) + "\n" + "=>LEDs on\n")
            else:
                self.ui.log.insert(Tkinter.END,"Donnee recue : " + str(ord(data)) + "\n" + "=>non reconnue\n")
            self.ui.log.see(Tkinter.END)
            self.ui.update()
    
    def clicked(self):
        self.serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.serversocket.bind((socket.gethostname(), int(self.ui.entryVariablePort.get())))
        self.serversocket.listen(1)
        self.ui.log.insert(Tkinter.END,"En attente de connexion sur le port " + self.ui.entryVariablePort.get()+ "\n")
        self.ui.update() 
        self.conn, self.addr = self.serversocket.accept()
        self.ui.log.insert(Tkinter.END,"Connecte \n")
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
 
#####################
#Fenetre (A propos) #
#####################       
class Fenetre(Tkinter.Toplevel):
    
    def __init__(self):
        Tkinter.Toplevel.__init__(self)
        self.title("A propos")
        self.config()
        toto = "##########################################################\n"
        toto = toto + "#\t\tInterface pour l'emulation de la carte STM32LDiscovery\t\t#\n"
        toto = toto + "#\t\t\t\tavec Qemu\t\t\t\t#\n"
        toto = toto + "#\t\tEID Timothee - MERCIER Michael - CLAVELIN Aurelien\t\t#\n"
        toto = toto + "#\t\t\t\t\t\t\t\t\t#\n"
        toto = toto + "#\t\thttp://air.imag.fr/mediawiki/index.php/Proj-2011-2012-qemu\t\t#\n"
        toto = toto + "##########################################################\n"
        self.text= Tkinter.Label(self,anchor='w',text=toto)
        self.resizable(False,False)
        self.text.pack()
        
    
if __name__ == "__main__":
    app = Ctrl(None)
    app.ui.title('STM32LDiscovery UI')
    app.ui.mainloop()
    