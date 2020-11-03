#!/usr/bin/env bash

Instances=20
Experiment=model
TotalSamples=1000000
ExperimentDirectory=./results/${Host}/${Experiment}
Host="$(hostname)"

SamplesPerInstance=$(( TotalSamples / Instances))

echo "Samples Per Instance: $SamplesPerInstance"
mkdir -p $ExperimentDirectory

date | tee $ExperimentDirectory/begin.txt
for i in $(seq 0 $(( Instances - 1 ))) ; do
    Offset=$(( SamplesPerInstance * i ))
    ResultPath=$ExperimentDirectory/$(printf "%02d" $i).txt
    ./$Experiment --iterations $SamplesPerInstance --skip $Offset 2> /dev/null > $ResultPath &
done

# Wait until all processes finish
wait
date | tee $ExperimentDirectory/end.txt


# vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4