#!/bin/bash

if [ ! -d "movies" ]; then
  mkdir movies
fi

cd movies

if [ ! -f "sintel_trailer_2k_480p24.y4m" ]; then
  wget https://media.xiph.org/video/derf/y4m/sintel_trailer_2k_480p24.y4m
else
  echo "Sintel already exists"
fi

if [ ! -f "blue_sky_1080p25.y4m" ]; then
  wget https://media.xiph.org/video/derf/y4m/blue_sky_1080p25.y4m
else
  echo "Blue sky File already exists"
fi
