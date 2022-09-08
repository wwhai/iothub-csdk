cc = gcc
out = iothub-device
objects = main.o iothub.o iothubconfig.o log.o iothubschema.o cJSON.o utils.o
objects-test = atest.o iothub.o iothubconfig.o log.o iothubschema.o cJSON.o utils.o

.PHONY : all
all : ${objects}
	bash gen-sdk.sh
	${cc} -o ${out} ${objects} -lpaho-mqtt3c -lpaho-mqtt3a

.PHONY : test
test : ${objects-test}
	bash gen-sdk.sh
	${cc} -o ${out}-test ${objects-test} -lpaho-mqtt3c -lpaho-mqtt3a

.PHONY : clean
clean :
	rm -rf iothub-device* ${objects}