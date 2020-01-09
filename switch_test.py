import socket
import string
import time
import GEMconf_classes_2018v2 #acr 2018-02-19 import GEMconf_classes_2018
import struct
import binascii
import array
import classes_test_functions
import sys
import threading ## acr 2018-02-21
import Queue     ## acr 2018-02-21
import os        ## acr 2018-02-21
import datetime



def add_input(input_queue):
    while True:
        #input_queue.put(sys.stdin.read(1))
        input_queue.put(sys.stdin.readline())

def Menu_and_prompt():
    input_queue = Queue.Queue()
    input_thread = threading.Thread(target=add_input, args=(input_queue,))
    input_thread.daemon = True
    input_thread.start()

    last_update = time.time()
    DONE = False
    print'\nEnter your command: '
    while not DONE:
        if ((time.time()-last_update)> 10) :
            last_update = time.time()
            print('\ntic')
            print'\nEnter your command: '
        else:
            ##print'\rEnter your command: '
            if not input_queue.empty():
                input_array = (input_queue.get()).split()
                if (len(input_array) > 0):
                    print '\n input_array: %s; input_array.length = %d' %(input_array, len(input_array))
                    if ( (input_array[0] == 'quit') or (input_array[0] == 'q') or (input_array[0] == 'Quit') or (input_array[0] == 'Q') ):
                        print '\nBye'
                        DONE = True
                    elif ( (input_array[0] == 'PEW') or (input_array[0] == 'P') ):
                        if (len(input_array) == 2):
                            print ('\nPEW %d' %(int(input_array[1],0)))
                        else:
                            print '\n bad command'
                            time.sleep(2)
                        print'\nEnter your command: '
                    else:
                        print '\n bad command\nEnter your command: '
                else:
                    input_queue.empty()
                    del input_array
##            else:
##                print'\nEnter your command: '

Menu_and_prompt()
exit()




##
##DONE = False
##while not DONE:
##    userinput = int(input('enter a string'))
##    if ((userinput == 3) or (userinput == 33)):
##        print '\ntre'
##    elif (userinput == 4):
##        print '\nquattro'
##    elif (userinput == 5):
##        print '\ncinque'
##        DONE = True
##    else:
##        print '\nnessuno di questi'
