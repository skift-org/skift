#!/bin/bash

r=$(cat $1)
r="${r//'%'/'%25'}"
r="${r//$'\n'/'%0A'}"
r="${r//$'\r'/'%0D'}"
echo "RELEASE_BODY=$r"
