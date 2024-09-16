CC = gcc

SRCS1 = OpenGl.c
SRCS2 = Project2.c
OBJS1 = $(SRCS1:.c=.o)
OBJS2 = $(SRCS2:.c=.o)
LIBS = -lglut -lGLU -lGL -lm

                                                             
all: opengl project2

opengl: $(OBJS1)

	$(CC) -o $@ $^ $(LIBS)
project2: $(OBJS2)

	$(CC) -o $@ $^
run: opengl project2

	./opengl &
	./project2
runOpenGl: opengl

	./opengl &
		
runProject2: project2

	./project2
	

%.o: %.c
	$(CC) -c $<

clean:
	rm -f $(OBJS1) opengl
	rm -f $(OBJS2) project2
