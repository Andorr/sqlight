.PHONY: build
build:
	cd ./build/ && make

run:
	./build/main

test:
	./build/tests

rbtest:
	rspec tests_spec.rb

play: play.cpp
	g++ play.cpp -o play -std=c++20 -Wall
	./play