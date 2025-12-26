#!/bin/bash

docker build . -t jsy/xmasday13
docker run -it --volume .:/app -m 1G -t jsy/xmasday13