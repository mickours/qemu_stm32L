
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
        ## champ IP ##
        self.labelIP = Tkinter.Label(self,anchor='w',text="IP du serveur Qemu :")
        self.labelIP.grid(column=0,row=0,sticky='EW')
        
        self.entryVariableIP = Tkinter.StringVar()
        self.entryVariableIP.set("Not supported yet")
        self.entryIP = Tkinter.Entry(self,textvariable=self.entryVariableIP)
        self.entryIP.grid(column=0,row=1,sticky='EW')
        '''
        
        ## champ Port LED verte ##
        self.labelPortVerte = Tkinter.Label(self,anchor='w',text="Port LED verte :")
        self.labelPortVerte.grid(column=0,row=2,sticky='EW') 
        
        self.entryVariablePortVerte = Tkinter.StringVar()
        self.entryVariablePortVerte.set(u"4242")
        self.entryPortVerte = Tkinter.Entry(self,textvariable=self.entryVariablePortVerte)
        self.entryPortVerte.grid(column=0,row=3,sticky='EW')
        
        ## champ Port LED Bleue ##
        self.labelPortBleue = Tkinter.Label(self,anchor='w',text="Port LED bleue :")
        self.labelPortBleue.grid(column=0,row=4,sticky='EW') 
        
        self.entryVariablePortBleue = Tkinter.StringVar()
        self.entryVariablePortBleue.set(u"4243")
        self.entryPortBleue = Tkinter.Entry(self,textvariable=self.entryVariablePortBleue)
        self.entryPortBleue.grid(column=0,row=5,sticky='EW')
        
        ## champ Port Bouton ##
        self.labelPortBouton = Tkinter.Label(self,anchor='w',text="Port Bouton :")
        self.labelPortBouton.grid(column=0,row=6,sticky='EW') 
        
        self.entryVariablePortBouton = Tkinter.StringVar()
        self.entryVariablePortBouton.set(u"4244")
        self.entryPortBouton = Tkinter.Entry(self,textvariable=self.entryVariablePortBouton)
        self.entryPortBouton.grid(column=0,row=7,sticky='EW')
        
        
        button = Tkinter.Button(self,text="Lancer",command=self.OnButtonLancerClick)
        button.grid(column=0,row=8,sticky="EW")
        
        self.labelLog = Tkinter.Label(self,anchor='w',text="Log : ")
        self.labelLog.grid(column=0,row=9,sticky='EW')
        
        self.log = Tkinter.Text(width=20,height=20)
        self.log.grid(column=0,row=10,sticky='EWNS',rowspan=50)
        self.log.insert(Tkinter.END,"")
        
        button2 = Tkinter.Button(self,text="Vider",command=self.Clear)
        button2.grid(column=0,row=60,sticky="EW")
        
        scrollbar = Tkinter.Scrollbar(self.log,width=10)
        scrollbar.pack(side='right', fill='y')
        
        scrollbar.config(command=self.log.yview)
        self.log.config(yscrollcommand=scrollbar.set)
        
        
        # Images #
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
        self.canvas.tag_bind("reset", "<B1-ButtonRelease>",self.Reset)
        self.canvas.tag_bind("user", "<Button-1>",self.UserPressed)
        self.canvas.tag_bind("user", "<B1-ButtonRelease>",self.UserReleased)
        
        
        
        self.ledGauche = self.canvas.create_image(35,400, anchor = "nw", image=self.ledBlanche)
        self.ledDroite = self.canvas.create_image(190,400, anchor = "nw", image=self.ledBlanche)
        
        #self.grid_columnconfigure(1,weight=1)
        #self.grid_columnconfigure(0,weight=1) INUTILE
        #self.grid_rowconfigure(0,weight=1) #Gestion verticale de l'agrandissement
        self.resizable(False,False) #Pas redimentionable !
        
        
        #self.geometry(self.geometry())
        self.update()
        
    def OnButtonLancerClick(self):
        app.event.set()
        
    def UserPressed(self,none):
        self.log.insert(Tkinter.END,"User button pressed\n")
        app.user_pressed()
        self.log.see(Tkinter.END)
        
    def UserReleased(self,none):
        self.log.insert(Tkinter.END,"User button released\n")
        app.user_released()
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
        self.serversocketVerte = None
        self.serversocketBleue = None
        self.serversocketBouton = None
        self.connVerte, self.addrVerte = None,None
        self.connBleue, self.addrBleue = None,None
        self.connBouton, self.addrBouton = None,None
        self.ui = UI(parent)
        self.on = False
        self.event = threading.Event()
        self.thread_verte=threading.Thread(target=self.fct_thread,
                                           args=("verte",self.ui.entryPortVerte,self.serversocketVerte,self.connVerte,self.addrVerte))
        self.thread_verte.setDaemon(True)
        self.thread_bleue=threading.Thread(target=self.fct_thread,
                                           args=("bleue",self.ui.entryPortBleue,self.serversocketBleue,self.connBleue,self.addrBleue))
        self.thread_bleue.setDaemon(True)
        self.thread_bouton=threading.Thread(target=self.fct_thread,
                                           args=("bouton",self.ui.entryPortBouton,self.serversocketBouton,self.connBouton,self.addrBouton))
        self.thread_bouton.setDaemon(True)
        self.thread_verte.start()
        self.thread_bleue.start()
        self.thread_bouton.start()
        
    def close_conn(self,conn):
            conn.close()
            self.ui.log.insert(Tkinter.END,"Deconnexion \n")
            
    def user_pressed(self):
        if self.connBouton != None :
            self.connBouton.send(struct.pack('B',1))
        else :
            self.ui.log.insert(Tkinter.END,"User button not connected\n")
            
    def user_released(self):
        if self.connBouton != None :
            self.connBouton.send(struct.pack('B',0))
        else :
            self.ui.log.insert(Tkinter.END,"User button not connected")
        
    def traiterrecv(self,conn):
        while 1:
            data = conn.recv(1)
            if not data: break
            if conn == self.connVerte :
                if ord(data) == 0:
                    self.ui.canvas.itemconfigure(self.ui.ledDroite, image=self.ui.ledBlanche)
                    self.ui.log.insert(Tkinter.END,"Data " + str(ord(data)) + " received on" + " connection verte\n")
                elif ord(data) == 1:
                    self.ui.canvas.itemconfigure(self.ui.ledDroite, image=self.ui.ledVerte)
                    self.ui.log.insert(Tkinter.END,"Data " + str(ord(data)) + " received on" + " connection verte\n")
                else:
                    self.ui.log.insert(Tkinter.END,"Data " + str(ord(data)) + " received on" + " connection verte => unknown\n")
            elif conn == self.connBleue :
                if ord(data) == 0:
                    self.ui.canvas.itemconfigure(self.ui.ledGauche, image=self.ui.ledBlanche)
                    self.ui.log.insert(Tkinter.END,"Data " + str(ord(data)) + " received on" + " connection bleue\n")
                elif ord(data) == 1:
                    self.ui.canvas.itemconfigure(self.ui.ledGauche, image=self.ui.ledBleue)
                    self.ui.log.insert(Tkinter.END,"Data " + str(ord(data)) + " received on" + " connection bleue\n")
                else:
                    self.ui.log.insert(Tkinter.END,"Data " + str(ord(data)) + " received on" + " connection bleue => unknown\n")
            elif conn == self.connBouton:
                self.ui.log.insert(Tkinter.END,"Data " + str(ord(data)) +" received on connection bouton => error \n")
            self.ui.log.see(Tkinter.END)
            self.ui.update()
    
    def launch_server(self,name,entryPort,serversocket,conn,addr):
        serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        serversocket.bind((socket.gethostname(), int(entryPort.get())))
        serversocket.listen(1) 
        conn, addr = serversocket.accept()
        if name == "verte" :
            self.ui.log.insert(Tkinter.END,"En attente de connexion sur le port " + entryPort.get()+ " pour la LED verte\n")
            self.ui.update()
            self.serversocketVerte = serversocket
            self.connVerte = conn
        if name == "bleue" :
            self.ui.log.insert(Tkinter.END,"En attente de connexion sur le port " + entryPort.get()+ " pour la LED bleue\n")
            self.ui.update()
            self.serversocketBleue = serversocket
            self.connBleue = conn
        if name == "bouton" :
            self.ui.log.insert(Tkinter.END,"En attente de connexion sur le port " + entryPort.get()+ " pour le bouton\n")
            self.ui.update()
            self.serversocketBouton = serversocket
            self.connBouton = conn
        self.ui.log.insert(Tkinter.END,"Connecte \n")
        self.ui.update()
        self.traiterrecv(conn)
        self.close_conn(conn)
        
    def tester(self):
        self.ui.canvas.itemconfigure(self.ui.ledGauche, image=self.ui.ledBleue)
        self.ui.canvas.itemconfigure(self.ui.ledDroite, image=self.ui.ledVerte)
        
    def fct_thread(self,name,entryPort,serversocket,conn,addr):
        while 1:
            while not self.event.isSet():
                self.event.wait()
            self.launch_server(name,entryPort,serversocket,conn,addr)
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
        

#####################
#Main               #
#####################   
if __name__ == "__main__":
    app = Ctrl(None)
    app.ui.title('STM32LDiscovery UI')
    app.ui.mainloop()
    