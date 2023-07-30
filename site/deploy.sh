#!/bin/bash

aws s3 cp . s3://trippinontubs/ --recursive \
--exclude "*" \
--include "favicon*" \
--include "*.html" \
--include "*.gif" \
--include "*.css" \
--include "*.map" \
--include "*.jpg"

aws cloudfront create-invalidation \
--distribution-id E3UAX08O8UI5X3 \
--paths /index.html /highscores.html /pico.min.css /pico.min.css.map /walking_tub.gif /favicon.ico