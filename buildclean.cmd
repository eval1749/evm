@echo off
del/s/q Debug Release ipch
del/s/q/a:h Debug Release ipch
rmdir/s/q Debug Release ipch
del/q *.sdf
del/q *~
del/q rep.txt
del/q/a:h *.suo 
