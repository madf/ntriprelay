INCS = 
DEFS = 
CFLAGS += -ggdb3 -W -Wall -Wextra $(INCS) $(DEFS)
CXXFLAGS = $(CFLAGS) -std=c++0x
LIBS = -lpthread -lboost_program_options -lboost_system
PROG = ncli

SOURCES = main.cpp \
	  client.cpp \
	  settings.cpp \
	  logger.cpp \
	  log_writer.cpp \
	  base64.cpp \
	  authenticator.cpp

.PHONY: all clean version.h

all: $(PROG)

$(PROG): version.h $(addsuffix .o,$(basename $(SOURCES)))
	$(CXX) $(LDFLAGS) $^ $(LIBS) -o $@

clean:
	rm -f $(addsuffix .o,$(basename $(SOURCES))) $(addsuffix .d,$(SOURCES)) $(PROG)

version.h:
	@sed "s/@REVNO@/"`git describe`"/" version.h.in > version.h

ifneq ($(MAKECMDGOALS),distclean)
ifneq ($(MAKECMDGOALS),clean)
-include $(addsuffix .d,$(SOURCES))
endif
endif

%.cpp.d: %.cpp
	@$(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\).o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

%.c.d: %.c
	@$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\).o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
