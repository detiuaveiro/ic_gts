#!/bin/bash

if [ ! -d "movies" ]; then
  mkdir movies
fi

cd movies

if [ ! -f "sintel_trailer_2k_480p24.y4m" ]; then
  wget https://media.xiph.org/video/derf/y4m/sintel_trailer_2k_480p24.y4m
else
  echo "File already exists"
  exit 1
fi
