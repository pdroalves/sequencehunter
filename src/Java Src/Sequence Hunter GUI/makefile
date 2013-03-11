DST="shunter-gui.jar"

all:
	javac -classpath  sqlite-jdbc-3.7.2.jar:src/ src/Principal.java -d bin/
	jar cvfm $(DST) manifest -C bin/ .
	chmod +x $(DST)

clean:
	rm ./$(DST)
	rm -R bin/*
