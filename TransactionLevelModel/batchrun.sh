#!/usr/bin/env bash

Instances=20
Experiment=jpeg3
TotalSamples=100
Host="$(hostname)"
ExperimentDirectory=./results/${Host}/${Experiment}

SamplesPerInstance=$(( TotalSamples / Instances))

echo "Samples Per Instance: $SamplesPerInstance"
mkdir -p $ExperimentDirectory

date | tee $ExperimentDirectory/begin.txt
for i in $(seq 0 $(( Instances - 1 ))) ; do
    ResultPath=$ExperimentDirectory/$(printf "%02d" $i).txt
    ./model --experiment $Experiment --iterations $SamplesPerInstance 2> /dev/null > $ResultPath &
done

# Wait until all processes finish
wait
date | tee $ExperimentDirectory/end.txt


# vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

