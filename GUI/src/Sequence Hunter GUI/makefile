DST="shunter-gui.jar"
CLASSPATH=".:src/"
all:
	javac -classpath  $(CLASSPATH) src/com/lnbio/Principal.java -d bin/
	jar cvfm $(DST) manifest -C bin/ .
	chmod +x $(DST)

clean:
	rm ./$(DST)
	rm -R bin/*
