all: measure_accuracy simple_counter approximate_counter hand_over_hand_list

clean:
	rm -f measure_accuracy simple_counter approximate_counter

measure_accuracy: measure_accuracy.c
	gcc -o measure_accuracy measure_accuracy.c -Wall

simple_counter: simple_counter.c
	gcc -o simple_counter.out simple_counter.c counter.h wrappers.h -pthread -O0 -Wall

hand_over_hand_list: hand_over_hand_list.c
	gcc -o hand_over_hand_list.out hand_over_hand_list.c wrappers.h -pthread -O0 -Wall

approximate_counter: approximate_counter.c
	gcc -o approximate_counter.out approximate_counter.c approximate_counter.h wrappers.h -pthread -O0 -Wall