DST="shunter-gui.jar"

all:
	javac -classpath src/ src/Principal.java -d bin/
	jar cvfm $(DST) manifest -C bin/ .
	chmod +x $(DST)

clean:
	rm ./$(DST)
	rm -R bin/*
