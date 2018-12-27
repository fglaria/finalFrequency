#! /bin/bash

#$cliqueFolder = $1

for clique in $1/*
do
	echo $clique;
	IFS='/' read -ra pathArray <<< "$clique"
	#echo ${pathArray[-1]}
	#$fileName = ${pathArray[-1]}
	./compress $2/${pathArray[-1]} > $clique.clog 2> $clique.cerror
	mv $clique.* ../compressed/.
	mv $2/*.sdsl ../compressed/.
	#break
done

