import csv
import sys
import os.path
import json

def parse_cad_points_to_json(file, region):
    output_string = ""
    with open(file, 'rb') as f:
        reader = csv.reader(f, delimiter=',')
        for row in reader:
            new_row = str(row)
            for ch in [" ","'","[","]"]:
                new_row = new_row.replace(ch,"")
            row_array = new_row.split(",")
            new_array = []
            for x in row_array:
                new_array.append(float(x)/1000)
            output_string += '{"point": ' + str(new_array) + ', "region": ' + region + '},\n'

    output_string = output_string[:-2]
    return output_string

# def create_json(input1, region1, input2, region2, input3, region3):
def create_json(filename, *arg):
    arg_length = len(arg)
    print arg_length
    if (arg_length > 1) and (arg_length % 2 != 0):
        print "Incorrect number of arguments"
        return        
    output_string = "[\n"

    if arg_length == 2:
        output_string += parse_cad_points_to_json(arg[1], "main region")
    else:
        for i in range(arg_length):
            if i % 2 != 0:
                continue
            else:
                output_string += parse_cad_points_to_json(arg[i], arg[i+1])

    json_file_name = input1.replace(".txt", ".json")

    output_string += "\n]"
    print output_string
    write_to_file = open(json_file_name, 'w')
    write_to_file.write(output_string)
    write_to_file.close()

    return json_file_name

if len(sys.argv) > 1:
    json_file = create_json(sys.argv)
    print json_file + " created"
else:
    print "Booooo-urns"



