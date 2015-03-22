include ../lib/makefile.head
filename = toOwlName

OBJL = $(APPOBJ)/$(filename).o $(OBJ)

$(EXEDIR)/$(filename).exe : $(APPOBJ)/$(filename).o $(OBJ)
	$(CC) $(CFLAGS) -lxerces-c2_6_0 -Lc:/bin $(OBJL) -I$(MYSRCLIB) -I$(GENCPPLIB) -o $@

$(APPOBJ)/$(filename).o : $(filename).cpp \
		$(OBJDR)/fullSyn.o $(OBJDR)/OntyxKb.o
	$(CC) $(CFLAGS) -I$(MYSRCLIB) -I$(GENCPPLIB) -c $< -o $@


include ../lib/makefile.incl