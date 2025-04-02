TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

# Найти MPI пути автоматически
MPI_CFLAGS = $$system(mpicc --showme:compile)
MPI_LFLAGS = $$system(mpicc --showme:link)

QMAKE_CXXFLAGS += $$MPI_CFLAGS
LIBS += $$MPI_LFLAGS

SOURCES += \
        main.cpp
