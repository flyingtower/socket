.PHONY: all clean

allobj = main.o
dst = testServer

all: main

main: $(allobj)
	g++ $(allobj) -o $(dst)

$(allobj): %.o:%.cpp
	g++ -c -g $< -o $@

clean:
	rm $(allobj) $(dst)
