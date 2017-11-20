#When executed, create 3 files in the same directory as your script
#Ech named differently (the name of the files is up to you)
#which remain there after your script finishes executing. 
#Each of these 3 files must contain exactly 10 random characters from the lowercase alphabet, with no spaces 
#("hoehdgwkdq", for example). The final (eleventh) character of each file should be a newline character.

#When executed, print out on screen the contents of the 3 files it is creating.
#When executed, after the file contents of all three files have been printed
#Print out two random integers (whose range is from 1 to 42, inclusive)
#Print out the product of the two numbers.

import random
import string    
import sys



items = ["one", "two", "three" ]

for item in items:
 fo = open("%s_hello_world.txt" % item, "w+")
 for x in range(0, 10):
     rand = random.choice(string.ascii_lowercase)
     fo.write("%s" % (rand))
     sys.stdout.write(str(rand))
     if x == 9: 
        fo.write("\n")
	print  		
fo.close()


#Getting random integers betwee 1 and 42
x = random.randint(1,42)
y = random.randint(1,42)

#printing them out
print(x)
print(y)
print(x * y) 
