#!/bin/sh
if [ -z "$1" ]; then
	dir=env-s
else
	dir=$1
fi

all_passed=y
for i in `find $dir | grep hex`; do
	if ../build/rvsim -f $i -b 80000000 -s 10000 -c 10000 -i -d 2> /dev/null | grep 'gp    0x0000000000000001' > /dev/null; then
		printf '[\033[32mPASSED\033[0m] %s\n' $i
	else 
		printf '[\033[31mFAILED\033[0m] %s\n' $i
		all_passed=n
	fi
done

if test $all_passed == 'y'; then
	printf '\n*** \033[32mAll tests passed!\033[0m ***\n'
fi
