cc = gcc
out = iothub-device
objects = main.o iothub.o iothubconfig.o log.o
iothub-device : ${objects}
	 ${cc} -o ${out} ${objects} -lpaho-mqtt3c -lpaho-mqtt3a

.PHONY : clean
clean :
	rm -rf ${out} ${objects}