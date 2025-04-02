TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

# Найти MPI пути автоматически
MPI_CFLAGS = $$system(mpicc --showme:compile)
MPI_LFLAGS = $$system(mpicc --showme:link)

QMAKE_CXXFLAGS += $$MPI_CFLAGS
LIBS += $$MPI_LFLAGS

INCLUDEPATH += /usr/include/openmpi
LIBS += -lmpi_cxx -lmpi

SOURCES += \
        main.cpp

DISTFILES += \
    README.md
