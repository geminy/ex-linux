TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
#    pipe/mainpipe.c \
#    pipe/fduplex.c \
#    pipe/mainpopen.c \
#    pipe/mainfifo.c \
#    pipe/server_main.c \
#    pipe/client_main.c \
#    pipe/pipeconf.c \
#    fifocliserv/mainserver.c \
#    fifocliserv/mainclient.c \
#    pipemesg/mesg_send.c \
#    pipemesg/mesg_recv.c \
#    pipemesg/client.c \
#    pipemesg/server.c \
#    pipemesg/mesg_main.c \
#    pxmsg/mqcreate1.c \
#    pxmsg/mqunlink.c \
#    pxmsg/mqgetattr.c \
#    pxmsg/mqcreate.c \
#    pxmsg/mqsend.c \
#    pxmsg/mqreceive.c \
#    pxmsg/mqsysconf.c \
#    pxmsg/mqnotifysig1.c \
#    pxmsg/mqnotifysig2.c \
#    pxmsg/mqnotifysig3.c \
#    pxmsg/mqnotifysig4.c \
#    pxmsg/mqnotifysig5.c \
#    pxmsg/mqnotifysig6.c \
#    rtsignals/test1.c \
#    mutex/producer_consumer.c \
#    mutex/producer_consumer2.c \
#    mutex/producer_consumer3.c \
#    mutex/producer_consumer4.c \
#    rwlock/cleanup.c \
#    rwlock/rwlock.c \
#    fcntl/nolock.c \
#    fcntl/fcntllock.c \
#    pxsem/semcreate.c \
#    pxsem/semunlink.c \
#    pxsem/semgetvalue.c \
#    pxsem/semwait.c \
#    pxsem/sempost.c \
#    pxsem/prodcons.c \
#    shm/increment.c \
#    shm/increment2.c \
#    shm/increment3.c \
#    shm/increment_map_anon.c \
#    shm/increment_dev_zero.c \
#    shm/mmap_test.c \
#    shm/mmap_test2.c \
#    pxshm/shmcreate.c \
#    pxshm/shmunlink.c \
#    pxshm/shmwrite.c \
#    pxshm/shmread.c \
#    pxshm/server.c \
#    pxshm/client.c \
    log/log.c \
    log/test_log.c

HEADERS += \
#    pipemesg/pipemesg.h \
    log/log.h

QMAKE_LIBS += -lrt -lpthread
