TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c

#SOURCES += iocase.c
#SOURCES += mmapex.c
#SOURCES += pipeex.c
#SOURCES += pollex.c
#SOURCES += popenex.c
#SOURCES += readfifo.c
SOURCES += readnwriten.c
#SOURCES += readvex.c
#SOURCES += selectex.c
#SOURCES += writefifo.c
#SOURCES += writevex.c

include(deployment.pri)
qtcAddDeployment()

