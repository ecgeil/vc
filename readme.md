# vc

###Synopsis
vc [-bfirtu] [-w maxchars] [input [output]]

###Description
`vc` counts the number of occurrences of each distinct line in <input>,
and writes the count, followed by the original line, to <output>.

Internally, `vc` uses a hash table to do this efficiently. 
In many cases, `vc` is a faster alternative to `sort | uniq -c`.

###Options
	-b 
		Ignore blank lines

	-f 
		Print the fraction of the total count for each item

	-i 
		Case insensitive 
		(all lines are converted to lowercase)

	-r 
		Reverse the sort order

	-t 
		Sort output by the text (lexicographically), 
		rather than by count.
		This will match the typical output of 
		`sort | uniq -c.`

	-u 
		Don't print counts (unique lines)

	-w maxchars
		truncate input lines longer than maxchars


	If no output file is specified, vc will write to stdout.
	
	If, additionally, no input file is specified, vc will
	read input from stdin.

	A hyphen (-) can be used for <input>, if <output> 
	is a file, to read from stdin.



###Performance
Let's create a file with 10 million random two-digit hexadecimal numbers:

	dd if=/dev/urandom count=10k bs=1k | od -tx1 -An | tr -cs "[:xdigit:]" "\n" > integers.txt

This is exactly the type of file I had in mind when I wrote `vc`. 
There are only 256 distinct, short lines, each occurring many times.

On my system, the traditional `sort | uniq -c` approach takes a while:

	time sort integers.txt | uniq -c
	->
		...
		41342 fc
		40733 fd
		40994 fe
		41001 ff

		real	2m25.495s
		user	2m29.969s
		sys	0m0.413s

`vc` is approximately 140 times faster:

	time vc -t integers.txt
	->
		...
		41342 fc
		40733 fd
		40994 fe
		41001 ff

		real	0m1.024s
		user	0m1.011s
		sys	0m0.011s


However, since `vc` stores every unique line in memory (plus some overhead for the hash table),
it may perform poorly for inputs with a huge number of distinct lines, or with very long
lines (using the -w option may help in the second case).