#!/usr/local/bin/python3

import cgi
import pathlib

# Create instance of FieldStorage
form = cgi.FieldStorage()

# Get data from fields
if form.getvalue('textcontent'):
    text = form.getvalue('textcontent')
else:
    text = "Not entered"

f1 = open(str(pathlib.Path(__file__).parent.absolute()) + "/first_part.html")
f2 = open(str(pathlib.Path(__file__).parent.absolute()) + "/last_part.html")

first_part = f1.read()
last_part = f2.read()

f1.close()
f2.close()

print(first_part)
print(text)
print(last_part)
