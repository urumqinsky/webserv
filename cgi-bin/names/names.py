#!/usr/local/bin/python3

import cgi
import pathlib


# Create instance of FieldStorage
form = cgi.FieldStorage()

# Get data from fields
first_name = form.getvalue('firstname', 'none')
last_name = form.getvalue('lastname', 'none')

f1 = open(str(pathlib.Path(__file__).parent.absolute()) + "/first_part.html")
f2 = open(str(pathlib.Path(__file__).parent.absolute()) + "/last_part.html")

first_part = f1.read()
last_part = f2.read()

f1.close()
f2.close()


print(first_part)
print("<div>")
print("<p class=\"first_form\">")
print("First field: %s<br>" % first_name)
print("Second field: %s<br>" % last_name)
print("</p>")
print("</div>")
print(last_part)