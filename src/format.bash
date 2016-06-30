#!/bin/bash

(echo n; echo p; echo 1; echo; echo; echo t; echo fd; echo w) |sudo fdisk "$1"

exit "$?" # return 0 if no error
