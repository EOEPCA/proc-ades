#!/usr/bin/env bash


mkdir -p 3ty/ && cd 3ty

if [[ ! -d "yaml-cpp-yaml-cpp-0.6.3" ]]
then
    curl -L   https://github.com/jbeder/yaml-cpp/archive/yaml-cpp-0.6.3.tar.gz -o yaml-cpp-0.6.3.tar.gz
    tar -zxvf yaml-cpp-0.6.3.tar.gz
    rm -f yaml-cpp-0.6.3.tar.gz
else
    echo 'yaml-cpp-yaml-cpp-0.6.3 exist!'
fi


