VPATH = lib/coveo:lib/coveo/seq:lib/coveo/seq/detail:lib/coveo/linq:lib/coveo/linq/detail:tests:tests/coveo:tests/coveo/seq:tests/coveo/linq

all_tests.out: all_tests.o enumerable_tests.o linq_operator_tests.o linq_chaining_tests.o tests_main.o
	$(CXX) -o all_tests.out all_tests.o enumerable_tests.o linq_operator_tests.o linq_chaining_tests.o tests_main.o

all_tests.o: all_tests.cpp all_tests.h
	$(CXX) -c -std=c++11 tests/coveo/linq/all_tests.cpp -Ilib -Itests
enumerable_tests.o: enumerable_tests.cpp enumerable_tests.h enumerable_test_util.h enumerable.h enumerable_detail.h sequence_util.h
	$(CXX) -c -std=c++11 tests/coveo/seq/enumerable_tests.cpp -Ilib -Itests
linq_operator_tests.o: linq_operator_tests.cpp linq_operator_tests.h enumerable_test_util.h linq.h linq_detail.h linq_util.h exception.h
	$(CXX) -c -std=c++11 tests/coveo/linq/linq_operator_tests.cpp -Ilib -Itests
linq_chaining_tests.o: linq_chaining_tests.cpp linq_chaining_tests.h linq.h linq_detail.h linq_util.h exception.h
	$(CXX) -c -std=c++11 tests/coveo/linq/linq_chaining_tests.cpp -Ilib -Itests
tests_main.o: tests_main.cpp
	$(CXX) -c -std=c++11 tests/tests_main.cpp -Ilib -Itests

clean:
	rm all_tests.out all_tests.o enumerable_tests.o linq_operator_tests.o linq_chaining_tests.o tests_main.o

