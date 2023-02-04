ifndef CONTIKI
  ${error CONTIKI not defined! Do not forget to source /upb/groups/fg-ccs/public/share/nes/2018w/env.sh}
endif

ifndef TARGET
  TARGET=sky
endif

PROJECT_SOURCEFILES += aodv_message.c aodv_table.c

all: aodv_node.sky

upload: aodv_node.upload

sim: aodv_node.csc aodv_node.c
	java -jar $(CONTIKI)/tools/cooja/dist/cooja.jar -quickstart=aodv_node.csc -contiki=$(CONTIKI)

simulation:
	java -jar $(CONTIKI)/tools/cooja/dist/cooja.jar -contiki=$(CONTIKI)

testfiles := aodv_utils_test.c ${PWD}/aodv_utils/aodv_message.c ${PWD}/aodv_utils/aodv_table.c  
test-utils: $(testfiles)
	gcc $(testfiles) -o test

CONTIKI_WITH_IPV4 = 1
CONTIKI_WITH_RIME = 1
include $(CONTIKI)/Makefile.include
