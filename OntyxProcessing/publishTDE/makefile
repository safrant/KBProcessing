include ../lib/makefile.head
filename = publishTDE

OBJL = $(APPOBJ)/$(filename).o $(OBJ)

$(EXEDIR)/$(filename).exe : $(APPOBJ)/$(filename).o $(OBJ)
	$(CC) $(CFLAGS) -static -LC:/MinGW/msys/1.0/local/lib -lxerces-c $(OBJL) -I$(MYSRCLIB) -I$(GENCPPLIB) -o $@

$(APPOBJ)/$(filename).o : $(filename).cpp \
		$(OBJDR)/fullSyn.o $(OBJDR)/OntyxKb.o
	$(CC) $(CFLAGS) -I$(MYSRCLIB) -I$(GENCPPLIB) -c $< -o $@


include ../lib/makefile.incl
