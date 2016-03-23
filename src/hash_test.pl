#!/usr/bin/perl

# Run some hashing tests.


my $table_size = 16384;


sub cb_hash
{
	# cbloom hash
	my ($x, $z, $l) = @_;
	$x = $x + 0;
	$z = $z + 0;
	$l = $l + 0;

	$x = $x >> $l;
	$z = $z >> $l;

	return $x + $z * 97 + $l * 193;
}


sub tu_hash
{
	# "tulrich hash"
	my ($x, $z, $l) = @_;
	$x = $x + 0;
	$z = $z + 0;
	$l = $l + 0;

	my $xc = $x >> $l;
	my $zc = $z >> $l;
	$xc = (($xc) ^ (($xc) << 4)) & 127;
	$zc = (((($zc) ^ (($zc) << 2)) & 127)) * 128;
	return ($xc + $zc + $l * 13);
}


sub db_hash
{
	# djb hash
	my ($x, $z, $l) = @_;
	$x = $x + 0;
	$z = $z + 0;
	$l = $l + 0;

	my @bytes;

	push(@bytes, ($x >> 24) & 255);
	push(@bytes, ($x >> 16) & 255);
	push(@bytes, ($x >> 8) & 255);
	push(@bytes, $x & 255);

	push(@bytes, ($z >> 24) & 255);
	push(@bytes, ($z >> 16) & 255);
	push(@bytes, ($z >> 8) & 255);
	push(@bytes, $z & 255);

	push(@bytes, ($l >> 24) & 255);
	push(@bytes, ($l >> 16) & 255);
	push(@bytes, ($l >> 8) & 255);
	push(@bytes, $l & 255);

	my $h = 0;
	for $b (@bytes)
	{
		$h = ((($h << 5) + $h) ^ $b) & ($table_size - 1);
	}

	return $h;
}


sub hash {
	return cb_hash(@_);
#	return tu_hash(@_);
#	return db_hash(@_);
}


# Init the table.
my @table;
for ($i = 0; $i < $table_size; $i++)
{
	push(@table, 0);
}


# Read keys from stdin, and total up occupancy.
my $input_count = 0;
while ($line = <>)
{
	my ($x, $y, $l) = split(" ", $line);
	my $h = hash($x, $y, $l) & ($table_size - 1);
	$table[$h] ++;
	$input_count++;
}


my $collisions = 0;
my $max_occupancy = 0;
for $count (@table) {
	if ($count > 1)
	{
		$collisions += $count - 1;
	}
	if ($count > $max_occupancy)
	{
		$max_occupancy = $count;
	}
}


print "inp = $input_count, coll = $collisions, max_occ = $max_occupancy\n";



# results:
#
# cb_hash: inp = 434, coll = 10, max_occ = 2
# tu_hash: inp = 434, coll = 3, max_occ = 2
# db_hash: inp = 434, coll = 9, max_occ = 2


# Sample key data from the Soul Ride texture cache for a typical scene.
# "x z level" for the active texture blocks.
#
# Copy this data to e.g. "hash_keys.txt" and do "./hash_test.pl < hash_keys.txt"
$TEST_DATA = <<EOQ;
33088 36096 6
32384 35584 7
34816 28672 11
30720 34816 11
34816 30720 11
33024 36352 7
32896 36288 5
28672 28672 11
33152 36256 5
33008 36336 4
26624 22528 11
33792 34816 9
49152 16384 13
32768 0 13
45056 16384 12
33024 36160 6
16384 20480 12
27648 32768 10
37888 30720 10
57344 8192 13
33408 35712 7
33024 35328 8
34560 34816 8
32992 36384 4
24576 12288 12
32256 34816 9
32768 32768 13
12288 16384 12
33040 36368 4
32256 35840 8
33072 36400 4
33120 36336 4
31744 35584 8
32768 35840 9
32768 33792 9
38912 29696 10
33664 35584 7
32768 31744 10
32944 36352 4
32768 29696 10
29696 34304 9
32704 36032 6
32976 36384 4
32768 34816 8
33088 36288 5
32640 35584 7
32256 35840 7
33104 36352 4
33280 35840 7
33056 36432 4
32768 22528 11
33024 35584 8
32992 36288 5
36864 26624 11
33040 36416 4
36864 24576 11
30720 28672 11
32896 36352 7
33120 36320 5
33280 35968 7
33152 35712 7
24576 30720 11
24576 28672 11
32960 36256 5
32768 16384 12
53248 12288 12
8192 8192 12
28672 20480 12
31744 32768 10
33792 30720 10
31744 34816 10
32976 36368 4
32832 36224 6
32960 36096 6
33024 36032 6
34816 33280 9
20480 12288 12
33024 36336 4
33024 36352 4
32768 36160 6
31744 33792 9
31744 35328 8
33056 36384 4
0 0 16
32992 36336 4
30720 33792 10
34816 29696 10
34816 31744 10
28672 31744 10
28672 29696 10
33008 36384 4
35328 33792 9
33792 34304 9
33088 36160 6
32256 33280 9
30720 34816 9
33024 36320 5
32512 35968 7
33088 36368 4
32864 36256 5
33280 36096 7
33152 35840 7
31488 34816 8
33024 36096 6
40960 26624 11
33104 36336 4
32960 36288 5
40960 24576 11
45056 22528 11
33024 36432 4
38912 26624 11
32768 34816 11
32928 36320 5
33280 36224 7
26624 28672 11
26624 30720 11
33280 35072 8
33056 36288 5
49152 12288 12
33088 36224 6
32896 36032 6
40960 0 13
20480 16384 12
33536 35968 7
32896 35712 7
32768 35584 7
24576 20480 12
31744 35072 8
29696 32768 10
35840 32768 10
29696 30720 10
32960 36352 4
32768 24576 13
49152 8192 13
28672 8192 12
34304 34816 9
32768 33280 9
16384 12288 12
32928 36352 5
33792 35712 7
16384 8192 13
31232 34304 9
33088 36384 4
33120 36352 4
32000 35328 8
33792 35584 8
33024 35072 8
28672 33792 10
30720 29696 10
30720 31744 10
32992 36400 4
33280 35840 9
33280 33792 9
31488 35072 8
32512 36096 7
33040 36352 4
30208 33280 9
33120 36256 5
33280 34816 8
32512 35712 7
33072 36384 4
43008 24576 11
31744 34816 8
32896 36256 5
45056 12288 12
33152 36288 5
33152 36224 7
32768 8192 12
33024 35584 7
28672 26624 11
28672 24576 11
22528 28672 11
33344 36096 6
36864 20480 12
32976 36336 4
40960 16384 12
32768 36224 6
32896 36096 6
33088 36032 6
12288 12288 12
32992 36416 4
32896 35968 7
32768 35840 7
16384 24576 12
33536 35712 7
33792 34816 10
33792 32768 10
31744 30720 10
8192 16384 12
32000 35584 8
32832 36160 6
16384 16384 14
33792 33792 9
33072 36368 4
32704 36224 6
33792 35328 8
37888 29696 10
32256 33792 9
32256 35840 9
33088 36336 4
33040 36400 4
31488 35328 8
32768 35840 10
26624 29696 10
32976 36352 4
26624 31744 10
32256 35712 7
32512 35840 7
33536 35840 7
32640 36096 6
33024 36224 6
33024 36368 4
32768 34816 9
34304 33280 9
33104 36384 4
33216 36096 6
32256 35584 7
33056 36400 4
33280 35584 7
32768 30720 10
32704 36160 6
33024 36256 5
31232 34816 9
32992 36320 5
32768 26624 11
33120 36288 5
33152 36160 6
32768 24576 11
40960 12288 12
20480 22528 11
32960 36224 5
33152 35968 7
33280 35712 7
30720 24576 11
30720 26624 11
33088 36320 5
32960 36032 6
32768 20480 12
40960 8192 13
28672 32768 12
36864 24576 12
53248 8192 12
33280 36032 6
8192 12288 12
32896 36224 7
32768 36096 7
49152 0 13
28672 16384 12
33792 35072 8
33008 36368 4
24576 24576 13
20480 8192 12
0 16384 13
32256 35072 8
33056 36352 4
33280 34304 9
33792 31744 10
33792 29696 10
34048 35328 8
31744 33280 9
33280 35328 8
33024 36384 4
32960 36336 4
32384 35968 7
33008 36416 4
32768 36096 8
32960 36368 4
33536 35072 8
33072 36336 4
32512 35584 7
33408 35840 7
33536 35584 7
33152 36224 5
33088 36400 4
32000 35072 8
32864 36288 5
33024 35712 7
30720 32768 10
28672 30720 10
32832 36256 5
34816 24576 11
32928 36224 5
34816 26624 11
49152 16384 12
32384 35840 7
33056 36256 5
22528 22528 11
32896 36320 5
57344 8192 12
32256 34304 9
20480 26624 11
20480 24576 11
45056 20480 12
32256 34816 8
32768 32768 12
16384 16384 12
33408 35968 7
32896 36160 6
24576 8192 12
32992 36352 4
12288 20480 12
32960 36224 6
32832 36096 6
33152 36032 6
35840 29696 10
29696 33792 10
35840 31744 10
29696 31744 10
29696 29696 10
29696 33280 9
35840 33280 9
34304 33792 9
33536 35328 8
31232 33280 9
32640 36160 6
33792 35584 7
33216 36160 6
32704 36096 6
32640 35968 7
33408 36096 7
33152 36096 6
32832 36288 5
33072 36352 4
28672 32768 10
32256 35584 8
34816 32768 10
30720 30720 10
33280 34816 9
34816 34304 9
32896 36224 5
36864 30720 11
33152 36320 5
36864 28672 11
24576 26624 11
32992 36256 5
33024 35840 7
32768 35712 7
24576 24576 11
32896 35584 7
33040 36384 4
28672 22528 11
34048 34816 8
33344 36032 6
22528 24576 11
30208 34304 9
22528 26624 11
36864 16384 12
31744 34816 9
40960 20480 12
32944 36336 4
33152 36288 6
32768 8192 13
12288 8192 12
32944 36368 4
24576 32768 13
57344 0 13
32976 36400 4
0 8192 13
33056 36336 4
32768 0 15
33536 35584 8
33104 36368 4
33056 36416 4
0 0 15
31744 29696 10
31744 31744 10
33792 33280 9
33040 36432 4
35328 33280 9
33008 36352 4
32256 35328 8
32640 35712 7
33056 36320 5
33056 36368 4
34048 35072 8
33024 36288 5
33024 36224 7
33152 35584 7
32768 34816 10
32864 36224 5
32768 32768 10
26624 30720 10
32768 34304 9
32512 35072 8
38912 28672 11
32960 36320 5
32768 28672 11
26624 24576 11
32768 35968 7
32896 35840 7
26624 26624 11
33024 36400 4
32928 36288 5
30720 22528 11
24576 22528 11
34304 34304 9
32768 35328 8
32832 36288 6
32960 36160 6
20480 20480 12
24576 16384 12
33008 36400 4
28672 12288 12
32960 36384 4
32896 36352 5
16384 8192 12
33216 36032 6
32768 16384 14
33664 35712 7
32000 35840 8
33024 36096 8
33088 36352 4
27648 29696 10
27648 31744 10
33024 36416 4
34816 33792 9
33280 33280 9
32384 35712 7
33664 35840 7
36864 29696 10
31744 34304 9
32992 36368 4
33040 36336 4
33408 35584 7
32512 35328 8
32864 36320 5
32640 35840 7
32832 36224 5
33024 35968 7
33072 36416 4
32768 35072 8
33152 36224 6
32832 36032 6
32928 36256 5
EOQ