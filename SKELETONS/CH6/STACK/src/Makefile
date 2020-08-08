CXXFLAGS = -g -O0 -I..
LDFLAGS  = -L../local/lib -lStack
../local/bin/client:client.cpp ../local/lib/libStack.a
	mkdir -p ../local/bin
	$(CXX) $(CXXFLAGS) $< $(LDFLAGS) -o $@ 

../local/lib/libStack.a:Stack.o
	mkdir -p ../local/lib
	ar rc $@ $<
	ranlib $@

clean:
	rm -f ../local/bin/client Stack.o ../local/lib/libStack.a *~
