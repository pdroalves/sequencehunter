DST="SHunter.jar"

all:
	javac -classpath src/ src/Principal.java -d bin/
	jar cvfm $(DST) manifest -C bin/ .
	chmod +x $(DST)
