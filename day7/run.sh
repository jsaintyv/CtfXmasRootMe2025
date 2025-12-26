#!/bin/bash


docker context use default
docker build . -t jsy/wiilauncher
docker run -it -t jsy/wiilauncher