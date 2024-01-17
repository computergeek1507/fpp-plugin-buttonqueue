include /opt/fpp/src/makefiles/common/setup.mk

all: libfpp-plugin-buttonqueue.so
debug: all

OBJECTS_fpp_buttonqueue_so += src/buttonqueuePlugin.o
LIBS_fpp_buttonqueue_so += -L/opt/fpp/src -lfpp
CXXFLAGS_src/buttonqueuePlugin.o += -I/opt/fpp/src

%.o: %.cpp Makefile
	$(CCACHE) $(CC) $(CFLAGS) $(CXXFLAGS) $(CXXFLAGS_$@) -c $< -o $@

libfpp-plugin-buttonqueue.so: $(OBJECTS_fpp_buttonqueue_so) /opt/fpp/src/libfpp.so
	$(CCACHE) $(CC) -shared $(CFLAGS_$@) $(OBJECTS_fpp_buttonqueue_so) $(LIBS_fpp_buttonqueue_so) $(LDFLAGS) -o $@

clean:
	rm -f libfpp-plugin-buttonqueue.so $(OBJECTS_fpp_buttonqueue_so)
