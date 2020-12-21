# Suivi de voie (à partir du marquage au sol) pour véhicule autonome
# Lane tracking (from ground marking) for autonomous vehicle

# Sylvain Grimal formation SI "science de l'ingenieur" lycée pierre gilles de gennes Digne


import sensor, image, time, network, usocket, sys, math

wifi= False
if wifi:
    SSID ='Racemini'     # Network SSID
    KEY  ='1234'     # Network key
    HOST =''     # Use first available interface
    PORT = 80  # Arbitrary non-privileged port

from pyb import UART
uart = UART(1, 115200)


nb= False # pour essai image niveau de gris
canny= False #traitement canny supplementaire uniquement en NB
edge= False
resolution=2 # 1 pour QQVGA (160x120 env 40-50 FPS sur H7) 2 pour QVGA (320x240 env 25-30 FPS) VGA non supporté (pb memoire)
enable_lens_corr = True # compense la déformation dans les angles mais reduit le champ visuel si True

taille=2 # taille de détection
filtre=.5 # 0 pas de filtre 1 pas de signal


if (resolution==1):
    xmax=160
    ymax=120
if (resolution==2):
    xmax=320
    ymax=240


sensor.reset()
if nb: sensor.set_pixformat(sensor.GRAYSCALE)
else : sensor.set_pixformat(sensor.RGB565)

if (resolution==1): sensor.set_framesize(sensor.QQVGA)
if (resolution==2): sensor.set_framesize(sensor.QVGA)


sensor.skip_frames(time = 2000)
#sensor.set_contrast(0)
#sensor.set_brightness(-3)
#sensor.set_saturation(1)
#sensor.set_gainceiling(16)
#sensor.set_auto_gain(True)
sensor.set_auto_exposure(True)
clock = time.clock()

#mode "Office" omnivision
#sensor.__write_reg(0x13, 0xff); #fd sans AWB
#sensor.__write_reg(0x01, 0x84);
#sensor.__write_reg(0x02, 0x4c);
#sensor.__write_reg(0x0e, 0x65);
#sensor.__write_reg(0x2d, 0x00);
#sensor.__write_reg(0x22, 0x98); #50Hz banding filter h98
#sensor.__write_reg(0x23, 0x03); #4 ou 8 step for 50hz
#sensor.__write_reg(0x11, 0x01);
#sensor.__write_reg(0x0e, 0x65);


# Init wlan module and connect to network
if wifi:
    print("Trying to connect... (may take a while)...")
    wlan = network.WINC()
    wlan.connect(SSID, key=KEY, security=wlan.WPA_PSK)

    # We should have a valid IP now via DHCP
    print(wlan.ifconfig())

    # Create server socket
    s = usocket.socket(usocket.AF_INET, usocket.SOCK_STREAM)

    # Bind and listen
    s.bind([HOST, PORT])
    s.listen(5)

    # Set server socket to non-blocking
    s.settimeout(0)


###################
# Zones d'interet #
###################

ratio=3.2/4 # partie haute zone d'anticipation, partie basse zone de Direction
ZONEA = (0,0,int(xmax),int(ratio*ymax)) # pas d'anticipation (freinage) non utilisé ici
#ZONED = (0,int(ratio*ymax),int(xmax),int((1-ratio)*ymax)) # zones ligne blanche
ZONED = (int(xmax/2),int(ratio*ymax),int(xmax/2),int((1-ratio)*ymax)) # zones ligne blanche gauche
ZONEG = (0,int(ratio*ymax),int(xmax/2),int((1-ratio)*ymax)) # zones ligne blanche droit



######################################################################################
# Objet point (de la route) permettant le passage des coordonnées caméra et véhicule #
######################################################################################
# modele sténopé (pinhole)

INCLI_CAM = 20*math.pi/180
ECAM=3.5
HCAM=29
ALPHA=math.tan(INCLI_CAM)
BETA=HCAM/math.cos(INCLI_CAM)
FOCAL=xmax*2/4 #focale et taille capteur

class Point():

    # définition d'un point dans le referentiel camera
    def setCam( self, x1, y1):
        zR = BETA/(ALPHA-y1/FOCAL)
        self.z = zR
        self.x = zR*x1/FOCAL
        self.y = zR*y1/FOCAL


    # recuperation des coordonnés dans le réferentiel véhicule
    def getVeh( self):
        x=self.x
        y=HCAM+self.y*math.cos(INCLI_CAM)-self.z*math.sin(INCLI_CAM) # pour debugage normalement=0 car point sur la route
        z=ECAM+self.y*math.sin(INCLI_CAM)+self.z*math.cos(INCLI_CAM)
        return (x,y,z)
    # constructeur
    def __init__(self):
        self.x=0
        self.y=0
        self.z=0

#####################
# Boucle principale #
#####################

max_degree = 55
THRESHOLD = (25, 255) # Grayscale threshold for dark things...

seuilD = 1000.0 # seuil inititial pour la detection des lignes
seuilG = 1000.0

nbLignes = 2

pd1c = Point()
pd2c = Point()
pg1c = Point()
pg2c = Point()



xd1=xg1=xd2=xg2=xmax/2
yd1=yg1=ymax*ratio
yd2=yg2=ymax

if wifi:
    print ('Waiting for connections..')
    client, addr = s.accept()
    client.settimeout(2.0)
    print ('Connected to ' + addr[0] + ':' + str(addr[1]))

    data = client.recv(1024)
    # Should parse client request here

    client.send("HTTP/1.1 200 OK\r\n" \
            "Server: OpenMV\r\n" \
            "Content-Type: multipart/x-mixed-replace;boundary=openmv\r\n" \
            "Cache-Control: no-cache\r\n" \
            "Pragma: no-cache\r\n\r\n")


#dif=1
while(True):
    clock.tick() # Track elapsed milliseconds between snapshots().

    ####################
    # Prise de l'image #
    ####################

    if nb :
        img = sensor.snapshot()
        if canny: img.find_edges(image.EDGE_CANNY, threshold=(50, 80))
        if edge: img.find_edges(image.EDGE_SIMPLE, threshold=(25, 80))
    else: img = sensor.snapshot() #.histeq(adaptive=True, clip_limit=1)

    if enable_lens_corr: img.lens_corr(2.2) # for 2.8mm lens...

    # clip_limit à ajuster afin de ne pas trop contraster une image normalement peu contrastée (ligne blanche sur route grise)



    ############################
    # traitement zone conduite #
    ############################


    ##### Zone Gauche

    ensembleLignes = img.find_lines(ZONEG, x_stride = taille, y_stride = taille, threshold = int(seuilG), theta_margin = 1, rho_margin = 2)

    ng=0
    somGX1=0
    somGY1=0
    somGX2=0
    somGY2=0


    for l in ensembleLignes:
        if ((-max_degree+180) <= l.theta()) or (l.theta() <= max_degree):
            #img.draw_line(l.line(), color = (255, 0, 0),thickness = resolution)

            ng+=1
            if (l.y2()>l.y1()) :   # si 1° point en bas ?

                somGX1+=l.x1()
                somGY1+=l.y1()
                somGX2+=l.x2()
                somGY2+=l.y2()

            else :                  # on inverse point 1 et 2
                somGX1+=l.x2()
                somGY1+=l.y2()
                somGX2+=l.x1()
                somGY2+=l.y1()

    # Ajustement des seuils un changement brutal des conditions d'eclairage peut entrainer une perte de la reconnaisance
    if (ng>nbLignes):
        seuilG=seuilG*1.02
    if (ng<nbLignes) and (seuilG>1):
        seuilG=seuilG*0.98

    # valeurs moyennes
    if ng!=0 :
        xg1=somGX1/ng*(1-filtre)+filtre*xg1
        yg1=somGY1/ng*(1-filtre)+filtre*yg1
        xg2=somGX2/ng*(1-filtre)+filtre*xg2
        yg2=somGY2/ng*(1-filtre)+filtre*yg2

    img.draw_line(int(xg1), int(yg1), int(xg2), int(yg2), color = (255, 0, 0),thickness = resolution*3)


    ##### Zone Droite

    ensembleLignes = img.find_lines(ZONED, x_stride = taille, y_stride = taille, threshold = int(seuilD), theta_margin = 1, rho_margin = 2)

    nd=0
    somDX1=0
    somDY1=0
    somDX2=0
    somDY2=0


    for l in ensembleLignes:
        if ((-max_degree+180) <= l.theta()) or (l.theta() <= max_degree):
            #*img.draw_line(l.line(), color = (255, 0, 0),thickness = resolution)

            nd+=1
            if (l.y2()>l.y1()) :   # si 1° point en bas ?

                somDX1+=l.x1()
                somDY1+=l.y1()
                somDX2+=l.x2()
                somDY2+=l.y2()

            else :                  # on inverse point 1 et 2
                somDX1+=l.x2()
                somDY1+=l.y2()
                somDX2+=l.x1()
                somDY2+=l.y1()


    # Ajustement des seuils un changement brutal des conditions d'eclairage peut entrainer une perte de la reconnaisance
    if (nd>nbLignes):
        seuilD=seuilD*1.02
    if (nd<nbLignes) and (seuilD>1):
        seuilD=seuilD*0.98

    # valeurs moyennes
    if nd!=0 :
        xd1=somDX1/nd*(1-filtre)+filtre*xd1
        yd1=somDY1/nd*(1-filtre)+filtre*yd1
        xd2=somDX2/nd*(1-filtre)+filtre*xd2
        yd2=somDY2/nd*(1-filtre)+filtre*yd2


    img.draw_line(int(xd1), int(yd1), int(xd2), int(yd2), color = (255, 0, 0),thickness = resolution*3)



    # Calcul de la médiane ligne droite-ligne gauche pour affichage

    xm1=((xd1+xg1)/2)
    xm2=((xd2+xg2)/2)
    ym1=((yd1+yg1)/2)
    ym2=((yd2+yg2)/2)

    img.draw_arrow(int(xm2),int(ym2),int(xm1),int(ym1),color = (0, 255, 0),thickness = resolution*2)
    img.draw_circle(int(xmax/2),ymax,int(xmax/20),color = (0, 255, 0))

    # calcul des points corespondants sur la route

    pd1c.setCam(xd1-xmax/2,-yd1+ymax/2)
    pd2c.setCam(xd2-xmax/2,-yd2+ymax/2)

    print ("         Hough x1=%.1f"%(xd1-xmax/2)," y1=%.1f"%(-yd1+ymax/2))


    xd1v,yd1v,zd1v=pd1c.getVeh()
    xd2v,yd2v,zd2v=pd2c.getVeh()

    pg1c.setCam(xg1-xmax/2,-yg1+ymax/2)
    pg2c.setCam(xg2-xmax/2,-yg2+ymax/2)
    xg1v,yg1v,zg1v=pg1c.getVeh()
    xg2v,yg2v,zg2v=pg2c.getVeh()

    uart.write("xd1%.1f\r" %(xd1v))
    uart.write("zd1%.1f\r" %(zd1v))
    uart.write("xd2%.1f\r" %(xd2v))
    uart.write("zd2%.1f\r" %(zd2v))
    uart.write("xg1%.1f\r" %(xg1v))
    uart.write("zg1%.1f\r" %(zg1v))
    uart.write("xg2%.1f\r" %(xg2v))
    uart.write("zg2%.1f\r" %(zg2v))


    if wifi:
        dif-=1 # 1 fois sur 2 pour moins ralentir
        if dif==0 :
            cframe = img.compressed(quality=15)
            header = "\r\n--openmv\r\n" \
                     "Content-Type: image/jpeg\r\n"\
                     "Content-Length:"+str(cframe.size())+"\r\n\r\n"
            client.send(header)
            client.send(cframe)
            dif=1



    print ("FPS: %i"%clock.fps()," Droite x1=%.1f"%xd1v," y1=%.1f"%yd1v," z1=%.1f"%zd1v, " x2=%.1f"%xd2v," y2=%.1f"%yd2v," zd2=%.1f"%zd2v)
    print ("         Droite x1=%.1f"%xg1v," y1=%.1f"%yg1v," z1=%.1f"%zg1v, " x2=%.1f"%xg2v," y2=%.1f"%yg2v," zd2=%.1f"%zg2v)
    #print ("FPS: %i"%clock.fps()," Seuil D:",seuilD, " Nb gauche: ",ng, "Nb droit: ",nd)



#img = sensor.snapshot().binary([THRESHOLD])
#img = sensor.snapshot()
#if enable_lens_corr: img.lens_corr(2.2) # for 2.8mm lens...
#img.find_edges(image.EDGE_SIMPLE, threshold=(150, 255))
#img.mode(1, threshold=True, offset=-10, invert=True)
#img.negate()
#img.laplacian(2, sharpen = False, mul = 0.05, add = 80, threshold = False, offset = 0, invert = False )
#img.remove_shadows()
#img.linpolar()

