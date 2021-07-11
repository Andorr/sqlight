.PHONY: build
build:
	cd ./build/ && make

run:
	./build/main .data/sqlight.db

test:
	./build/tests

rbtest:
	rspec tests_spec.rb

cmake:
	cmake -S . -B build

play: play.cpp
	g++ play.cpp -o play -std=c++20 -Wall
	./play