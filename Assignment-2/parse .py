#Rajdeep Das
#18CS30034
import xml.etree.ElementTree as ET
import sys
import numpy as np 
from geolite2 import geolite2   #installl geolite2 by this command: pip install maxminddb-geolite2  
import csv

#the following code is used to 
#get the ip adresses of all the users those
#who have accessed LearnBasics through FreeBasics App
tree=ET.parse(sys.argv[1])    
root=tree.getroot()
ip_list =[]    #all the ip adress will be store in this list
for x in root:
    for y in x:
        s1=(y.get('name'))
        if s1=="http":
            
            for z in y:
                s2=z.get('name')
                if s2=="http.x_forwarded_for":
                    s4=z.get('show')    #s4 straing contains the ip adress
                s3=z.get('showname') 
                if s3=="Via: Internet.org\\r\\n":  
                    ip_list.append(s4)

#the following code is to get the  
#unique ip values 
ip_list2=[]   #a new list has been created to store unique ip adresses
x = np.array(ip_list)  
ip_list2=np.unique(x)  #here we are using numpy unique function to store the unique ip adresses into ip_list2


#the following code is to get country name 
#and number of users per country by using the geolite2 package
country_cnt = {}   #dictionary to count the number of users per country
for ip in ip_list2:
    reader = geolite2.reader()
    ab=(reader.get(ip))
    country = (ab['country']['names']['en'])
    if country not in country_cnt:
        country_cnt[country] = 0
    country_cnt[country] +=1
geolite2.close()

#the following code is to save contents of 
#country_cnt into the data.csv file
with open('data.csv', 'w') as f:
    for key in country_cnt:
        f.write("%s,%s\n"%(key,country_cnt[key]))
