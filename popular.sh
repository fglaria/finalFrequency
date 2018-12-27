#! /bin/bash

#$cliqueFolder = $1

for clique in $1/*
do
	echo $clique;
	./popular $clique > $clique.log 2> $clique.error
	mv $clique.* ../partitioned/.
done

