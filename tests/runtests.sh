#/bin/sh

testfile=testfile.txt
tmpinput=input.txt
tmpexpected=expected.txt
tmpoutput=output.txt
ntests=0
npass=0
nfail=0

set -u -e

check_output() 
{
	status=$?
	if [ $status -ne 0 ]
	then
		echo "FAIL"
		echo "vc exited with status" "$status"
		pass=0
		nfail=$(($nfail+1))
	fi

	DIFFS=`diff "$tmpexpected" "$tmpoutput"`
	if [[ $DIFFS != "" ]] ;
	then
		echo "FAIL"
		diff -y "$tmpexpected" "$tmpoutput"
		nfail=$(($nfail+1))
	else
		echo "PASS"
		npass=$(($npass+1))
	fi
}

while IFS= read -r line || [[ -n $line ]]
do
	if [[ $line == @test* ]] ;
	then
		ntests=$((ntests+1))
		
		echo "running test:" ${line#@test}
		read -r line
		if [[ $line != @arguments* ]]; then
			echo "Expected @arguments"
			exit 1
		fi

		args=${line#@arguments}
		echo "arguments: " $args
		read -r line

		> $tmpinput
		while IFS= read -r line && [[ $line != @output* ]]
		do
			echo "$line" >> $tmpinput
		done

		> $tmpexpected
		while IFS= read -r line && [[ $line != @end* ]]
		do
			echo "$line" >> $tmpexpected
		done

		set +e
		echo "Testing with file/file... "
		rm -f $tmpoutput
		../vc $args $tmpinput $tmpoutput
		check_output


		echo "Testing with stdin/stdout... "
		rm -f $tmpoutput
		cat $tmpinput | ../vc $args > $tmpoutput
		check_output

		echo "Testing with file/stdout..."
		rm -f $tmpoutput
		cat $tmpinput | ../vc $args "-" $tmpoutput
		check_output

		echo "Testing with stdin/file.. "
		rm -f $tmpoutput
		cat $tmpinput | ../vc $args "-" $tmpoutput
		check_output
		set -e

	fi
done < $testfile


echo
echo $npass "tests passed"
echo $nfail "tests failed"

rm -f $tmpinput $tmpoutput $tmpexpected

if [ $nfail -eq 0 ]
then
	exit
else
	exit 1
fi