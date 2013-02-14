DST="database-manager.jar"

all:
	javac -classpath src/ src/main/Main.java -d bin/
	jar cvfm $(DST) manifest -C bin/ .
	chmod +x $(DST)

clean:
	rm ./$(DST)
	rm -R bin/*
