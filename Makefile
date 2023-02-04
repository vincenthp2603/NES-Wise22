testfiles := aodv_utils_test.c ${PWD}/aodv_utils/aodv_message.c ${PWD}/aodv_utils/aodv_table.c  
test-utils: $(testfiles)
	gcc $(testfiles) -o test